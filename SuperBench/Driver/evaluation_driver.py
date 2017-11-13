import argparse
import subprocess
import os
import SimpleITK as sitk
import numpy as np

from popi import POPI

def deform_point_set(tmp_directory, deformation_field_file_name, point_set):
    tfx_point_set_0_file_name = os.path.join(tmp_directory, 'point_set.pts')
    with open(tfx_point_set_0_file_name, 'w') as f:
        f.write('point')
        f.write(str(point_set.rows()))
        for point in point_set:
            f.write('%f %f %f' % point)

    transformixImageFilter = sitk.TransformixImageFilter()
    transformixImageFilter.SetMovingImage(sitk.ReadImage(deformation_field_file_name)) # Only used for image information
    transformixImageFilter.SetFixedPointSetFileName(tfx_point_set_0_file_name)

    transformParameterMap = sitk.ParameterMap()
    transformParameterMap['Transform'] = ['DeformationFieldTransform']
    transformParameterMap['DeformationFieldFileName'] = [deformation_field_file_name]

    transformixImageFilter.SetTransformParameterMap(transformParameterMap)
    transformixImageFilter.Execute()

    # If midpoint reg algorithm, we cannot use this approach

def tre(deformation_field, point_set_0, point_set_1):
    tfx_point_set_0_file_name =
    target_registration_error = np.zeros((point_set_0.shape[0], 1))
    for point_0, point_1 in zip(point_set_0, point_set_1):
        target_registration_error[0] = (point_set_1-point_set_0).sum().sqrt()

def run(parameters):

    datasets = []

    if parameters.popi_input_directory is not None:
        datasets.append(POPI(parameters.popi_input_directory))

    parameter_file_names = [os.path.join(parameters.parameter_file_directory, parameter_file_name)
                            for parameter_file_name
                            in os.listdir(parameters.parameter_file_directory)
                            if parameter_file_name.endswith('.json')
                            or parameter_file_name.endswith('.xml')]

    # Run evaluations
    for parameter_file_name in os.listdir(parameters.parameter_file_directory):
        parameter_file_base_name = os.path.basename(parameter_file_name)
        if not parameter_file_base_name.startswith('elastix'): continue  # DBG
        for dataset in datasets:
            for image_file_names, point_set_file_names, relative_output_file_name in dataset.data():
                try:
                    deformation_field_file_name = os.path.join(parameters.output_directory,
                                                               parameter_file_base_name,
                                                               relative_output_file_name)

                    deformation_field = sitk.ReadImage(deformation_field_file_name)
                    point_set_0 = np.loadtxt(point_set_file_names[0])
                    point_set_1 = np.loadtxt(point_set_file_names[1])
                    print(tre(deformation_field, point_set_0, point_set_1))

                    pass
                except Exception as e:
                    # TODO: Handle failed evaluation
                    print(e)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='SuperBench evaluation driver.')

    # Data sets
    parser.add_argument('--parameter-file-directory', '-pfd', required=True, help='Directory with parameter files.')
    parser.add_argument('--output-directory', '-od', required=True)
    parser.add_argument('--popi-input-directory', '-pid', help='Directory with POPI data.')

    parameters = parser.parse_args()
    run(parameters)

