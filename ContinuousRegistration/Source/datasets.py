import os, glob, csv, re
from abc import ABCMeta, abstractmethod
from itertools import combinations
import numpy as np

from metrics import tre, hausdorff, inverse_consistency_labels, inverse_consistency_points, \
    dice, merge_dicts

class Dataset(object):
    __metaclass__ = ABCMeta

    @abstractmethod
    def generator(self):
        pass

    @abstractmethod
    def evaluate(self):
        pass

    def evaluate_point_set(self, superelastix, file_names):
        tre_0, tre_1 = tre(superelastix, file_names['ground_truth_file_names'], file_names['displacement_field_file_names_full_path'])
        hausdorff_0, hausdorff_1 = hausdorff(superelastix, file_names['ground_truth_file_names'], file_names['displacement_field_file_names_full_path'])
        inverse_consistency_points_0, inverse_consistency_points_1 = inverse_consistency_points(superelastix, file_names['ground_truth_file_names'], file_names['displacement_field_file_names_full_path'])

        return (merge_dicts(tre_0, hausdorff_0, inverse_consistency_points_0),
                merge_dicts(tre_1, hausdorff_1, inverse_consistency_points_1))

    # Default evaluation method for label ground truths
    def evaluate_label_image(self, superelastix, file_names):
        inverse_consistency_atlas_0, inverse_consistency_atlas_1 = inverse_consistency_labels(superelastix, file_names['ground_truth_file_names'], file_names['displacement_field_file_names_full_path'])
        dice_0, dice_1 = dice(superelastix, file_names['ground_truth_file_names'], file_names['displacement_field_file_names_full_path'])

        return (merge_dicts(inverse_consistency_atlas_0, dice_0),
                merge_dicts(inverse_consistency_atlas_1, dice_1))

    def make_shell_scripts(self, superelastix, blueprint_file_name, file_names, output_directory):
        if not os.path.exists(os.path.join(output_directory, 'sh')):
            os.mkdir(os.path.join(output_directory, 'sh'))

        # Moving to fixed
        self.make_shell_script(superelastix,
                               blueprint_file_name,
                               file_names['image_file_names'],
                               file_names['displacement_field_file_names'],
                               output_directory)

        # Fixed to moving
        self.make_shell_script(superelastix,
                               blueprint_file_name,
                               (file_names['image_file_names'][1], file_names['image_file_names'][0]),
                               (file_names['displacement_field_file_names'][1], file_names['displacement_field_file_names'][0]),
                               output_directory)


    def make_shell_script(self, superelastix, blueprint_file_name, image_file_names, displacement_field_file_names, output_directory):
        shell_script_file_name = os.path.join(output_directory, 'sh', os.path.splitext(displacement_field_file_names[0])[0] \
            .replace('/', '_').replace('\\', '_').replace('.', '_') + '.sh')

        with open(shell_script_file_name, 'w') as shell_script:
            shell_script.write('%s --conf %s --in FixedImage=%s MovingImage=%s --out DisplacementField=%s --loglevel trace --logfile %s' % (
                superelastix,
                blueprint_file_name,
                image_file_names[0],
                image_file_names[1],
                os.path.join(output_directory, displacement_field_file_names[0]),
                os.path.splitext(shell_script_file_name)[0] + '.log'))

    def make_batch_scripts(self):
        pass


class CUMC12(Dataset):
    def __init__(self, input_directory):
        self.name = 'CUMC12'
        self.category = 'Brain'

        self.input_directory = input_directory
        self.image_file_names = []
        self.label_file_names = []
        self.displacement_field_file_names = []

        image_file_names = [os.path.join(input_directory, 'Heads', image) for image in os.listdir(os.path.join(input_directory, 'Heads')) if image.endswith('.hdr')]
        self.image_file_names = [pair for pair in combinations(image_file_names, 2)]

        atlas_file_names = [os.path.join(input_directory, 'Atlases', atlas) for atlas in os.listdir(os.path.join(input_directory, 'Atlases')) if atlas.endswith('.hdr')]
        self.label_file_names = [pair for pair in combinations(atlas_file_names, 2)]

        for image_file_name_0, image_file_name_1 in self.image_file_names:
            image_file_name_0 = os.path.basename(image_file_name_0)
            image_file_name_1 = os.path.basename(image_file_name_1)
            image_file_name_we_0, image_extension_we_0 = os.path.splitext(image_file_name_0)
            image_file_name_we_1, image_extension_we_1 = os.path.splitext(image_file_name_1)
            self.displacement_field_file_names.append((os.path.join(self.name, image_file_name_we_1 + "_to_" + image_file_name_we_0 + ".nii"),
                                                       os.path.join(self.name, image_file_name_we_0 + "_to_" + image_file_name_we_1 + ".nii")))


    def generator(self):
        for image_file_names, label_file_names, displacement_field_file_names in zip(self.image_file_names, self.label_file_names, self.displacement_field_file_names):
            yield {
                "image_file_names": image_file_names,
                "ground_truth_file_names": label_file_names,
                "displacement_field_file_names": displacement_field_file_names
            }


    def evaluate(self, superelastix, file_names):
        return self.evaluate_label_image(superelastix, file_names)


