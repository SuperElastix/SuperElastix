import os
import numpy as np

from evaluation_metrics import tre, hausdorff, singularity_ratio, inverse_consistency_points, merge_dicts

class SPREAD(object):
    def __init__(self, input_directory):
        self.name = 'SPREAD'
        self.category = 'Lung'

        self.input_directory = input_directory
        self.image_file_names = []
        self.point_set_file_names = []
        self.relative_deformation_field_file_names = []

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

            self.deformation_field_file_names.append((os.path.join(self.name, sub_directory, 'baseline_to_followup.nii'),
                                                               os.path.join(self.name, sub_directory, 'followup_to_baseline.nii')))


    def generator(self):
        for image_file_names, point_set_file_names, deformation_field_file_names in zip(self.image_file_names, self.point_set_file_names, self.deformation_field_file_names):
            yield {
                "image_file_names": image_file_names,
                "ground_truth_file_names": point_set_file_names,
                "deformation_field_file_names": deformation_field_file_names
            }


    def evaluate(self, registration_driver, file_names):

        tre_0, tre_1 = tre(registration_driver, file_names.point_set_file_names, file_names.deformation_field_file_names)
        hausdorff_0, hausdorff_1 = hausdorff(registration_driver, file_names.point_set_file_names, file_names.deformation_field_file_names_fullpath)
        singularity_ratio_0, singularity_ratio_1 = singularity_ratio(file_names.deformation_field_file_names_fullpath)
        inverse_consistency_points_0, inverse_consistency_points_1 = inverse_consistency_points(registration_driver, file_names.point_set_file_names, file_names.deformation_field_file_names_fullpath)

        result_0 = merge_dicts(tre_0, hausdorff_0, singularity_ratio_0, inverse_consistency_points_0)
        result_1 = merge_dicts(tre_1, hausdorff_1, singularity_ratio_1, inverse_consistency_points_1)

        return result_0, result_1


