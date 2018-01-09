import os
from itertools import combinations

from evaluation_metrics import singularity_ratio, inverse_consistency_points, merge_dicts

class LBPA40(object):
    def __init__(self, input_directory):
        self.name = 'LBPA40'
        self.category = 'Brain'

        self.input_directory = input_directory
        self.image_file_names = []
        self.atlas_file_names = []
        self.relative_deformation_field_file_names = []

        # TODO: Random subsample?
        n = 10
        for i in range(1, n):
            for j in range(1, n):
                self.image_file_names.append((os.path.join(input_directory, 'LPBA40_rigidly_registered_pairs', 'l{0}_to_l{1}.hdr'.format(i, j)),
                                              os.path.join(input_directory, 'LPBA40_rigidly_registered_pairs', 'l{1}_to_l{0}.hdr'.format(i, j))))

                self.atlas_file_names.append((os.path.join(input_directory, 'LPBA40_rigidly_registered_label_pairs', 'l{0}_to_l{1}.hdr'.format(i, j)),
                                              os.path.join(input_directory, 'LPBA40_rigidly_registered_label_pairs', 'l{1}_to_l{0}.hdr'.format(i, j))))

                self.deformation_field_file_names.append((os.path.join(self.name, 'l{0}_to_l{1}.hdr'.format(i, j)),
                                              os.path.join(self.name, 'l{1}_to_l{0}.hdr'.format(i, j))))


    def generator(self):
        for image_file_names, atals_file_names, deformation_field_file_names in zip(self.image_file_names, self.atlas_file_names, self.deformation_field_file_names):
            yield image_file_names, atals_file_names, deformation_field_file_names


    def evaluate(self,
                 registration_driver,
                 image_file_names,
                 atlas_file_names,
                 deformation_field_file_names):

        singularity_ratio_0, singularity_ratio_1 = singularity_ratio(deformation_field_file_names)
        inverse_consistency_points_0, inverse_consistency_points_1 = inverse_consistency_points(registration_driver, atlas_file_names, deformation_field_file_names)

        result_0 = merge_dicts(singularity_ratio_0, inverse_consistency_points_0)
        result_1 = merge_dicts(singularity_ratio_1, inverse_consistency_points_1)

        return result_0, result_1