class DIRLAB(Dataset):
    def __init__(self, input_directory):
        self.name = 'DIRLAB'
        self.category = 'Lung'

        self.input_directory = input_directory
        self.image_file_names = []
        self.point_set_file_names = []
        self.displacement_field_file_names = []

        # DIR-LAB provides raw binary image only so we write mhd header files for loading the data.
        # The image information was retrieved from https://www.dir-lab.com/ReferenceData.html
        dirlab_image_information_file_name = os.path.join(os.path.dirname(__file__), "dirlab_image_information.csv")

        # Find corresponding subdirectory which contains the id and associate it with this line of the csv file.
        # TODO: Maybe not the prettiest approach
        sub_directories = [directory for directory in os.listdir(self.input_directory) if os.path.isdir(os.path.join(input_directory, directory))]
        sub_directory_ids = list(map(lambda str: int(re.search(r'\d+', str).group()), sub_directories))
        dirlab_image_information = dict()
        for image_information in csv.DictReader(open(dirlab_image_information_file_name)):
            dirlab_image_information[image_information['id']] = image_information
            dirlab_image_information[image_information['id']]['sub_directory'] = sub_directories[sub_directory_ids.index(int(image_information['id']))]

        # Now we have all the information necessary so generate the header files
        for id in dirlab_image_information:
            img_0_file_name = glob.glob(os.path.join(input_directory, dirlab_image_information[id]['sub_directory'], 'Images', '*T00*.img'))[0]
            img_1_file_name = glob.glob(os.path.join(input_directory, dirlab_image_information[id]['sub_directory'], 'Images', '*T50*.img'))[0]

            # Write mhd header files
            mhd_0_file_name = os.path.splitext(img_0_file_name)[0] + '.mhd'
            with open(mhd_0_file_name, 'w') as mhd:
                mhd.write('ObjectType = Image\n')
                mhd.write('NDims = 3\n')
                mhd.write('BinaryData = True\n')
                mhd.write('DimSize = %s %s %s\n' % (dirlab_image_information[id]['x_size'], dirlab_image_information[id]['y_size'], dirlab_image_information[id]['z_size']))
                mhd.write('ElementSpacing = %s %s %s\n' % (dirlab_image_information[id]['x_spacing'], dirlab_image_information[id]['y_spacing'], dirlab_image_information[id]['z_spacing']))
                mhd.write('ElementType = MET_USHORT\n')
                mhd.write('ElementDataFile = %s\n' % img_0_file_name)

            mhd_1_file_name = os.path.splitext(img_1_file_name)[0] + '.mhd'
            with open(mhd_1_file_name, 'w') as mhd:
                mhd.write('ObjectType = Image\n')
                mhd.write('NDims = 3\n')
                mhd.write('BinaryData = True\n')
                mhd.write('DimSize = %s %s %s\n' % (dirlab_image_information[id]['x_size'], dirlab_image_information[id]['y_size'], dirlab_image_information[id]['z_size']))
                mhd.write('ElementSpacing = %s %s %s\n' % (dirlab_image_information[id]['x_spacing'], dirlab_image_information[id]['y_spacing'], dirlab_image_information[id]['z_spacing']))
                mhd.write('ElementType = MET_USHORT\n')
                mhd.write('ElementDataFile = %s\n' % img_1_file_name)

            self.image_file_names.append((mhd_0_file_name, mhd_1_file_name))

            point_set_0 = glob.glob(os.path.join(input_directory, dirlab_image_information[id]['sub_directory'], 'ExtremePhases', '*T00_xyz.txt'))[0]
            point_set_1 = glob.glob(os.path.join(input_directory, dirlab_image_information[id]['sub_directory'], 'ExtremePhases', '*T50_xyz.txt'))[0]
            self.point_set_file_names.append((point_set_0, point_set_1))

            displacement_field_file_name_0 = os.path.join(self.name, dirlab_image_information[id]['sub_directory'], '50_to_00.nii')
            displacement_field_file_name_1 = os.path.join(self.name, dirlab_image_information[id]['sub_directory'], '00_to_50.nii')
            self.displacement_field_file_names.append((displacement_field_file_name_0, displacement_field_file_name_1))



    def generator(self):
        for image_file_names, point_set_file_names, displacement_field_file_names in zip(self.image_file_names, self.point_set_file_names, self.displacement_field_file_names):
            yield {
                "image_file_names": image_file_names,
                "ground_truth_file_names": point_set_file_names,
                "displacement_field_file_names": displacement_field_file_names
            }


    def evaluate(self, superelastix, file_names):
        return self.evaluate_point_set(superelastix, file_names)

