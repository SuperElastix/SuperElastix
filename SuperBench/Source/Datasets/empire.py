import os
from itertools import combinations

from evaluation_metrics import tre, hausdorff, singularity_ratio, inverse_consistency_points, merge_dicts

class EMPIRE(object):
    def __init__(self, input_directory):
        self.name = 'EMPIRE'
        self.category = 'Lung'

        self.input_directory = input_directory
        self.image_file_names = []
        self.point_set_file_names = []
        self.deformation_field_file_names = []

        for i in range(0, 31):
            self.image_file_names.append = (os.path.join(input_directory, 'scans', "%02d" % i + '_Fixed.mhd'),
                                            os.path.join(input_directory, 'scans', "%02d" % i + '_Moving.mhd'))

            # TODO: Find out output format
            self.deformation_field_file_names.append(("%02d" % i + '_Fixed.mhd',
                                                     "%02d" % i + '_Moving.mhd'))

    def generator(self):
        for image_file_names, deformation_field_file_names in zip(self.image_file_names, self.deformation_field_file_names):
            yield image_file_names, 'dummy_string', deformation_field_file_names


    def evaluate(self,
                 registration_driver,
                 image_file_names,
                 point_set_file_names,
                 deformation_field_file_names):

        # TODO
        pass

