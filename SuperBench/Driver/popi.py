import os

class POPI(object):
    def __init__(self, input_directory):
        self.input_directory = input_directory
        self.image_file_names = []
        self.point_set_file_names = []
        self.relative_output_file_names = []

        sub_directories = [directory for directory in os.listdir(self.input_directory) if not directory.startswith('.')]

        for sub_directory in sub_directories:
            self.image_file_names.append((os.path.join(input_directory, sub_directory, 'mhd', '00.mhd'),
                                          os.path.join(input_directory, sub_directory, 'mhd', '50.mhd')))
            self.point_set_file_names.append((os.path.join(input_directory, sub_directory, 'pts', '00.pts'),
                                              os.path.join(input_directory, sub_directory, 'pts', '50.pts')))
            self.relative_output_file_names.append(os.path.join('POPI', sub_directory, '00->50.mhd'))

            self.image_file_names.append((os.path.join(input_directory, sub_directory, 'mhd', '50.mhd'),
                                          os.path.join(input_directory, sub_directory, 'mhd', '00.mhd')))
            self.point_set_file_names.append((os.path.join(input_directory, sub_directory, 'pts', '50.pts'),
                                              os.path.join(input_directory, sub_directory, 'pts', '00.pts')))
            self.relative_output_file_names.append(os.path.join('POPI', sub_directory, '50->00.mhd'))

    def data(self):
        for image_pair_file_names, point_set_pair_file_names, relative_output_file_name in zip(self.image_file_names, self.point_set_file_names, self.relative_output_file_names):
            yield image_pair_file_names, point_set_pair_file_names, relative_output_file_name