class EMPIRE(Dataset):
    # input_directory should point to the 'scans' directory. We do not use the lung masks.
    def __init__(self, input_directory):
        self.name = 'EMPIRE'
        self.category = 'Lung'

        self.image_file_names = []
        self.point_set_file_names = []
        self.displacement_field_file_names = []

        for i in range(1, 31):
            self.image_file_names.append((os.path.join(input_directory, 'scans', "%02d" % i + '_Fixed.mhd'),
                                          os.path.join(input_directory, 'scans', "%02d" % i + '_Moving.mhd')))

            # TODO: Find out output format
            self.displacement_field_file_names.append((os.path.join(self.name, "%02d" % i + '_Moving_to_Fixed.mhd'),
                                                      os.path.join(self.name, "%02d" % i + '_Fixed_to_Moving.mhd')))


    def generator(self):
        for image_file_names, displacement_field_file_names in zip(self.image_file_names, self.displacement_field_file_names):
            yield {
                "image_file_names": image_file_names,
                "displacement_field_file_names": displacement_field_file_names
            }


    def evaluate(self, superelastix, file_names):
        pass


class ISBR18(Dataset):
    def __init__(self, input_directory):
        self.name = 'ISBR18'
        self.category = 'Brain'

        self.input_directory = input_directory
        self.image_file_names = []
        self.atlas_file_names = []
        self.displacement_field_file_names = []

        image_file_names = [os.path.join(input_directory, 'Heads', image) for image in os.listdir(os.path.join(input_directory, 'Heads')) if image.endswith('.hdr')]
        self.image_file_names = [pair for pair in combinations(image_file_names, 2)]

        atlas_file_names = [os.path.join(input_directory, 'Atlases', atlas) for atlas in os.listdir(os.path.join(input_directory, 'Atlases')) if atlas.endswith('.hdr')]
        self.atlas_file_names = [pair for pair in combinations(atlas_file_names, 2)]

        for image_file_name_0, image_file_name_1 in self.image_file_names:
            image_file_name_0 = os.path.basename(image_file_name_0)
            image_file_name_1 = os.path.basename(image_file_name_1)
            image_file_name_we_0, image_extension_we_0 = os.path.splitext(image_file_name_0)
            image_file_name_we_1, image_extension_we_1 = os.path.splitext(image_file_name_1)
            self.displacement_field_file_names.append((os.path.join(self.name, image_file_name_we_1 + "_to_" + image_file_name_we_0 + ".nii"),
                                                      os.path.join(self.name, image_file_name_we_0 + "_to_" + image_file_name_we_1 + ".nii")))


    def generator(self):
        for image_file_names, atlas_file_names, displacement_field_file_names in zip(self.image_file_names, self.atlas_file_names, self.displacement_field_file_names):
            yield {
                "image_file_names": image_file_names,
                "ground_truth_file_names": atlas_file_names,
                "displacement_field_file_names": displacement_field_file_names
            }


    def evaluate(self, superelastix, file_names):
        return self.evaluate_label_image(superelastix, file_names)


class LPBA40(Dataset):
    def __init__(self, input_directory):
        self.name = 'LPBA40'
        self.category = 'Brain'

        self.input_directory = input_directory
        self.image_file_names = []
        self.atlas_file_names = []
        self.displacement_field_file_names = []

        image_file_names = [glob.glob(os.path.join(self.input_directory, 'delineation_space', sub_directory, '*.delineation.skullstripped.hdr'))[0] for sub_directory in os.listdir(os.path.join(self.input_directory, 'delineation_space')) if os.path.isdir(os.path.join(self.input_directory, 'delineation_space', sub_directory))]
        self.image_file_names = [pair for pair in combinations(image_file_names, 2)]

        atlas_file_names = [glob.glob(os.path.join(self.input_directory, 'delineation_space', sub_directory, '*.delineation.skullstripped.hdr'))[0] for sub_directory in os.listdir(os.path.join(self.input_directory, 'delineation_space')) if os.path.isdir(os.path.join(self.input_directory, 'delineation_space', sub_directory))]
        self.atlas_file_names = [pair for pair in combinations(atlas_file_names, 2)]

        for image_file_name_0, image_file_name_1 in self.image_file_names:
            image_file_name_0 = os.path.basename(image_file_name_0)
            image_file_name_1 = os.path.basename(image_file_name_1)
            image_file_name_we_0 = os.path.splitext(image_file_name_0)[0]
            image_file_name_we_1 = os.path.splitext(image_file_name_1)[0]
            self.displacement_field_file_names.append((os.path.join(self.name, image_file_name_we_1 + "_to_" + image_file_name_we_0 + ".nii"),
                                                      os.path.join(self.name, image_file_name_we_0 + "_to_" + image_file_name_we_1 + ".nii")))


    def generator(self):
        for image_file_names, atlas_file_names, displacement_field_file_names in zip(self.image_file_names, self.atlas_file_names, self.displacement_field_file_names):
            yield {
                "image_file_names": image_file_names,
                "ground_truth_file_names": atlas_file_names,
                "displacement_field_file_names": displacement_field_file_names
            }


    def evaluate(self, superelastix, file_names):
        return self.evaluate_label_image(superelastix, file_names)


