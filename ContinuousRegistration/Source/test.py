import unittest, os, shutil
from datetime import datetime
import SimpleITK as sitk

from ContinuousRegistration.Source.metrics import tre


def create_vtk_file(output_directory, point):
    output_name = "{:%Y-%m-%d-%H:%M:%S.%f}".format(datetime.now()) + ".vtk"
    output_path = os.path.join(output_directory, output_name)
    with open(output_path, 'w+') as f:
        f.write("# vtk DataFile Version 2.0\n")
        f.write("SuperBench test point set\n")
        f.write("ASCII\n")
        f.write("DATASET POLYDATA\n")
        f.write("POINTS 1 float\n")

        for p in point:
            f.write("%f " % p)

        f.write("\n")

    return output_path


def create_deformation_field_file(output_directory, deformation):
    size = (16, 17, 18)
    deformation_field = sitk.Image(size, sitk.sitkVectorFloat32, 3)

    # TODO: Handle nD
    for i in range(size[0]):
        for j in range(size[1]):
            for k in range(size[2]):
                deformation_field.SetPixel((i, j, k), deformation)

    output_file_name = os.path.join(output_directory, "{:%Y-%m-%d-%H:%M:%S.%f}".format(datetime.now()) + ".nii")
    sitk.WriteImage(deformation_field, output_file_name)
    return output_file_name


class TestEvaulationMetrics(unittest.TestCase):

    TEMP_DIR = os.path.abspath('temporary-test-folder')

    def __init__(self, *args, **kwargs):
        # TODO: Pass via command line
        self.registration_driver = ""
        super(TestEvaulationMetrics, self).__init__(*args, **kwargs)

        if not os.path.exists(self.TEMP_DIR):
            os.mkdir(self.TEMP_DIR)

    @unittest.skip("nonsense util `registration_driver` is given")
    def test_tre(self):
        point_0 = (10.0, 7.0, 6.0)
        point_1 = (9.0, 6.0, 8.0)
        deformation_0 = (-1., -1., 2.)
        deformation_1 = (1., 1., -2.)

        point_set_0 = create_vtk_file(self.TEMP_DIR, point_0)
        point_set_1 = create_vtk_file(self.TEMP_DIR, point_1)
        deformation_field_0 = create_deformation_field_file(self.TEMP_DIR, deformation_0)
        deformation_field_1 = create_deformation_field_file(self.TEMP_DIR, deformation_1)
        tre_0, tre_1 = tre(self.registration_driver, (point_set_0, point_set_1),
                           (deformation_field_0, deformation_field_1))

        assert(tre_0['TRE'] == 0.0)
        assert(tre_1['TRE'] == 0.0)

    def test_create_vtk_file(self):
        path_file = create_vtk_file(self.TEMP_DIR, [7, 42])
        assert(os.path.isfile(path_file))

    def __del__(self):
        shutil.rmtree(self.TEMP_DIR, ignore_errors=True)


if __name__ == '__main__':
    unittest.main()