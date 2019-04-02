import glob, csv, re, stat
from abc import ABCMeta
from itertools import combinations

from ContinuousRegistration.Source.metrics import tre, hausdorff, inverse_consistency, label_overlap
from ContinuousRegistration.Source.util import *

class Dataset:
    """
    Base class for datasets. The derived classes need only to implement disk access  and how to evaluate the dataset.
    Everything else is handled by this class. See metrics.py for available evaluation metrics.
    """
    __metaclass__ = ABCMeta

    def generator(self):
        for file_name in self.file_names:
            yield file_name

    @staticmethod
    def make_shell_scripts(superelastix, blueprint_file_name, file_names, output_directory, script_file_extension):
        if not os.path.exists(os.path.join(output_directory, 'sh')):
            os.mkdir(os.path.join(output_directory, 'sh'))
        COMMAND_TEMPLATE = '#!/bin/sh\n%s --conf "%s" --in %s %s --out DisplacementField="%s" --loglevel trace --logfile "%s"'

        # Fixed=image0, Moving=image1, Output: image1_to_image0
        root = splitext(file_names['disp_field_file_names'][0])[0]
        shell_script_file_name_0 = os.path.join(output_directory, 'sh',
                                              root.replace('/', '_').replace('\\', '_').replace('.', '_') + script_file_extension)

        if 'mask_file_names' not in file_names:
            file_names['mask_file_names'] = ('', '')
        with open(shell_script_file_name_0, 'w') as shell_script:
            shell_script.write(COMMAND_TEMPLATE % (superelastix, blueprint_file_name,
                'FixedImage="%s" MovingImage="%s" ' % tuple(file_names['image_file_names']),
                'FixedMask="%s" MovingMask="%s"' % tuple(file_names['mask_file_names']),
                os.path.join(output_directory, file_names['disp_field_file_names'][0]),
                os.path.splitext(shell_script_file_name_0)[0] + '.log'))

        os.chmod(shell_script_file_name_0,
                 os.stat(shell_script_file_name_0).st_mode | stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH)
        logging.info('Wrote %s' % shell_script_file_name_0)

        # Fixed=image1, Moving=image0, Output: image0_to_image1
        root = splitext(file_names['disp_field_file_names'][1])[0]
        shell_script_file_name_1 = os.path.join(output_directory, 'sh',
                                              root.replace('/', '_').replace('\\', '_').replace('.', '_') + script_file_extension)

        if 'mask_file_names' not in file_names:
            file_names['mask_file_names'] = ('', '')
        with open(shell_script_file_name_1, 'w') as shell_script:
            shell_script.write(COMMAND_TEMPLATE % (superelastix, blueprint_file_name,
                'FixedImage="%s" MovingImage="%s" ' % tuple(file_names['image_file_names'][::-1]),
                'FixedMask="%s" MovingMask="%s"' % tuple(file_names['mask_file_names'][::-1]),
                os.path.join(output_directory, file_names['disp_field_file_names'][1]),
                os.path.splitext(shell_script_file_name_1)[0] + '.log'))

        os.chmod(shell_script_file_name_1,
                 os.stat(shell_script_file_name_0).st_mode | stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH)
        logging.info('Wrote %s' % shell_script_file_name_1)

    @staticmethod
    def evaluate():
        """
        The derived class should either call evaluate_point_set,
        `evaluate_label_image` implement its own metrics, or combinations here
        """
        pass

    def evaluate_point_set(self, superelastix, file_names, output_directory):
        """ Default evaluation method for point set ground truths

        :param superelastix:
        :param file_names:
        :param output_directory:
        :return:
        """
        disp_field_paths = (
            os.path.join(output_directory, file_names['disp_field_file_names'][0]),
            os.path.join(output_directory, file_names['disp_field_file_names'][1]))

        point_sets = (self.read_point_set(file_names['ground_truth_file_names'][0]),
                      self.read_point_set(file_names['ground_truth_file_names'][1]))

        tre_0, tre_1 = tre(superelastix, point_sets, disp_field_paths)
        hausdorff_0, hausdorff_1 = hausdorff(superelastix, point_sets, disp_field_paths)
        inverse_consistency_0, inverse_consistency_1 = inverse_consistency(
            superelastix, disp_field_paths, file_names['mask_file_names'])

        return {
            file_names['disp_field_file_names'][0]:
                merge_dicts(tre_0, hausdorff_0, inverse_consistency_0),
            file_names['disp_field_file_names'][1]:
                merge_dicts(tre_1, hausdorff_1, inverse_consistency_1)
        }

    @staticmethod
    def evaluate_label(superelastix, file_names, output_directory):
        """ Default evaluation method for label ground truths

        :param superelastix:
        :param file_names:
        :param output_directory:
        :return:
        """
        disp_field_paths = (
            os.path.join(output_directory, file_names['disp_field_file_names'][0]),
            os.path.join(output_directory, file_names['disp_field_file_names'][1]),
        )

        inverse_consistency_0, inverse_consistency_1 = inverse_consistency(
            superelastix, disp_field_paths, file_names['mask_file_names'])

        dice_0, dice_1 = label_overlap(superelastix, file_names['ground_truth_file_names'], disp_field_paths)

        return {
            file_names['disp_field_file_names'][0]:
                merge_dicts(inverse_consistency_0, dice_0),
            file_names['disp_field_file_names'][1]:
                merge_dicts(inverse_consistency_1, dice_1)
        }

    @staticmethod
    def make_images(superelastix, file_names, output_directory):
        disp_field_0_file_name = os.path.join(output_directory, file_names['disp_field_file_names'][0])
        disp_field_1_file_name = os.path.join(output_directory, file_names['disp_field_file_names'][1])

        warp_image(superelastix, file_names['image_file_names'][1], disp_field_0_file_name, 'image')
        warp_image(superelastix, file_names['image_file_names'][0], disp_field_1_file_name, 'image')

    @staticmethod
    def make_labels(superelastix, file_names, output_directory):
        disp_field_0_file_name = os.path.join(output_directory, file_names['disp_field_file_names'][0])
        disp_field_1_file_name = os.path.join(output_directory, file_names['disp_field_file_names'][1])

        warp_image(superelastix, file_names['ground_truth_file_names'][1], disp_field_0_file_name, 'label')
        warp_image(superelastix, file_names['ground_truth_file_names'][0], disp_field_1_file_name, 'label')

    @staticmethod
    def make_difference_images(superelastix, file_names, output_directory):
        disp_field_0_file_name = os.path.join(output_directory, file_names['disp_field_file_names'][0])
        disp_field_1_file_name = os.path.join(output_directory, file_names['disp_field_file_names'][1])

        image_1_to_0 = sitk.ReadImage(warp_image(superelastix, file_names['image_file_names'][1], disp_field_0_file_name, 'result_image'))
        image_0_to_1 = sitk.ReadImage(warp_image(superelastix, file_names['image_file_names'][0], disp_field_1_file_name, 'result_image'))

        difference_image_1_to_0_base_name, difference_image_1_to_0_file_name_ext = splitext(disp_field_0_file_name)
        difference_image_1_to_0_file_name = difference_image_1_to_0_base_name + '_difference_image' + difference_image_1_to_0_file_name_ext
        difference_image_0_to_1_base_name, difference_image_0_to_1_file_name_ext = splitext(disp_field_0_file_name)
        difference_image_0_to_1_file_name = difference_image_0_to_1_base_name + '_difference_image' + difference_image_0_to_1_file_name_ext

        image_0 = sitk.ReadImage(file_names['image_file_names'][0])
        image_1 = sitk.ReadImage(file_names['image_file_names'][1])

        difference_image_1_to_0 = sitk.Cast(image_0, sitk.sitkFloat32) - sitk.Cast(image_1_to_0, sitk.sitkFloat32)
        difference_image_0_to_1 = sitk.Cast(image_1, sitk.sitkFloat32) - sitk.Cast(image_0_to_1, sitk.sitkFloat32)

        sitk.WriteImage(difference_image_1_to_0, difference_image_1_to_0_file_name)
        sitk.WriteImage(difference_image_0_to_1, difference_image_0_to_1_file_name)

    @staticmethod
    def make_checkerboards(superelastix, file_names, output_directory):
        disp_field_0_file_name = os.path.join(output_directory, file_names['disp_field_file_names'][0])
        disp_field_1_file_name = os.path.join(output_directory, file_names['disp_field_file_names'][1])

        disp_field_0_path, disp_field_0_ext = splitext(disp_field_0_file_name)
        disp_field_1_path, disp_field_1_ext = splitext(disp_field_1_file_name)

        checkerboard_0_file_name = disp_field_0_path + '_checkerboard' + disp_field_0_ext
        checkerboard_1_file_name = disp_field_1_path + '_checkerboard' + disp_field_1_ext

        image_0 = sitk.RescaleIntensity(sitk.ReadImage(file_names['image_file_names'][0]))
        image_1 = sitk.RescaleIntensity(sitk.ReadImage(file_names['image_file_names'][1]))

        # TODO: Better way of creating checkerboard pattern
        big_number = 1e9
        sitk.WriteImage(sitk.CheckerBoard(image_0 < -big_number, image_0 > -big_number, (5,)*image_0.GetDimension()), checkerboard_0_file_name)
        sitk.WriteImage(sitk.CheckerBoard(image_1 < -big_number, image_1 > -big_number, (5,)*image_0.GetDimension()), checkerboard_1_file_name)

        warp_image(superelastix, checkerboard_1_file_name, disp_field_0_file_name, 'checkerboard')
        warp_image(superelastix, checkerboard_0_file_name, disp_field_1_file_name, 'checkerboard')

    @staticmethod
    def make_image_checkerboards(superelastix, file_names, output_directory):
        disp_field_0_file_name = os.path.join(output_directory, file_names['disp_field_file_names'][0])
        disp_field_1_file_name = os.path.join(output_directory, file_names['disp_field_file_names'][1])

        disp_field_0_path, disp_field_0_ext = splitext(disp_field_0_file_name)
        disp_field_1_path, disp_field_1_ext = splitext(disp_field_1_file_name)

        image_checkerboard_0_file_name = disp_field_0_path + '_image_checkerboard' + disp_field_0_ext
        image_checkerboard_1_file_name = disp_field_1_path + '_image_checkerboard' + disp_field_1_ext

        warped_image_0_to_1_file_name = warp_image(superelastix, file_names['image_file_names'][0],
                                                   disp_field_1_file_name, 'result_image')
        warped_image_1_to_0_file_name = warp_image(superelastix, file_names['image_file_names'][1],
                                                   disp_field_0_file_name, 'result_image')

        image_0 = sitk.RescaleIntensity(sitk.ReadImage(file_names['image_file_names'][0]))
        image_1 = sitk.RescaleIntensity(sitk.ReadImage(file_names['image_file_names'][1]))

        sitk.WriteImage(sitk.CheckerBoard(image_0, sitk.RescaleIntensity(sitk.ReadImage(warped_image_1_to_0_file_name, image_1.GetPixelID())),
                                          (5,)*image_0.GetDimension()), image_checkerboard_0_file_name)
        sitk.WriteImage(sitk.CheckerBoard(image_1, sitk.RescaleIntensity(sitk.ReadImage(warped_image_0_to_1_file_name, image_1.GetPixelID())),
                                          (5,)*image_1.GetDimension()), image_checkerboard_1_file_name)

    @staticmethod
    def make_label_checkerboards(superelastix, file_names, output_directory):
        disp_field_0_file_name = os.path.join(output_directory, file_names['disp_field_file_names'][0])
        disp_field_1_file_name = os.path.join(output_directory, file_names['disp_field_file_names'][1])

        disp_field_0_path, disp_field_0_ext = splitext(disp_field_0_file_name)
        disp_field_1_path, disp_field_1_ext = splitext(disp_field_1_file_name)

        image_checkerboard_0_file_name = disp_field_0_path + '_label_checkerboard' + disp_field_0_ext
        image_checkerboard_1_file_name = disp_field_1_path + '_label_checkerboard' + disp_field_1_ext

        warped_image_0_file_name = warp_image(superelastix, file_names['ground_truth_file_names'][0],
                                              disp_field_1_file_name, 'label_checkerboard')
        warped_image_1_file_name = warp_image(superelastix, file_names['ground_truth_file_names'][1],
                                              disp_field_0_file_name, 'label_checkerboard')

        label_0 = sitk.ReadImage(file_names['ground_truth_file_names'][0])
        label_1 = sitk.ReadImage(file_names['ground_truth_file_names'][1])

        sitk.WriteImage(sitk.CheckerBoard(label_0, sitk.ReadImage(warped_image_1_file_name, label_0.GetPixelID()),
                                          (5,) * label_0.GetDimension()),
                        image_checkerboard_0_file_name)
        sitk.WriteImage(sitk.CheckerBoard(label_1, sitk.ReadImage(warped_image_0_file_name, label_1.GetPixelID()),
                                          (5,) * label_1.GetDimension()),
                        image_checkerboard_1_file_name)

    @staticmethod
    def read_point_set(file_name):
        return read_pts(file_name)