class POPI(Dataset):
    def __init__(self, input_directory):
        self.name = 'POPI'
        self.category = 'Lung'

        self.input_directory = input_directory
        self.image_file_names = []
        self.image_mask_file_names = []
        self.point_set_file_names = []
        self.displacement_field_file_names = []

        sub_directories = [directory for directory in os.listdir(self.input_directory) if os.path.isdir(os.path.join(input_directory, directory))]

        for sub_directory in sub_directories:
            self.image_file_names.append((os.path.join(input_directory, sub_directory, 'mhd', '00.mhd'),
                                          os.path.join(input_directory, sub_directory, 'mhd', '50.mhd')))
            # self.mask_file_names.append((os.path.join(input_directory, sub_directory, 'mhd', '00.mhd'),
            #                              os.path.join(input_directory, sub_directory, 'mhd', '50.mhd')))
            self.point_set_file_names.append((os.path.join(input_directory, sub_directory, 'pts', '00.pts'),
                                              os.path.join(input_directory, sub_directory, 'pts', '50.pts')))
            self.displacement_field_file_names.append((os.path.join(self.name, sub_directory, '50_to_00.mhd'),
                                                      os.path.join(self.name, sub_directory, '00_to_50.mhd')))


    def generator(self):
        for image_file_names, point_set_file_names, displacement_field_file_names in zip(self.image_file_names, self.point_set_file_names, self.displacement_field_file_names):
            yield {
                "image_file_names": image_file_names,
                "ground_truth_file_names": point_set_file_names,
                "displacement_field_file_names": displacement_field_file_names
            }

    def evaluate(self, superelastix, file_names):
        return self.evaluate_point_set(superelastix, file_names)


class SPREAD(Dataset):
    def __init__(self, input_directory):
        self.name = 'SPREAD'
        self.category = 'Lung'

        self.input_directory = input_directory
        self.image_file_names = []
        self.point_set_file_names = []
        self.relative_displacement_field_file_names = []

        sub_directories = [directory for directory in os.listdir(self.input_directory) if os.path.isdir(os.path.join(input_directory, directory))]

        for sub_directory in sub_directories:
            self.image_file_names.append((os.path.join(input_directory, 'mhd', sub_directory, 'baseline_1.mha'),
                                          os.path.join(input_directory, 'mhd', sub_directory, 'followup_1.mha')))

            baseline_point_set_file_name_we = os.path.join(input_directory, 'ground_truth', 'distinctivePoints', sub_directory + '_baseline_1_Cropped_point')
            point_set = np.loadtxt(baseline_point_set_file_name_we + '.txt', skiprows=2)
            np.savetxt(baseline_point_set_file_name_we + '_without_header.txt', point_set)

            follow_up_point_set_file_name_we = os.path.join(input_directory, 'ground_truth', 'annotate', 'Consensus', sub_directory, sub_directory + '_b1f1_point')
            point_set = np.loadtxt(follow_up_point_set_file_name_we + '.txt', skiprows=2)
            np.savetxt(follow_up_point_set_file_name_we + '_without_header.txt', point_set)

            self.point_set_file_names.append((baseline_point_set_file_name_we + '_without_header.txt',
                                              follow_up_point_set_file_name_we + '_without_header.txt'))

            self.displacement_field_file_names.append((os.path.join(self.name, sub_directory, 'followup_to_baseline.nii'),
                                                               os.path.join(self.name, sub_directory, 'baseline_to_followup.nii')))


    def generator(self):
        for image_file_names, point_set_file_names, displacement_field_file_names in zip(self.image_file_names, self.point_set_file_names, self.displacement_field_file_names):
            yield {
                "image_file_names": image_file_names,
                "ground_truth_file_names": point_set_file_names,
                "displacement_field_file_names": displacement_field_file_names
            }


    def evaluate(self, superelastix, file_names):
        self.evaluate_point_set(superelastix, file_names)

