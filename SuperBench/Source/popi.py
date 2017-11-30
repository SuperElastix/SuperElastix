import os
import subprocess
import numpy as np
import scipy
import SimpleITK as sitk
from .eval import eval_point_sets, eval_singularity_ratio

class POPI(object):
    def __init__(self, input_directory):
        self.name = 'POPI'
        self.input_directory = input_directory
        self.image_file_names = []
        self.point_set_file_names = []
        self.relative_deformation_field_file_names = []

        sub_directories = [directory for directory in os.listdir(self.input_directory) if not directory.startswith('.')]

        for sub_directory in sub_directories:
            self.image_file_names.append((os.path.join(input_directory, sub_directory, 'mhd', '00.mhd'),
                                          os.path.join(input_directory, sub_directory, 'mhd', '50.mhd')))
            self.point_set_file_names.append((os.path.join(input_directory, sub_directory, 'pts', '00.pts'),
                                              os.path.join(input_directory, sub_directory, 'pts', '50.pts')))
            self.relative_deformation_field_file_names.append((os.path.join(self.name, sub_directory, '00->50.mhd'),
                                                               os.path.join(self.name, sub_directory, '50->00.mhd')))

    def generator(self):
        for image_file_name_pair, point_set_file_name_pair, relative_deformation_field_file_name_pair in zip(self.image_file_names, self.point_set_file_names, self.relative_deformation_field_file_names):
            yield image_file_name_pair, point_set_file_name_pair, relative_deformation_field_file_name_pair

    def evaluate(self, superelastix, image_file_name_pair, point_set_file_name_pair, relative_deformation_field_file_name_pair):
        # TODO: Warp point sets
        # TODO: Implement inverse consistency

        # TODO: Is there a better way to merge dicts?
        result = {}
        result.update(eval_point_sets(np.loadtxt(point_set_file_name_pair[0]),
                                      np.loadtxt(point_set_file_name_pair[1])))
        result.update(eval_singularity_ratio(sitk.ReadImage(relative_deformation_field_file_name_pair[0]),
                                             sitk.ReadImage(relative_deformation_field_file_name_pair[1])))

        return result

