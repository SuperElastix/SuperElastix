import os
import subprocess
import numpy as np
import scipy
import SimpleITK as sitk

from metrics import tre, hausdorff

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
        for (image_file_name_pair,
             point_set_file_name_pair,
             relative_deformation_field_file_name_pair) in zip(self.image_file_names,
                                                               self.point_set_file_names,
                                                               self.relative_deformation_field_file_names):
            yield image_file_name_pair, point_set_file_name_pair, relative_deformation_field_file_name_pair

    def evaluate(self,
                 parameters,
                 image_file_names,
                 point_set_file_names,
                 deformation_field_file_names):

        result_0, result_1 = tre(parameters.registration_driver, point_set_file_names, deformation_field_file_names)

        deformation_field_array_0 = sitk.GetArrayViewFromImage(sitk.ReadImage(deformation_field_file_names[0]))
        deformation_field_array_1 = sitk.GetArrayViewFromImage(sitk.ReadImage(deformation_field_file_names[1]))
        deformation_field_eval_0, deformation_field_eval_1 \
            = metrics.eval_deformation_field(parameters.registraiton_driver, point_set_file_names, deformation_field_file_names)

        result_0.update(deformation_field_eval_0)
        result_1.update(deformation_field_eval_1)

        return deformation_field_eval_0, deformation_field_eval_1

