import os, glob, csv, re

from evaluation_metrics import tre, hausdorff, singularity_ratio, inverse_consistency_points, merge_dicts

class DIRLAB(object):
    def __init__(self, input_directory):
        self.name = 'DIRLAB'
        self.category = 'Lung'

        self.input_directory = input_directory
        self.image_file_names = []
        self.point_set_file_names = []
        self.relative_deformation_field_file_names = []

        # DIR-LAB provides raw binary image only so we write mhd header files for loading the data.
        # The image information was retrieved from https://www.dir-lab.com/ReferenceData.html
        dirlab_image_information_file_name = os.path.join(os.path.dirname(__file__), "dirlab_image_information.csv")

        # Find corresponding subdirectory which contains the id and associate it with this line of the csv file.
        # TODO: Maybe not the prettiest approach
        sub_directories = [directory for directory in os.listdir(self.input_directory) if os.path.isdir(os.path.join(input_directory, directory))]
        sub_directory_ids = list(map(lambda str: int(re.search(r'\d+', str).group()), sub_directories))
        dirlab_image_information = dict()
        for image_information in csv.DictReader(open(dirlab_image_information_file_name)):
            dirlab_image_information[image_information['id']] = image_information
            dirlab_image_information[image_information['id']]['sub_directory'] = sub_directories[sub_directory_ids.index(int(image_information['id']))]

        # Now we have all the information necessary for generating the required files.
        for id in dirlab_image_information:
            img_0_file_name = glob.glob(os.path.join(input_directory, dirlab_image_information[id]['sub_directory'], 'Images', '*T00*.img'))[0]
            img_1_file_name = glob.glob(os.path.join(input_directory, dirlab_image_information[id]['sub_directory'], 'Images', '*T50*.img'))[0]

            # Write mhd header files
            mhd_0_file_name = os.path.splitext(img_0_file_name)[0] + '.mhd'
            with open(mhd_0_file_name, 'w') as mhd:
                mhd.write('ObjectType = Image\n')
                mhd.write('NDims = 3\n')
                mhd.write('BinaryData = True\n')
                mhd.write('DimSize = %s %s %s\n' % (dirlab_image_information[id]['x_size'], dirlab_image_information[id]['y_size'], dirlab_image_information[id]['z_size']))
                mhd.write('ElementSpacing = %s %s %s\n' % (dirlab_image_information[id]['x_spacing'], dirlab_image_information[id]['y_spacing'], dirlab_image_information[id]['z_spacing']))
                mhd.write('ElementTYPE = MET_USHORT\n')
                mhd.write('ElementDataFile = %s\n' % img_0_file_name)

            mhd_1_file_name = os.path.splitext(img_1_file_name)[0] + '.mhd'
            with open(mhd_1_file_name, 'w') as mhd:
                mhd.write('ObjectType = Image\n')
                mhd.write('NDims = 3\n')
                mhd.write('BinaryData = True\n')
                mhd.write('DimSize = %s %s %s\n' % (dirlab_image_information[id]['x_size'], dirlab_image_information[id]['y_size'], dirlab_image_information[id]['z_size']))
                mhd.write('ElementSpacing = %s %s %s\n' % (dirlab_image_information[id]['x_spacing'], dirlab_image_information[id]['y_spacing'], dirlab_image_information[id]['z_spacing']))
                mhd.write('ElementTYPE = MET_USHORT\n')
                mhd.write('ElementDataFile = %s\n' % img_1_file_name)

            self.image_file_names.append((mhd_0_file_name, mhd_1_file_name))

            point_set_0 = glob.glob(os.path.join(input_directory, dirlab_image_information[id]['sub_directory'], 'ExtremePhases', '*T00_xyz.txt'))[0]
            point_set_1 = glob.glob(os.path.join(input_directory, dirlab_image_information[id]['sub_directory'], 'ExtremePhases', '*T50_xyz.txt'))[0]
            self.point_set_file_names.append((point_set_0, point_set_1))

            self.relative_deformation_field_file_names.append((os.path.join(self.name, dirlab_image_information[id]['sub_directory'], '00_to_50.nii'),
                                                               os.path.join(self.name, dirlab_image_information[id]['sub_directory'], '50_to_00.nii')))



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
        singularity_ratio_0, singularity_ratio_1 = singularity_ratio(deformation_field_file_names)
        inverse_consistency_points_0, inverse_consistency_points_1 = inverse_consistency_points(registration_driver, point_set_file_names, deformation_field_file_names)

        result_0 = merge_dicts(tre_0, hausdorff_0, singularity_ratio_0, inverse_consistency_points_0)
        result_1 = merge_dicts(tre_1, hausdorff_1, singularity_ratio_1, inverse_consistency_points_1)

        return result_0, result_1