class BRAIN2D(Dataset):
    """
    Convenience data set for quickly testing registrations of brains in 2d.
    Useful when tuning very slow methods like ANTs. Simply wraps some
    of our testing data and exposes it as a dataset.
    """
    def __init__(self, input_directory, output_directory):
        self.name = 'BRAIN2D'
        self.category = 'Brain'

        self.file_names = [{
            'image_file_names': (
                os.path.join(input_directory, 'r16slice.nii.gz'),
                os.path.join(input_directory, 'r64slice.nii.gz')
            ),
            'ground_truth_file_names': (
                os.path.join(input_directory, 'r16slice_mask.nii.gz'),
                os.path.join(input_directory, 'r64slice_mask.nii.gz'),
            ),
            'mask_file_names': (
                os.path.join(input_directory, 'r16slice_mask.nii.gz'),
                os.path.join(input_directory, 'r64slice_mask.nii.gz'),
            )
        }]

        self.file_names[0]['disp_field_file_names'] = create_disp_field_names(self.file_names[0]['image_file_names'], self.name)

    def evaluate(self, superelastix, file_names, output_directory):
        return self.evaluate_label(superelastix, file_names, output_directory)


class LUNG2D(Dataset):
    """
    Convenience data set for quickly testing registrations of lungs in 2d.
    Useful when tuning very slow methods like ANTs. Simply wraps some
    of our testing data and exposes it as a test dataset.
    """
    def __init__(self, input_directory, output_directory):
        self.name = 'LUNG2D'
        self.category = 'Lung'

        self.file_names = [{
            'image_file_names': (
                os.path.join(input_directory, 'Lung2dFixedImage.nii.gz'),
                os.path.join(input_directory, 'Lung2dMovingImage.nii.gz')
            ),
            'ground_truth_file_names': (
                os.path.join(input_directory, 'Lung2dFixedMask.nii.gz'),
                os.path.join(input_directory, 'Lung2dMovingMask.nii.gz'),
            ),
            'mask_file_names': (
                os.path.join(input_directory, 'Lung2dFixedMask.nii.gz'),
                os.path.join(input_directory, 'Lung2dMovingMask.nii.gz'),
            )
        }]

        self.file_names[0]['disp_field_file_names'] = create_disp_field_names(self.file_names[0]['image_file_names'], self.name)

    def evaluate(self, superelastix, file_names, output_directory):
        return self.evaluate_label(superelastix, file_names, output_directory)


