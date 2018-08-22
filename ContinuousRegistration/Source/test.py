import unittest, os, shutil
from datetime import datetime
import numpy as np

from ContinuousRegistration.Source.metrics import tre, dice
from ContinuousRegistration.Source.util import warp_point_set

def tmp_file_name(output_directory, ext=".nii"):
    os.makedirs(output_directory, exist_ok=True)
    return os.path.join(output_directory, "{:%Y-%m-%d-%H:%M:%S.%f}".format(datetime.now()) + ".nii")

def create_vtk_file(output_directory, point):
    os.makedirs(output_directory, exist_ok=True)
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
    os.makedirs(output_directory, exist_ok=True)
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
    def __init__(self, *args, **kwargs):
        super(TestEvaulationMetrics, self).__init__(*args, **kwargs)

    def test_warp_point_set(self):
        deformation = np.array([1., 0., -5.])
        point_set = np.array([[1., 2., 3.]])

        deformation_field_file_name = create_deformation_field_file(os.environ['TMP_DIR'], deformation)
        warped_point_set = warp_point_set(os.environ['SUPERELASTIX'], point_set, deformation_field_file_name)
        assert(all(warped_point_set == [2., 2., -2.]))

    def test_tre(self):

        point_set_0 = np.array([[10.0, 7.0, 6.0]])
        point_set_1 = np.array([[9.0, 6.0, 8.0]])
        deformation_0 = np.array([-1., -1., 2.])
        deformation_1 = np.array([1., 1., -2.])

        deformation_field_file_name_0 = create_deformation_field_file(os.environ['TMP_DIR'], deformation_0)
        deformation_field_file_name_1 = create_deformation_field_file(os.environ['TMP_DIR'], deformation_1)
        tre_0, tre_1 = tre(os.environ['SUPERELASTIX'], (point_set_0, point_set_1), (deformation_field_file_name_0, deformation_field_file_name_1))

        assert(tre_0['TRE'] == 0.0)
        assert(tre_1['TRE'] == 0.0)

    def __del__(self):
        shutil.rmtree(os.environ['TMP_DIR'], ignore_errors=True)


if __name__ == '__main__':
    unittest.main()