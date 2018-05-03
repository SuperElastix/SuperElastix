import os, glob, csv, re
from abc import ABCMeta, abstractmethod
import numpy as np
from itertools import combinations

from ContinuousRegistration.Source.metrics import tre, hausdorff, inverse_consistency_labels, inverse_consistency_points, dice
from ContinuousRegistration.Source.util import take, sort_file_names, copy_information_from_images_to_labels, merge_dicts
from ContinuousRegistration.Source.util import create_mask_by_thresholding, create_mask_by_size


def create_list_displacement_field_names(image_file_names, name):
    displacement_field_names = []
    for name_0, name_1 in image_file_names:
        name_0 = os.path.basename(name_0)
        name_1 = os.path.basename(name_1)
        name_we_0, image_extension_we_0 = os.path.splitext(name_0)
        name_we_1, image_extension_we_1 = os.path.splitext(name_1)
        name_pair_1 = name_we_1 + "_to_" + name_we_0 + ".nii.gz"
        name_pair_0 = name_we_0 + "_to_" + name_we_1 + ".nii.gz"
        displacement_field_names.append((os.path.join(name, name_pair_1),
                                         os.path.join(name, name_pair_0)))
    return displacement_field_names


class Dataset(object):
    """
    Base class for datasets. The derived classes need only to implement the file
    layout on disk and how to evaluate the dataset.  Everything else is handled
    by this class. See metrics.py for available metrics.
    """
    __metaclass__ = ABCMeta

    def generator(self):
        for file_name in self.file_names:
            yield file_name

    def make_shell_scripts(self, superelastix, blueprint_file_name, file_names, output_directory):
        if not os.path.exists(os.path.join(output_directory, 'sh')):
            os.mkdir(os.path.join(output_directory, 'sh'))
        COMMAND_TEMPLATE = '%s --conf %s --in %s %s --out DisplacementField=%s --loglevel trace --logfile %s'

        # Fixed to moving
        root = os.path.splitext(file_names['displacement_field_file_names'][0])[0]
        shell_script_file_name = os.path.join(output_directory, 'sh',
                                              root.replace('/', '_').replace('\\', '_').replace('.', '_') + '.sh')

        if 'mask_file_names' not in file_names:
            file_names['mask_file_names'] = ('', '')
        with open(shell_script_file_name, 'w') as shell_script:
            shell_script.write(COMMAND_TEMPLATE % (superelastix, blueprint_file_name,
                'FixedImage=%s MovingImage=%s ' % tuple(file_names['image_file_names']),
                'FixedMask=%s MovingMask=%s' % tuple(file_names['mask_file_names']),
                os.path.join(output_directory, file_names['displacement_field_file_names'][0]),
                os.path.splitext(shell_script_file_name)[0] + '.log'))

        # Moving to fixed
        root = os.path.splitext(file_names['displacement_field_file_names'][1])[0]
        shell_script_file_name = os.path.join(output_directory, 'sh',
                                              root.replace('/', '_').replace('\\', '_').replace('.', '_') + '.sh')

        if 'mask_file_names' not in file_names:
            file_names['mask_file_names'] = ('', '')
        with open(shell_script_file_name, 'w') as shell_script:
            shell_script.write(COMMAND_TEMPLATE % (superelastix, blueprint_file_name,
                'FixedImage=%s MovingImage=%s ' % tuple(file_names['image_file_names'][::-1]),
                'FixedMask=%s MovingMask=%s' % tuple(file_names['mask_file_names'][::-1]),
                os.path.join(output_directory, file_names['displacement_field_file_names'][1]),
                os.path.splitext(shell_script_file_name)[0] + '.log'))

    def make_batch_scripts(self):
        pass

    #
    @abstractmethod
    def evaluate(self):
        """
        The derived class should either call evaluate_point_set,
        `evaluate_label_image` implement its own metrics, or combinations here
        """
        pass

    def evaluate_point_set(self, superelastix, file_names, output_directory):
        displacement_field_paths = (
            os.path.join(output_directory, file_names['displacement_field_file_names'][0]),
            os.path.join(output_directory, file_names['displacement_field_file_names'][1]),
        )

        tre_0, tre_1 = tre(superelastix, file_names['ground_truth_file_names'],
                           displacement_field_paths)
        hausdorff_0, hausdorff_1 = hausdorff(superelastix,
                                             file_names['ground_truth_file_names'],
                                             displacement_field_paths)
        inverse_consistency_points_0, inverse_consistency_points_1 = inverse_consistency_points(
            superelastix, file_names['ground_truth_file_names'], displacement_field_paths)

        return {
            file_names['displacement_field_file_names'][0]:
                merge_dicts(tre_0, hausdorff_0, inverse_consistency_points_0),
            file_names['displacement_field_file_names'][1]:
                merge_dicts(tre_1, hausdorff_1, inverse_consistency_points_1)
        }

    def evaluate_label_image(self, superelastix, file_names, output_directory):
        """ Default evaluation method for label ground truths

        :param superelastix:
        :param file_names:
        :param output_directory:
        :return:
        """
        displacement_field_paths = (
            os.path.join(output_directory, file_names['displacement_field_file_names'][0]),
            os.path.join(output_directory, file_names['displacement_field_file_names'][1]),
        )

        inverse_consistency_atlas_0, inverse_consistency_atlas_1 = inverse_consistency_labels(
            superelastix, file_names['ground_truth_file_names'], displacement_field_paths)

        dice_0, dice_1 = dice(superelastix, file_names['ground_truth_file_names'],
                              displacement_field_paths)

        return {
            file_names['displacement_field_file_names'][0]:
                merge_dicts(inverse_consistency_atlas_0, dice_0),
            file_names['displacement_field_file_names'][1]:
                merge_dicts(inverse_consistency_atlas_1, dice_1)
        }