class CUMC12(Dataset):
    def __init__(self, input_directory, output_directory, max_number_of_registrations):
        self.name = 'CUMC12'
        self.category = 'Brain'

        self.input_directory = input_directory
        self.file_names = []

        image_file_names = sorted([os.path.join(input_directory, 'Heads', image)
                       for image in os.listdir(os.path.join(input_directory, 'Heads'))
                       if image.endswith('.hdr')])
        image_file_names = [pair for pair in combinations(image_file_names, 2)]
        image_file_names = take(image_file_names, max_number_of_registrations // 2)

        label_file_names = sorted([os.path.join(input_directory, 'Atlases', atlas)
                       for atlas in os.listdir(os.path.join(input_directory, 'Atlases'))
                       if atlas.endswith('.hdr')])
        label_file_names = [pair for pair in combinations(label_file_names, 2)]
        label_file_names = take(label_file_names, max_number_of_registrations // 2)

        disp_field_file_names = [create_disp_field_names(image_file_name_pair, self.name)
                                 for image_file_name_pair in image_file_names]

        mask_file_names = [create_mask_by_thresholding(label_file_name, disp_field_file_name, output_directory, 0., 32, 16)
                      for label_file_name, disp_field_file_name in zip(label_file_names, disp_field_file_names)]

        for image_file_name, mask_file_name, label_file_name, disp_field_file_name \
                in zip(image_file_names, mask_file_names, label_file_names, disp_field_file_names):
            self.file_names.append({
                'image_file_names': image_file_name,
                'mask_file_names': mask_file_name,
                'ground_truth_file_names': label_file_name,
                'disp_field_file_names': disp_field_file_name
            })

    def evaluate(self, superelastix, file_names, output_directory):
        return self.evaluate_label(superelastix, file_names, output_directory)


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

        os.makedirs(os.path.join(output_directory, 'tmp', 'point_sets', self.name), exist_ok=True)

        # Now we have all the information necessary and can generate the header files
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

            index_set_0_file_name = glob.glob(os.path.join(input_directory,
                                                 dirlab_image_information[id]['sub_directory'],
                                                 'ExtremePhases', '*T00_xyz.txt'))[0]
            index_set_1_file_name = glob.glob(os.path.join(input_directory,
                                                 dirlab_image_information[id]['sub_directory'],
                                                 'ExtremePhases', '*T50_xyz.txt'))[0]

            point_set_0 = index2point(sitk.ReadImage(mhd_0_file_name), self.read_point_set(index_set_0_file_name))
            point_set_1 = index2point(sitk.ReadImage(mhd_1_file_name), self.read_point_set(index_set_1_file_name))
            point_set_0_file_name = os.path.join(output_directory, 'tmp', 'point_sets', self.name, os.path.basename(index_set_0_file_name))
            point_set_1_file_name = os.path.join(output_directory, 'tmp', 'point_sets', self.name, os.path.basename(index_set_1_file_name))
            write_pts(point_set_0, point_set_0_file_name)
            write_pts(point_set_1, point_set_1_file_name)

            image_file_names = (mhd_0_file_name, mhd_1_file_name)
            point_set_file_names = (point_set_0_file_name, point_set_1_file_name)
            disp_field_file_names = (
                os.path.join(self.name, dirlab_image_information[id]['sub_directory'], 'moving50_to_fixed00.mha'),
                os.path.join(self.name, dirlab_image_information[id]['sub_directory'], 'moving00_to_fixed50.mha')
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
                logging.warning('Masks not found for DIRLAB. Creating mask with elements set to one.')
                mask_file_names = [
                    create_mask_by_size(image_file_names[i],
                                        os.path.join(output_directory, 'tmp', 'masks',
                                                     disp_field_file_names[i]))
                    for i in range(2)
                ]

            file_names.append({
                'image_file_names': image_file_names,
                'mask_file_names': mask_file_names,
                'ground_truth_file_names': point_set_file_names,
                'disp_field_file_names': disp_field_file_names
            })

        self.file_names = take(sort_file_names(file_names),
                               max_number_of_registrations // 2)

    def evaluate(self, superelastix, file_names, output_directory):
        return self.evaluate_point_set(superelastix, file_names, output_directory)

    @staticmethod
    def read_point_set(file_name):
        return read_pts(file_name)


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
            disp_field_file_names = (
                os.path.join(self.name, "%02d" % i + '_moving1_to_fixed0.nii.gz'),
                os.path.join(self.name, "%02d" % i + '_moving0_to_fixed1.nii.gz')
            )

            file_names.append({
                'image_file_names': image_file_names,
                'mask_file_names': mask_file_names,
                'disp_field_file_names': disp_field_file_names
            })

        self.file_names = take(sort_file_names(file_names),
                               max_number_of_registrations // 2)

    def evaluate(self, superelastix, file_names, output_directory):
        # TODO: Submit to EMPIRE
        pass

    @staticmethod
    def read_point_set(file_name):
        return read_pts(file_name)

# Download from http://brain-development.org/brain-atlases/individual-adult-brain-atlases-new/.
# Fill in info and click on "Adult individual atlases (30 MR images and manual segmentations;
# A Hammers, R Allom et al. 2003, IS Gousias et al. 2008, I. Faillenot et al. 2017)". It should
# point to this link:
# http://biomedic.doc.ic.ac.uk/brain-development/downloads/hammers/Hammers_mith-n30r95.tar.gz
# Do not download directly. Fill in information.
class HAMMERS(Dataset):
    def __init__(self, input_directory, output_directory, max_number_of_registrations):
        self.name = 'HAMMERS'
        self.category = 'Brain'

        self.input_directory = input_directory
        self.file_names = []

        image_file_names = sorted([os.path.join(input_directory, image)
                       for image in os.listdir(input_directory)
                       if image.endswith('.nii.gz') and not 'seg' in image])
        image_file_names = [pair for pair in combinations(image_file_names, 2)]
        image_file_names = take(image_file_names, max_number_of_registrations // 2)

        label_file_names = sorted([os.path.join(input_directory, image)
                       for image in os.listdir(input_directory)
                       if image.endswith('seg.nii.gz')])
        label_file_names = [pair for pair in combinations(label_file_names, 2)]
        label_file_names = take(label_file_names, max_number_of_registrations // 2)

        disp_field_file_names = [create_disp_field_names(image_file_name_pair, self.name)
                                 for image_file_name_pair in image_file_names]

        mask_file_names = [create_mask_by_thresholding(label_file_name, disp_field_file_name, output_directory, 0., 32, 16)
                      for label_file_name, disp_field_file_name in zip(label_file_names, disp_field_file_names)]

        for image_file_name, mask_file_name, label_file_name, disp_field_file_name \
                in zip(image_file_names, mask_file_names, label_file_names, disp_field_file_names):
            self.file_names.append({
                'image_file_names': image_file_name,
                'mask_file_names': mask_file_name,
                'ground_truth_file_names': label_file_name,
                'disp_field_file_names': disp_field_file_name
            })


    def evaluate(self, superelastix, file_names, output_directory):
        return self.evaluate_label(superelastix, file_names, output_directory)


class IBSR18(Dataset):
    def __init__(self, input_directory, output_directory, max_number_of_registrations):
        self.name = 'IBSR18'
        self.category = 'Brain'

        self.input_directory = input_directory
        self.file_names = []

        image_file_names = sorted([os.path.join(input_directory, subdirectory, image)
                                   for subdirectory in os.listdir(input_directory) if os.path.isdir(os.path.join(input_directory, subdirectory))
                                   for image in os.listdir(os.path.join(input_directory, subdirectory))
                                   if image.endswith('_ana_strip.nii.gz')])
        image_file_names = [pair for pair in combinations(image_file_names, 2)]
        image_file_names = take(image_file_names, max_number_of_registrations // 2)

        label_file_names = sorted([os.path.join(input_directory, subdirectory, image)
                                   for subdirectory in os.listdir(input_directory) if os.path.isdir(os.path.join(input_directory, subdirectory))
                                   for image in os.listdir(os.path.join(input_directory, subdirectory))
                                   if image.endswith('_seg_ana.nii.gz')])
        label_file_names = [pair for pair in combinations(label_file_names, 2)]
        label_file_names = take(label_file_names, max_number_of_registrations // 2)

        mask_file_names = sorted([os.path.join(input_directory, subdirectory, image)
                                   for subdirectory in os.listdir(input_directory) if os.path.isdir(os.path.join(input_directory, subdirectory))
                                   for image in os.listdir(os.path.join(input_directory, subdirectory))
                                   if image.endswith('_ana_brainmask.nii.gz')])
        mask_file_names = [pair for pair in combinations(mask_file_names, 2)]
        mask_file_names = take(mask_file_names, max_number_of_registrations // 2)

        disp_field_file_names = [create_disp_field_names(image_file_name_pair, self.name)
                                 for image_file_name_pair in image_file_names]

        for image_file_name, mask_file_name, label_file_name, disp_field_file_name \
                in zip(image_file_names, mask_file_names, label_file_names, disp_field_file_names):
            self.file_names.append({
                'image_file_names': image_file_name,
                'mask_file_names': mask_file_name,
                'ground_truth_file_names': label_file_name,
                'disp_field_file_names': disp_field_file_name
            })

    def evaluate(self, superelastix, file_names, output_directory):
        return self.evaluate_label(superelastix, file_names, output_directory)



class LPBA40(Dataset):
    def __init__(self, input_directory, output_directory, max_number_of_registrations):
        self.name = 'LPBA40'
        self.category = 'Brain'

        self.input_directory = input_directory
        self.file_names = []

        image_file_names = []
        for sub_directory in os.listdir(os.path.join(input_directory, 'delineation_space')):
            if os.path.isdir(os.path.join(input_directory, 'delineation_space', sub_directory)):
                for image_file_name in os.listdir(os.path.join(input_directory, 'delineation_space', sub_directory)):
                    if image_file_name.endswith('delineation.skullstripped.hdr'):
                        image_file_names.append(os.path.join('delineation_space', sub_directory, image_file_name))

        label_file_names = []
        for sub_directory in os.listdir(os.path.join(input_directory, 'delineation_space')):
            if os.path.isdir(os.path.join(input_directory, 'delineation_space', sub_directory)):
                for label_file_name in glob.glob(os.path.join(input_directory, 'delineation_space', sub_directory, 'S*delineation.structure.label.hdr')):

                    input_label_file_name = os.path.join(input_directory, 'delineation_space', sub_directory, label_file_name)
                    output_label_file_name = os.path.join(output_directory, 'tmp', 'removed_labels', os.path.basename(label_file_name))

                    # Remove labels outside skullstripped image
                    if not os.path.exists(output_label_file_name):
                        os.makedirs(os.path.dirname(output_label_file_name), exist_ok=True)
                        label = sitk.ChangeLabel(sitk.ReadImage(input_label_file_name), {181: 0, 182: 0})
                        sitk.WriteImage(label, output_label_file_name)

                    label_file_names.append(output_label_file_name)

        image_file_names = [pair for pair in combinations(sorted(image_file_names), 2)]
        label_file_names = [pair for pair in combinations(sorted(label_file_names), 2)]

        image_file_names = take(image_file_names, max_number_of_registrations // 2)
        label_file_names = take(label_file_names, max_number_of_registrations // 2)

        image_file_names = [create_identity_world_information(pair, self.name, input_directory, output_directory)
                            for pair in image_file_names]

        disp_field_file_names = [create_disp_field_names(image_file_name_pair, self.name)
                                 for image_file_name_pair in image_file_names]

        mask_file_names = [create_mask_by_thresholding(label, disp_field, output_directory, 0., 32, 30)
                           for label, disp_field in zip(label_file_names, disp_field_file_names)]

        for image_file_name, mask_file_name, label_file_name, disp_field_file_name \
                in zip(image_file_names, mask_file_names, label_file_names, disp_field_file_names):
            self.file_names.append({
                'image_file_names': image_file_name,
                'mask_file_names': mask_file_name,
                'ground_truth_file_names': label_file_name,
                'disp_field_file_names': disp_field_file_name
            })

    def evaluate(self, superelastix, file_names, output_directory):
        return self.evaluate_label(superelastix, file_names, output_directory)


class MGH10(Dataset):
    def __init__(self, input_directory, output_directory, max_number_of_registrations):
        self.name = 'MGH10'
        self.category = 'Brain'

        self.input_directory = input_directory
        self.file_names = []

        image_file_names = sorted([os.path.join(input_directory, 'Heads', image)
                       for image in os.listdir(os.path.join(input_directory, 'Heads'))
                       if image.endswith('.hdr')])
        image_file_names = [pair for pair in combinations(image_file_names, 2)]
        image_file_names = take(image_file_names, max_number_of_registrations // 2)

        label_file_names = sorted([os.path.join(input_directory, 'Atlases', atlas)
                       for atlas in os.listdir(os.path.join(input_directory, 'Atlases'))
                       if atlas.endswith('.hdr')])
        label_file_names = [pair for pair in combinations(label_file_names, 2)]
        label_file_names = take(label_file_names, max_number_of_registrations // 2)

        disp_field_file_names = [create_disp_field_names(image_file_name_pair, self.name)
                                 for image_file_name_pair in image_file_names]

        # Label images do not have any world coordinate information
        label_file_names = [copy_information_from_images_to_labels(image_file_name_pair,
                                                                   label_file_name_pair,
                                                                   disp_field_file_name_pair,
                                                                   output_directory,
                                                                   'MET_USHORT')
                       for image_file_name_pair, label_file_name_pair, disp_field_file_name_pair
                       in zip(image_file_names, label_file_names, disp_field_file_names)]

        mask_file_names = [create_mask_by_thresholding(label_file_name, disp_field, output_directory, 0., 32, 16)
                           for label_file_name, disp_field in zip(label_file_names, disp_field_file_names)]

        for image_file_name, mask_file_name, label_file_name, disp_field_file_name \
                in zip(image_file_names, mask_file_names, label_file_names, disp_field_file_names):
            self.file_names.append({
                'image_file_names': image_file_name,
                'mask_file_names': mask_file_name,
                'ground_truth_file_names': label_file_name,
                'disp_field_file_names': disp_field_file_name
            })

    def evaluate(self, superelastix, file_names, output_directory):
        return self.evaluate_label(superelastix, file_names, output_directory)


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
            disp_field_file_names = (os.path.join(self.name, sub_directory, 'moving50_to_fixed00.nii.gz'),
                                     os.path.join(self.name, sub_directory, 'moving00_to_fixed50.nii.gz'))

            if mask_directory is not None and os.path.exists(mask_directory):
                mask_file_names = (os.path.join(mask_directory, sub_directory, 'mhd', '00.mhd'),
                                   os.path.join(mask_directory, sub_directory, 'mhd', '50.mhd'))
            else:
                # If no mask was provided, just generate mask filled with ones
                logging.warning('Masks not found for POPI. Creating mask with elements set to one.')
                mask_file_names = [
                    create_mask_by_size(image_file_names[i],
                                        os.path.join(output_directory, 'tmp', 'masks',
                                                     disp_field_file_names[i]))
                    for i in range(2)
                ]

            file_names.append({
                "image_file_names": image_file_names,
                "mask_file_names": mask_file_names,
                "ground_truth_file_names": point_set_file_names,
                "disp_field_file_names": disp_field_file_names
            })

        self.file_names = take(sort_file_names(file_names),
                               max_number_of_registrations // 2)

    def evaluate(self, superelastix, file_names, output_directory):
        return self.evaluate_point_set(superelastix, file_names, output_directory)

    @staticmethod
    def read_point_set(file_name):
        return read_pts(file_name)


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

            mask_file_names = (os.path.join(input_directory, 'mhd', sub_directory, 'baseline_1_mask_vi_semiauto.mha'),
                                os.path.join(input_directory, 'mhd', sub_directory, 'followup_1_mask_vi_semiauto.mha'))

            point_set_0_file_name = os.path.join(input_directory, 'groundtruth', 'distinctivePoints',
                                                 sub_directory + '_baseline_1_Cropped_point.txt')
            point_set_1_file_name = os.path.join(input_directory, 'groundtruth', 'annotate', 'Consensus',
                                                 sub_directory + '_b1f1_point.txt')
            point_set_file_names = (point_set_0_file_name, point_set_1_file_name)

            disp_field_file_names = (os.path.join(self.name, sub_directory, 'movingfollowup_to_fixedbaseline.mha'),
                                     os.path.join(self.name, sub_directory, 'movingbaseline_to_fixedfollowup.mha'))

            file_names.append({
                "image_file_names": image_file_names,
                "mask_file_names": mask_file_names,
                "ground_truth_file_names": point_set_file_names,
                "disp_field_file_names": disp_field_file_names
            })

        self.file_names = take(sort_file_names(file_names),
                               max_number_of_registrations // 2)

    def evaluate(self, superelastix, file_names, output_directory):
        return self.evaluate_point_set(superelastix, file_names, output_directory)

    @staticmethod
    def read_point_set(file_name):
        return read_pts(file_name, skiprows=2)


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

                    displace_field_names = \
                        create_disp_field_names(image_names, self.name)

                    file_names.append({
                        "image_file_names": image_names,
                        "mask_file_names": mask_names,
                        "ground_truth_file_names": point_set_names,
                        "disp_field_file_names": displace_field_names,
                    })

        self.file_names = take(sort_file_names(file_names),
                               max_number_of_registrations // 2)

    def evaluate(self, superelastix, file_names, output_directory):
        return self.evaluate_point_set(superelastix, file_names, output_directory)

    def read_point_set(self, file_name):
        return self.read_csv(file_name)


def load_datasets(parameters):
    datasets = dict()

    if parameters.brain2d_input_directory is not None:
        logging.info('Loading dataset Brain2d.')
        brain2d = BRAIN2D(parameters.brain2d_input_directory,
                          parameters.output_directory)
        datasets[brain2d.name] = brain2d
        logging.debug('Using files:\n{0}'.format(json.dumps(brain2d.file_names, indent=2)))

    if parameters.lung2d_input_directory is not None:
        logging.info('Loading dataset Lung2d.')
        lung2d = LUNG2D(parameters.lung2d_input_directory,
                        parameters.output_directory)
        datasets[lung2d.name] = lung2d
        logging.debug('Using files:\n{0}'.format(json.dumps(lung2d.file_names, indent=2)))

    if parameters.cumc12_input_directory is not None:
        logging.info('Loading dataset CUMC12.')
        cumc12 = CUMC12(parameters.cumc12_input_directory,
                        parameters.output_directory,
                        parameters.max_number_of_registrations_per_dataset)
        datasets[cumc12.name] = cumc12
        logging.debug('Using files:\n{0}'.format(json.dumps(cumc12.file_names, indent=2)))

    if parameters.dirlab_input_directory is not None:
        logging.info('Loading dataset DIRLAB.')
        dirlab = DIRLAB(parameters.dirlab_input_directory,
                        parameters.dirlab_mask_directory,
                        parameters.output_directory,
                        parameters.max_number_of_registrations_per_dataset)
        datasets[dirlab.name] = dirlab
        logging.debug('Using files:\n{0}'.format(json.dumps(dirlab.file_names, indent=2)))

    if parameters.empire_input_directory is not None:
        logging.info('Loading dataset EMPIRE.')
        empire = EMPIRE(parameters.empire_input_directory,
                        parameters.max_number_of_registrations_per_dataset)
        datasets[empire.name] = empire
        logging.debug('Using files:\n{0}'.format(json.dumps(empire.file_names, indent=2)))

    if parameters.hammers_input_directory is not None:
        logging.info('Loading dataset HAMMERS.')
        hammers = HAMMERS(parameters.hammers_input_directory,
                        parameters.output_directory,
                        parameters.max_number_of_registrations_per_dataset)
        datasets[hammers.name] = hammers
        logging.debug('Using files:\n{0}'.format(json.dumps(hammers.file_names, indent=2)))

    if parameters.ibsr18_input_directory is not None:
        logging.info('Loading dataset IBSR18.')
        ibsr18 = IBSR18(parameters.ibsr18_input_directory,
                        parameters.output_directory,
                        parameters.max_number_of_registrations_per_dataset)
        datasets[ibsr18.name] = ibsr18
        logging.debug('Using files:\n{0}'.format(json.dumps(ibsr18.file_names, indent=2)))

    if parameters.lpba40_input_directory is not None:
        logging.info('Loading dataset LPBA40.')
        lpba40 = LPBA40(parameters.lpba40_input_directory,
                        parameters.output_directory,
                        parameters.max_number_of_registrations_per_dataset)
        datasets[lpba40.name] = lpba40
        logging.debug('Using files:\n{0}'.format(json.dumps(lpba40.file_names, indent=2)))

    if parameters.mgh10_input_directory is not None:
        logging.info('Loading dataset MGH10.')
        mgh10 = MGH10(parameters.mgh10_input_directory,
                      parameters.output_directory,
                      parameters.max_number_of_registrations_per_dataset)
        datasets[mgh10.name] = mgh10
        logging.debug('Using files:\n{0}'.format(json.dumps(mgh10.file_names, indent=2)))

    if parameters.popi_input_directory is not None:
        logging.info('Loading dataset POPI.')
        popi = POPI(parameters.popi_input_directory,
                    parameters.popi_mask_directory,
                    parameters.output_directory,
                    parameters.max_number_of_registrations_per_dataset)
        datasets[popi.name] = popi
        logging.debug('Using files:\n{0}'.format(json.dumps(popi.file_names, indent=2)))

    if parameters.spread_input_directory is not None:
        logging.info('Loading dataset SPREAD.')
        spread = SPREAD(parameters.spread_input_directory,
                        parameters.output_directory,
                        parameters.max_number_of_registrations_per_dataset)
        datasets[spread.name] = spread
        logging.debug('Using files:\n{0}'.format(json.dumps(spread.file_names, indent=2)))

    if parameters.hbia_input_directory is not None:
        logging.info('Loading dataset HistoBIA.')
        hbia = HBIA(parameters.hbia_input_directory,
                    parameters.output_directory,
                    parameters.max_number_of_registrations_per_dataset,
                    scale=10)
        datasets[hbia.name] = hbia
        logging.debug('Using files:\n{0}'.format(json.dumps(hbia.file_names, indent=2)))

    return datasets
