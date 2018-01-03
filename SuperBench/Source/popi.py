import os
import subprocess
import numpy as np
import scipy
import SimpleITK as sitk

from metrics import tre, hausdorff, singularity_ratio, inverse_consistency

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
                                                               os.path.join(self.name, sub_directory, '00<-50.mhd')))

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
        singularity_ratio_0, singularity_ratio_1 = singularity_ratio(registration_driver, deformation_field_file_names)
        inverse_consistency_0, inverse_consistency_1 = inverse_consistency(registration_driver, point_set_file_names, deformation_field_file_names)

        return tre_0.update(hausdorff_0, singularity_ratio_0, inverse_consistency_0), tre_1.update(hausdorff_1, singularity_ratio_1, inverse_consistency_1)

