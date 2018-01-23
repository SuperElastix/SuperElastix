import os
from itertools import combinations

from evaluation_metrics import tre, hausdorff, singularity_ratio, inverse_consistency_atlas, merge_dicts

class MGH10(object):
    def __init__(self, input_directory):
        self.name = 'MGH10'
        self.category = 'Brain'

        self.input_directory = input_directory
        self.image_file_names = []
        self.atlas_file_names = []
        self.deformation_field_file_names = []

        image_file_names = [os.path.join(input_directory, 'Heads', image) for image in os.listdir(os.path.join(input_directory, 'Heads')) if image.endswith('.hdr')]
        self.image_file_names = [pair for pair in combinations(image_file_names, 2)]

        atlas_file_names = [os.path.join(input_directory, 'Heads', atlas) for atlas in os.listdir(os.path.join(input_directory, 'Atlases')) if atlas.endswith('.hdr')]
        self.atlas_file_names = [pair for pair in combinations(atlas_file_names, 2)]

        for image_file_name_0, image_file_name_1 in self.image_file_names:
            image_file_name_we_0, image_extension_we_0 = os.path.splitext(image_file_name_0)
            image_file_name_we_1, image_extension_we_1 = os.path.splitext(image_file_name_1)
            self.deformation_field_file_names.append((os.path.join(self.name, image_file_name_we_0 + "_to_" + image_file_name_we_1 + ".nii"),
                                                      os.path.join(self.name, image_file_name_we_1 + "_to_" + image_file_name_we_0 + ".nii")))


    def generator(self):
        for image_file_names, atlas_file_names, deformation_field_file_names in zip(self.image_file_names, self.atlas_file_names, self.deformation_field_file_names):
            yield {
                "image_file_names": image_file_names,
                "ground_truth_file_names": atlas_file_names,
                "deformation_field_file_names": deformation_field_file_names
            }


    def evaluate(self, registration_driver, file_names):

        singularity_ratio_0, singularity_ratio_1 = singularity_ratio(file_names.deformation_field_file_names_fullpath)
        inverse_consistency_atlas_0, inverse_consistency_atlas_1 = inverse_consistency_atlas(registration_driver, file_names.atlas_file_names, file_names.deformation_field_file_names_fullpath)

        result_0 = merge_dicts(singularity_ratio_0, inverse_consistency_atlas_0)
        result_1 = merge_dicts(singularity_ratio_1, inverse_consistency_atlas_1)

        return result_0, result_1


