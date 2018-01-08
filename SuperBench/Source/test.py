import unittest, os, tempfile
from datetime import datetime
import SimpleITK as sitk

from evaluation_metrics import tre

def create_vtk_file(output_directory, point):
    output_file_name = os.path.join(output_directory, "{:%Y-%m-%d-%H:%M:%S.%f}".format(datetime.now()) + ".vtk")
    with open(output_file_name, 'w+') as f:
        f.write("# vtk DataFile Version 2.0\n")
        f.write("SuperBench test point set\n")
        f.write("ASCII\n")
        f.write("DATASET POLYDATA\n")
        f.write("POINTS 1 float\n")

        for p in point:
            f.write("%f " % p)

        f.write("\n")

    return output_file_name


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
    def __init__(self, *args, **kwargs):
        # TODO: Pass via command line
        self.registration_driver = "/Users/kasper/Development/build/SuperElastix/SuperElastix-build/SuperBench/Source/Drivers/localhost.sh"
        super(TestEvaulationMetrics, self).__init__(*args, **kwargs)

    def test_tre(self):
        point_0 = (10.0, 7.0, 6.0)
        point_1 = (9.0, 6.0, 8.0)
        deformation_0 = (-1., -1., 2.)
        deformation_1 = (1., 1., -2.)
        with tempfile.TemporaryDirectory() as tmpdir:
            point_set_0 = create_vtk_file(tmpdir, point_0)
            point_set_1 = create_vtk_file(tmpdir, point_1)
            deformation_field_0 = create_deformation_field_file(tmpdir, deformation_0)
            deformation_field_1 = create_deformation_field_file(tmpdir, deformation_1)
            tre_0, tre_1 = tre(self.registration_driver, (point_set_0, point_set_1), (deformation_field_0, deformation_field_1))

        a = 1

if __name__ == '__main__':
    unittest.main()