class CUMC12(Dataset):
    def __init__(self, input_directory, output_directory, max_number_of_registrations):
        self.name = 'CUMC12'
        self.category = 'Brain'

        self.input_directory = input_directory
        file_names = []

        image_names = [os.path.join(input_directory, 'Heads', image)
                       for image in os.listdir(os.path.join(input_directory, 'Heads'))
                       if image.endswith('.hdr')]
        image_names = [pair for pair in combinations(image_names, 2)]

        label_names = [os.path.join(input_directory, 'Atlases', atlas)
                       for atlas in os.listdir(os.path.join(input_directory, 'Atlases'))
                       if atlas.endswith('.hdr')]
        label_names = [pair for pair in combinations(label_names, 2)]

        displacement_field_names = create_list_displacement_field_names(image_names, self.name)

        mask_names = [create_mask_by_thresholding(label, disp_field, output_directory, 0., 32, 16)
                      for label, disp_field in zip(label_names, displacement_field_names)]

        for image_file_name, mask_file_name, label_file_name, displacement_field_file_name \
                in zip(image_names, mask_names, label_names, displacement_field_names):
            file_names.append({
                'image_file_names': image_file_name,
                'mask_file_names': mask_file_name,
                'ground_truth_file_names': label_file_name,
                'displacement_field_file_names': displacement_field_file_name
            })

        self.file_names = take(sort_file_names(file_names),
                               max_number_of_registrations // 2)

    def evaluate(self, superelastix, file_names, output_directory):
        return self.evaluate_label_image(superelastix, file_names, output_directory)


class DIRLAB(Dataset):
    def __init__(self, input_directory, mask_directory, output_directory, max_number_of_registrations):
        self.name = 'DIRLAB'
        self.category = 'Lung'

        self.input_directory = input_directory
        file_names = []

        # DIR-LAB provides raw binary image only so we write mhd header files for loading the data.
        # The image information was retrieved from https://www.dir-lab.com/ReferenceData.html
        dirlab_image_information_file_name = os.path.join(os.path.dirname(__file__), "dirlab_image_information.csv")

        # Find corresponding subdirectory which contains the id and associate it with this line of the csv file.
        # TODO: Maybe not the prettiest approach
        sub_directories = [directory for directory in os.listdir(self.input_directory)
                           if os.path.isdir(os.path.join(input_directory, directory))]
        sub_directory_ids = list(map(lambda str: int(re.search(r'\d+', str).group()), sub_directories))
        dirlab_image_information = dict()
        for image_information in csv.DictReader(open(dirlab_image_information_file_name)):
            dirlab_image_information[image_information['id']] = image_information
            dirlab_image_information[image_information['id']]['sub_directory'] = \
                sub_directories[sub_directory_ids.index(int(image_information['id']))]

        def _write_mhd_file(img_file_name):
            mhd_file_name = os.path.join(output_directory, 'tmp', 'images',
                                         self.name,
                                         os.path.splitext(os.path.basename(
                                               img_file_name))[0] + '.mhd')
            with open(mhd_file_name, 'w') as mhd:
                mhd.write('ObjectType = Image\n')
                mhd.write('NDims = 3\n')
                mhd.write('BinaryData = True\n')
                mhd.write('DimSize = %s %s %s\n' % (
                dirlab_image_information[id]['x_size'],
                dirlab_image_information[id]['y_size'],
                dirlab_image_information[id]['z_size']))
                mhd.write('ElementSpacing = %s %s %s\n' % (
                dirlab_image_information[id]['x_spacing'],
                dirlab_image_information[id]['y_spacing'],
                dirlab_image_information[id]['z_spacing']))
                mhd.write('ElementType = MET_USHORT\n')
                mhd.write('ElementDataFile = %s\n' % img_file_name)
            return mhd_file_name

        # Now we have all the information necessary so generate the header files
        for id in dirlab_image_information:
            img_0_file_name = glob.glob(os.path.join(input_directory,
                                                     dirlab_image_information[id]['sub_directory'],
                                                     'Images', '*T00*.img'))[0]
            img_1_file_name = glob.glob(os.path.join(input_directory,
                                                     dirlab_image_information[id]['sub_directory'],
                                                     'Images', '*T50*.img'))[0]

            # Write mhd header files
            os.makedirs(os.path.join(output_directory, 'tmp', 'images', self.name), exist_ok=True)

            mhd_0_file_name = _write_mhd_file(img_0_file_name)
            mhd_1_file_name = _write_mhd_file(img_1_file_name)

            point_set_0 = glob.glob(os.path.join(input_directory,
                                                 dirlab_image_information[id]['sub_directory'],
                                                 'ExtremePhases', '*T00_xyz.txt'))[0]
            point_set_1 = glob.glob(os.path.join(input_directory,
                                                 dirlab_image_information[id]['sub_directory'],
                                                 'ExtremePhases', '*T50_xyz.txt'))[0]

            image_file_names = (mhd_0_file_name, mhd_1_file_name)
            point_set_file_names = (point_set_0, point_set_1)
            displacement_field_file_names = (
                os.path.join(self.name, dirlab_image_information[id]['sub_directory'], '50_to_00.nii.gz'),
                os.path.join(self.name, dirlab_image_information[id]['sub_directory'], '00_to_50.nii.gz')
            )

            if mask_directory is not None and os.path.exists(mask_directory):
                mask_file_names = (
                    os.path.join(mask_directory, dirlab_image_information[id]['sub_directory'],
                                 'Images', os.path.basename(mhd_0_file_name)),
                    os.path.join(mask_directory, dirlab_image_information[id]['sub_directory'],
                                 'Images', os.path.basename(mhd_1_file_name))
                )
            else:
                # If no mask was provided, just generate mask filled with ones
                mask_file_names = [
                    create_mask_by_size(image_file_names[i],
                                        os.path.join(output_directory, 'tmp', 'masks',
                                                     displacement_field_file_names[i]))
                    for i in range(2)
                ]

            file_names.append({
                'image_file_names': image_file_names,
                'mask_file_names': mask_file_names,
                'ground_truth_file_names': point_set_file_names,
                'displacement_field_file_names': displacement_field_file_names
            })

        self.file_names = take(sort_file_names(file_names),
                               max_number_of_registrations // 2)

    def evaluate(self, superelastix, file_names, output_directory):
        return self.evaluate_point_set(superelastix, file_names, output_directory)


class EMPIRE(Dataset):
    def __init__(self, input_directory, max_number_of_registrations):
        self.name = 'EMPIRE'
        self.category = 'Lung'

        file_names = []

        for i in range(1, 31):
            image_file_names = (
                os.path.join(input_directory, 'scans', "%02d" % i + '_Fixed.mhd'),
                os.path.join(input_directory, 'scans', "%02d" % i + '_Moving.mhd')
            )

            mask_file_names = (
                os.path.join(input_directory, 'lungMasks', "%02d" % i + '_Fixed.mhd'),
                os.path.join(input_directory, 'lungMasks', "%02d" % i + '_Moving.mhd')
            )

            # TODO: Find out output format
            displacement_field_file_names = (
                os.path.join(self.name, "%02d" % i + '_Moving_to_Fixed.nii.gz'),
                os.path.join(self.name, "%02d" % i + '_Fixed_to_Moving.nii.gz')
            )

            file_names.append({
                'image_file_names': image_file_names,
                'mask_file_names': mask_file_names,
                'displacement_field_file_names': displacement_field_file_names
            })

        self.file_names = take(sort_file_names(file_names),
                               max_number_of_registrations // 2)

    def evaluate(self, superelastix, file_names, output_directory):
        # TODO: Submit to EMPIRE
        pass


class ISBR18(Dataset):
    def __init__(self, input_directory, output_directory, max_number_of_registrations):
        self.name = 'ISBR18'
        self.category = 'Brain'

        self.input_directory = input_directory
        file_names = []

        image_names = [os.path.join(input_directory, 'Heads', image)
                       for image in os.listdir(os.path.join(input_directory, 'Heads'))
                       if image.endswith('.hdr')]
        image_names = [pair for pair in combinations(image_names, 2)]

        label_names = [os.path.join(input_directory, 'Atlases', atlas)
                       for atlas in os.listdir(os.path.join(input_directory, 'Atlases'))
                       if atlas.endswith('.hdr') and not "copied-information" in atlas]
        label_names = [pair for pair in combinations(label_names, 2)]

        displacement_field_names = create_list_displacement_field_names(image_names, self.name)

        # These label images do not have any world coordinate information
        label_names = [copy_information_from_images_to_labels(image, label,
                                                              disp_field,
                                                              output_directory,
                                                              'MET_USHORT')
                       for image, label, disp_field
                       in zip(image_names, label_names, displacement_field_names)]

        mask_names = [create_mask_by_thresholding(label, disp_field, output_directory, 0., 32, 16)
                      for label, disp_field in zip(label_names, displacement_field_names)]

        for image_name, mask_name, label_name, disp_field_name \
                in zip(image_names, mask_names, label_names, displacement_field_names):
            file_names.append({
                'image_file_names': image_name,
                'mask_file_names': mask_name,
                'ground_truth_file_names': label_name,
                'displacement_field_file_names': disp_field_name
            })

        self.file_names = take(sort_file_names(file_names),
                               max_number_of_registrations // 2)

    # TODO: Find out why inverse consistency does not work with this dataset
    def evaluate(self, superelastix, file_names, output_directory):

        displacement_field_paths = (
            os.path.join(output_directory, file_names['displacement_field_file_names'][0]),
            os.path.join(output_directory, file_names['displacement_field_file_names'][1]),
        )

        dice_0, dice_1 = dice(superelastix,
                              file_names['ground_truth_file_names'],
                              displacement_field_paths)

        return {
            file_names['displacement_field_file_names'][0]: dice_0,
            file_names['displacement_field_file_names'][1]: dice_1
        }


class LPBA40(Dataset):
    def __init__(self, input_directory, output_directory, max_number_of_registrations):
        self.name = 'LPBA40'
        self.category = 'Brain'

        self.input_directory = input_directory
        file_names = []

        image_names = [
            glob.glob(os.path.join(self.input_directory, 'delineation_space',
                                   sub_directory, '*.delineation.skullstripped.hdr'))[0]
            for sub_directory in os.listdir(os.path.join(self.input_directory,
                                                         'delineation_space'))
            if os.path.isdir(os.path.join(self.input_directory, 'delineation_space',
                                          sub_directory))
        ]

        image_names = [pair for pair in combinations(image_names, 2)]

        label_names = [
            glob.glob(os.path.join(self.input_directory, 'delineation_space',
                                   sub_directory, '*.delineation.structure.label.hdr'))[0]
            for sub_directory in os.listdir(os.path.join(self.input_directory,
                                                         'delineation_space'))
            if os.path.isdir(os.path.join(self.input_directory, 'delineation_space',
                                          sub_directory))
        ]

        label_names = [pair for pair in combinations(label_names, 2)]

        displacement_field_names = create_list_displacement_field_names(image_names, self.name)

        mask_file_names = [create_mask_by_thresholding(label, disp_field, output_directory, 0., 2, 2)
                           for label, disp_field in zip(label_names, displacement_field_names)]

        for image_name, mask_name, label_name, disp_field_name \
                in zip(image_names, mask_file_names, label_names, displacement_field_names):
            file_names.append({
                'image_file_names': image_name,
                'mask_file_names': mask_name,
                'ground_truth_file_names': label_name,
                'displacement_field_file_names': disp_field_name
            })

        self.file_names = take(sort_file_names(file_names),
                               max_number_of_registrations // 2)

    def evaluate(self, superelastix, file_names, output_directory):
        return self.evaluate_label_image(superelastix, file_names, output_directory)


class MGH10(Dataset):
    def __init__(self, input_directory, output_directory, max_number_of_registrations):
        self.name = 'MGH10'
        self.category = 'Brain'

        self.input_directory = input_directory
        file_names = []

        image_names = [os.path.join(input_directory, 'Heads', image)
                       for image in os.listdir(os.path.join(input_directory, 'Heads'))
                       if image.endswith('.hdr')]
        image_names = [pair for pair in combinations(image_names, 2)]

        label_names = [os.path.join(input_directory, 'Atlases', atlas)
                       for atlas in os.listdir(os.path.join(input_directory, 'Atlases'))
                       if atlas.endswith('.hdr')]
        label_names = [pair for pair in combinations(label_names, 2)]

        displacement_field_names = create_list_displacement_field_names(image_names, self.name)

        # Label images do not have any world coordinate information
        label_names = [copy_information_from_images_to_labels(image_pair,
                                                              label_pair,
                                                              disp_field_pair,
                                                              output_directory,
                                                              'MET_USHORT')
                       for image_pair, label_pair, disp_field_pair
                       in zip(image_names, label_names, displacement_field_names)]

        mask_file_names = [create_mask_by_thresholding(label, disp_field, output_directory, 0., 32, 16)
                           for label, disp_field in zip(label_names, displacement_field_names)]

        for image_name, mask_name, label_name, displ_field_name \
                in zip(image_names, mask_file_names, label_names, displacement_field_names):
            file_names.append({
                'image_file_names': image_name,
                'mask_file_names': mask_name,
                'ground_truth_file_names': label_name,
                'displacement_field_file_names': displ_field_name
            })

        self.file_names = take(sort_file_names(file_names),
                               max_number_of_registrations // 2)

    def evaluate(self, superelastix, file_names, output_directory):
        return self.evaluate_label_image(superelastix, file_names, output_directory)


class POPI(Dataset):

    def __init__(self, input_directory, mask_directory, output_directory, max_number_of_registrations):
        self.name = 'POPI'
        self.category = 'Lung'

        self.input_directory = input_directory
        file_names = []

        sub_directories = [directory for directory in os.listdir(self.input_directory)
                           if os.path.isdir(os.path.join(input_directory, directory))]

        for sub_directory in sub_directories:
            image_file_names = (os.path.join(input_directory, sub_directory, 'mhd', '00.mhd'),
                                os.path.join(input_directory, sub_directory, 'mhd', '50.mhd'))
            point_set_file_names = (os.path.join(input_directory, sub_directory, 'pts', '00.pts'),
                                    os.path.join(input_directory, sub_directory, 'pts', '50.pts'))
            displacement_field_file_names = (os.path.join(self.name, sub_directory, '50_to_00.nii.gz'),
                                             os.path.join(self.name, sub_directory, '00_to_50.nii.gz'))

            if mask_directory is not None and os.path.exists(mask_directory):
                mask_file_names = (os.path.join(mask_directory, sub_directory, 'mhd', '00.mhd'),
                                   os.path.join(mask_directory, sub_directory, 'mhd', '50.mhd'))
            else:
                # If no mask was provided, just generate mask filled with ones
                mask_file_names = [
                    create_mask_by_size(image_file_names[i],
                                        os.path.join(output_directory, 'tmp', 'masks',
                                                     displacement_field_file_names[i]))
                    for i in range(2)
                ]

            file_names.append({
                "image_file_names": image_file_names,
                "mask_file_names": mask_file_names,
                "ground_truth_file_names": point_set_file_names,
                "displacement_field_file_names": displacement_field_file_names
            })

        self.file_names = take(sort_file_names(file_names),
                               max_number_of_registrations // 2)

    def evaluate(self, superelastix, file_names, output_directory):
        return self.evaluate_point_set(superelastix, file_names, output_directory)


class SPREAD(Dataset):

    def __init__(self, input_directory, output_directory, max_number_of_registrations):
        self.name = 'SPREAD'
        self.category = 'Lung'

        self.input_directory = input_directory
        file_names = []

        sub_directories = [directory
                           for directory in os.listdir(os.path.join(self.input_directory, 'mhd'))
                           if os.path.isdir(os.path.join(self.input_directory, 'mhd', directory))]

        for sub_directory in sub_directories:
            image_file_names = (os.path.join(input_directory, 'mhd', sub_directory, 'baseline_1.mha'),
                                os.path.join(input_directory, 'mhd', sub_directory, 'followup_1.mha'))

            os.makedirs(os.path.join(output_directory, self.name), exist_ok=True)

            name = sub_directory + '_baseline_1_Cropped_point'
            baseline_point_set_file_name_we = os.path.join(input_directory, 'groundtruth',
                                                           'distinctivePoints', name)
            point_set = np.loadtxt(baseline_point_set_file_name_we + '.txt', skiprows=2)
            baseline_point_set_file_name_we_without_header = \
                os.path.join(output_directory, 'tmp', self.name, sub_directory + '_baseline_1_Cropped_point.txt')
            np.savetxt(baseline_point_set_file_name_we_without_header, point_set)

            follow_up_point_set_file_name_we = os.path.join(input_directory, 'groundtruth', 'annotate', 'Consensus',
                                                            sub_directory + '_b1f1_point')
            point_set = np.loadtxt(follow_up_point_set_file_name_we + '.txt', skiprows=2)
            follow_up_point_set_file_name_we_without_header = os.path.join(output_directory, 'tmp',
                                                                           self.name, name + '.txt')
            np.savetxt(follow_up_point_set_file_name_we_without_header, point_set)


            point_set_file_names = (baseline_point_set_file_name_we_without_header,
                                    follow_up_point_set_file_name_we_without_header)

            displacement_field_file_names = (os.path.join(self.name, sub_directory,
                                                          'followup_to_baseline.nii.gz'),
                                             os.path.join(self.name, sub_directory,
                                                          'baseline_to_followup.nii.gz'))

            file_names.append({
                "image_file_names": image_file_names,
                "ground_truth_file_names": point_set_file_names,
                "displacement_field_file_names": displacement_field_file_names
            })

        self.file_names = take(sort_file_names(file_names),
                               max_number_of_registrations // 2)

    def evaluate(self, superelastix, file_names, output_directory):
        self.evaluate_point_set(superelastix, file_names, output_directory)


class HBIA(Dataset):
    """
    a wrapper for dataset with stain Histology provided by CMP-BIA sections
    http://cmp.felk.cvut.cz/~borovji3/?page=dataset

    The dataset contains several sets (1st level) of stained tissues in given
    scales (2nd level) where the name is `scale_<NUMBER>sc`.
    The next level (3rd level) belong to the images and landmarks.
    The images share the same name with landmark files and the point are set
    across all slices of the particular tissue so the registration pairs are
    generated one against all (we can skip symmetric pairs like A->B and B->A)
    """

    PATTERN_FOLDER_SCALE = 'scale-%ipc'

    def __init__(self, input_directory, output_directory,
                 max_number_of_registrations, scale=None):
        self.name = 'HistoBIA'
        self.category = 'cancer'

        self.input_directory = input_directory
        # set default scale 10%
        scale = 10 if scale is None else scale

        sub_folders = [d for d in os.listdir(self.input_directory)
                       if os.path.isdir(os.path.join(input_directory, d))]

        assert len(sub_folders) > 0, 'missing downloaded dataset files'
        # read all available scales from the first set
        scales = [int(re.findall('\\d+', d)[0])
                  for d in os.listdir(os.path.join(self.input_directory, sub_folders[0]))
                  if os.path.isdir(os.path.join(input_directory, sub_folders[0], d))]
        assert scale in scales, 'not supported dataset scale (%i) from %s' \
                                % (scale, repr(scales))

        file_names = []
        for dir_set in sorted(sub_folders):
            # create the path to the image/points destination
            path_images = os.path.join(self.input_directory, dir_set,
                                       self.PATTERN_FOLDER_SCALE % scale)
            # listing all images in the set with given scale
            list_image_names = sorted([n for n in os.listdir(path_images)
                                       if os.path.splitext(n)[-1] in ['.png', '.jpg']])

            # creating pairs one-to-all with and skipp duplicit symetric pairs
            for i, image_name_0 in enumerate(list_image_names):
                pints_name_0 = os.path.splitext(image_name_0)[0] + '.csv'
                for image_name_1 in list_image_names[i+1:]:
                    pints_name_1 = os.path.splitext(image_name_1)[0] + '.csv'

                    # compose the registration image pair
                    image_names = (os.path.join(path_images, image_name_0),
                                   os.path.join(path_images, image_name_1))
                    # and pair of landmarks related to the image pair
                    point_set_names = (os.path.join(path_images, pints_name_0),
                                       os.path.join(path_images, pints_name_1))

                    # If no mask was provided, generate mask filled with ones
                    mask_names = [
                        create_mask_by_size(image_names[i],
                                            os.path.join(output_directory,
                                                         'tmp', 'masks',
                                                         image_names[i]))
                        for i in range(2)
                    ]

                    file_names.append({
                        "image_file_names": image_names,
                        "mask_file_names": mask_names,
                        "ground_truth_file_names": point_set_names,
                    })

        self.file_names = take(sort_file_names(file_names),
                               max_number_of_registrations // 2)

    def evaluate(self, superelastix, file_names, output_directory):
        return self.evaluate_point_set(superelastix, file_names, output_directory)
