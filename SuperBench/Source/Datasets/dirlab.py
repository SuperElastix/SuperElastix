import os
import glob

from evaluation_metrics import tre, hausdorff, singularity_ratio, inverse_consistency_points, merge_dicts

class DIRLAB(object):
    def __init__(self, input_directory):
        self.name = 'DIRLAB'
        self.category = 'Lung'
        self.input_directory = input_directory
        self.image_file_names = []
        self.point_set_file_names = []
        self.relative_deformation_field_file_names = []

        sub_directories = [directory for directory in os.listdir(self.input_directory) if os.path.isdir(os.path.join(input_directory, directory))]

        for sub_directory in sub_directories:
            image_0 = glob.glob(os.path.join(input_directory, sub_directory, 'Images', '*T00*'))[0]
            image_1 = glob.glob(os.path.join(input_directory, sub_directory, 'Images', '*T50*'))[0]
            self.image_file_names.append((image_0, image_1))

            point_set_0 = glob.glob(os.path.join(input_directory, sub_directory, 'ExtremePhases', '*T00_xyz.txt'))[0]
            point_set_1 = glob.glob(os.path.join(input_directory, sub_directory, 'ExtremePhases', '*T50_xyz.txt'))[0]
            self.point_set_file_names.append((point_set_0, point_set_1))

            self.relative_deformation_field_file_names.append((os.path.join(self.name, sub_directory, '00_to_50.nii'),
                                                               os.path.join(self.name, sub_directory, '50_to_00.nii')))


    def generator(self):
        for image_file_names, point_set_file_names, deformation_field_file_names in zip(self.image_file_names, self.point_set_file_names, self.relative_deformation_field_file_names):
            yield image_file_names, point_set_file_names, deformation_field_file_names


    def evaluate(self,
                 registration_driver,
                 image_file_names,
                 point_set_file_names,
                 deformation_field_file_names):

        tre_0, tre_1 = tre(registration_driver, point_set_file_names, deformation_field_file_names)
        hausdorff_0, hausdorff_1 = hausdorff(registration_driver, point_set_file_names, deformation_field_file_names)
        singularity_ratio_0, singularity_ratio_1 = singularity_ratio(deformation_field_file_names)
        inverse_consistency_points_0, inverse_consistency_points_1 = inverse_consistency_points(registration_driver, point_set_file_names, deformation_field_file_names)

        result_0 = merge_dicts(tre_0, hausdorff_0, singularity_ratio_0, inverse_consistency_points_0)
        result_1 = merge_dicts(tre_1, hausdorff_1, singularity_ratio_1, inverse_consistency_points_1)

        return result_0, result_1

