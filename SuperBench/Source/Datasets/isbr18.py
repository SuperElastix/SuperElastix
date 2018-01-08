import os
from itertools import combinations

from evaluation_metrics import tre, hausdorff, singularity_ratio, inverse_consistency_points, merge_dicts

class ISBR18(object):
    def __init__(self, input_directory):
        self.name = 'ISBR18'
        self.category = 'Brain'

        self.input_directory = input_directory
        self.image_file_names = []
        self.point_set_file_names = []
        self.relative_deformation_field_file_names = []

        self.image_file_names = combinations([image for image in os.listdir(os.path.join(input_directory, 'Heads')) if image.endswith('.hdr')], 2)
        self.atlas_file_names = combinations([atlas for atlas in os.listdir(os.path.join(input_directory, 'Atlases')) if atlas.endswith('.hdr')], 2)
        self.deformation_field_file_names = [(os.path.join(self.name, file_name_0), os.path.join(self.name, file_name_1)) for file_name_0, file_name_1 in self.image_file_names]


    def generator(self):
        for image_file_names, point_set_file_names, deformation_field_file_names in zip(self.image_file_names, self.point_set_file_names, self.relative_deformation_field_file_names):
            yield image_file_names, point_set_file_names, deformation_field_file_names


    def evaluate(self,
                 registration_driver,
                 image_file_names,
                 point_set_file_names,
                 deformation_field_file_names):

        singularity_ratio_0, singularity_ratio_1 = singularity_ratio(deformation_field_file_names)
        inverse_consistency_points_0, inverse_consistency_points_1 = inverse_consistency_points(registration_driver, point_set_file_names, deformation_field_file_names)

        result_0 = merge_dicts(singularity_ratio_0, inverse_consistency_points_0)
        result_1 = merge_dicts(singularity_ratio_1, inverse_consistency_points_1)

        return result_0, result_1

