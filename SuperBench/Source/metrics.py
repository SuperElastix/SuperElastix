import inspect
import subprocess
import os

import SimpleITK as sitk
import numpy as np

def load_point_set(file_name):
    return np.loadtxt(file_name, skiprows=3)

def warp_point_set(registration_driver, deformation_field_file_name, point_set_file_name, output_file_name):
    blueprint_file_name = os.path.join(os.path.dirname(inspect.stack()[0][1]), 'warp_point_set.json')
    stdout = subprocess.check_output([registration_driver,
                                      blueprint_file_name,
                                      deformation_field_file_name,
                                      point_set_file_name,
                                      output_file_name])

    return load_point_set(output_file_name)

def singularity_ratio(deformation_field_file_names):
    deformation_field_array_0 = sitk.GetArrayViewFromImage(sitk.ReadImage(deformation_field_file_names[0]))
    deformation_field_array_1 = sitk.GetArrayViewFromImage(sitk.ReadImage(deformation_field_file_names[1]))
    return ({
        'SingularityRatio': np.sum(deformation_field_array_0 < 0)/np.prod(deformation_field_array_0.shape),
    }, {
        'SingularityRatio': np.sum(deformation_field_array_1 < 0)/np.prod(deformation_field_array_1.shape),
    })

# def inverse_consistency:
#     pass
#     # TODO: Implement inverse consistensy
#     # For both point sets, warp points forward->backward, then call eval_point_sets and save TRE


def tre(registration_driver, point_set_file_names, deformation_field_file_names):
    output_file_name = os.path.join(os.path.dirname(registration_driver), 'warped_point_set.vtk')
    point_set_0_to_1 = warp_point_set(registration_driver, point_set_file_names[0], deformation_field_file_names[0], output_file_name)
    point_set_1_to_0 = warp_point_set(registration_driver, point_set_file_names[1], deformation_field_file_names[1], output_file_name)
    point_set_0 = np.loadtxt(point_set_file_names[0])
    point_set_1 = np.loadtxt(point_set_file_names[1])
    l2 = np.sqrt(np.sum((point_set_0 - point_set_1) ** 2, 1))
    return (
        { 'TRE': np.mean(np.sqrt(np.sum((point_set_0_to_1 - point_set_1) ** 2, 1))) },
        { 'TRE': np.mean(np.sqrt(np.sum((point_set_1_to_0 - point_set_0) ** 2, 1))) }
    )

def hausdorff(registration_driver, point_set_file_name_0, point_set_file_name_1, deformation_field_file_name_0, deformation_field_file_name_1):
    point_set_0_to_1 = warp_point_set(registration_driver, point_set_file_name_0, deformation_field_file_name_0)
    point_set_1_to_0 = warp_point_set(registration_driver, point_set_file_name_1, deformation_field_file_name_1)
    point_set_0 = np.loadtxt(point_set_file_name_0)
    point_set_1 = np.loadtxt(point_set_file_name_1)
    l2 = np.sqrt(np.sum((point_set_0 - point_set_1) ** 2, 1))
    return (
        { 'Hausdorff': np.max(np.sqrt(np.sum((point_set_0_to_1 - point_set_1) ** 2, 1))) },
        { 'Hausdorff': np.max(np.sqrt(np.sum((point_set_1_to_0 - point_set_0) ** 2, 1))) }
    )

def eval_segmentations(labels):
    pass