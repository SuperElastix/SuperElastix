import os, glob
from itertools import combinations

from evaluation_metrics import singularity_ratio, inverse_consistency_points, merge_dicts

class LPBA40(object):
    def __init__(self, input_directory):
        self.name = 'LPBA40'
        self.category = 'Brain'

        self.input_directory = input_directory
        self.image_file_names = []
        self.atlas_file_names = []
        self.deformation_field_file_names = []

        image_file_names = [glob.glob(os.path.join(self.input_directory, 'delineation_space', sub_directory, '*.delineation.skullstripped.hdr'))[0] for sub_directory in os.listdir(os.path.join(self.input_directory, 'delineation_space')) if os.path.isdir(os.path.join(self.input_directory, 'delineation_space', sub_directory))]
        self.image_file_names = [pair for pair in combinations(image_file_names, 2)]

        atlas_file_names = [glob.glob(os.path.join(self.input_directory, 'delineation_space', sub_directory, '*.delineation.skullstripped.hdr'))[0] for sub_directory in os.listdir(os.path.join(self.input_directory, 'delineation_space')) if os.path.isdir(os.path.join(self.input_directory, 'delineation_space', sub_directory))]
        self.atlas_file_names = [pair for pair in combinations(atlas_file_names, 2)]

        for image_file_name_0, image_file_name_1 in self.image_file_names:
            image_file_name_0 = os.path.basename(image_file_name_0)
            image_file_name_1 = os.path.basename(image_file_name_1)
            image_file_name_we_0 = os.path.splitext(image_file_name_0)[0]
            image_file_name_we_1 = os.path.splitext(image_file_name_1)[0]
            self.deformation_field_file_names.append((os.path.join(self.name, image_file_name_we_0 + "_to_" + image_file_name_we_1 + ".nii"),
                                                      os.path.join(self.name, image_file_name_we_1 + "_to_" + image_file_name_we_0 + ".nii")))


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

