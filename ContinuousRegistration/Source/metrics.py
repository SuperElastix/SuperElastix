import logging
import numpy as np
import SimpleITK as sitk
from ContinuousRegistration.Source.util import load_point_set, load_vtk
from ContinuousRegistration.Source.util import warp_label_image, warp_point_set

labelOverlapMeasurer = sitk.LabelOverlapMeasuresImageFilter()
labelOverlapMeasurer.SetGlobalDefaultCoordinateTolerance(1)

def tre(registration_driver, point_set_file_names, deformation_field_file_names):
    point_set_0_to_1 = load_vtk(warp_point_set(registration_driver, point_set_file_names[0], deformation_field_file_names[0]))
    point_set_1_to_0 = load_vtk(warp_point_set(registration_driver, point_set_file_names[1], deformation_field_file_names[1]))
    point_set_0 = load_point_set(point_set_file_names[0])
    point_set_1 = load_point_set(point_set_file_names[1])
    return (
        { 'TRE': np.mean(np.sqrt(np.sum((point_set_0_to_1 - point_set_1) ** 2, -1))) },
        { 'TRE': np.mean(np.sqrt(np.sum((point_set_1_to_0 - point_set_0) ** 2, -1))) }
    )

def hausdorff(registration_driver, point_set_file_names, deformation_field_file_names):
    point_set_0_to_1 = load_vtk(warp_point_set(registration_driver, point_set_file_names[0], deformation_field_file_names[0]))
    point_set_1_to_0 = load_vtk(warp_point_set(registration_driver, point_set_file_names[1], deformation_field_file_names[1]))
    point_set_0 = load_point_set(point_set_file_names[0])
    point_set_1 = load_point_set(point_set_file_names[1])
    return (
        { 'Hausdorff': np.max(np.sqrt(np.sum((point_set_0_to_1 - point_set_1) ** 2, -1))) },
        { 'Hausdorff': np.max(np.sqrt(np.sum((point_set_1_to_0 - point_set_0) ** 2, -1))) }
    )

def inverse_consistency_points(registration_driver, point_set_file_names, deformation_field_file_names):
    point_set_0_to_1_file_name = warp_point_set(registration_driver, point_set_file_names[0], deformation_field_file_names[0])
    point_set_0_to_1_to_0_file_name = warp_point_set(registration_driver, point_set_0_to_1_file_name, deformation_field_file_names[1])
    point_set_1_to_0_file_name = warp_point_set(registration_driver, point_set_file_names[1], deformation_field_file_names[1])
    point_set_1_to_0_to_1_file_name = warp_point_set(registration_driver, point_set_1_to_0_file_name, deformation_field_file_names[0])
    point_set_0 = load_point_set(point_set_file_names[0])
    point_set_1 = load_point_set(point_set_file_names[1])
    return (
        { 'InverseConsistencyTRE': np.mean(np.sqrt(np.sum((load_vtk(point_set_0_to_1_to_0_file_name) - point_set_0) ** 2, -1))) },
        { 'InverseConsistencyTRE': np.mean(np.sqrt(np.sum((load_vtk(point_set_1_to_0_to_1_file_name) - point_set_1) ** 2, -1))) }
    )


def inverse_consistency_labels(registration_driver, label_file_names, deformation_field_file_names):
    label_image_0_to_1_file_name = warp_label_image(registration_driver, label_file_names[0], deformation_field_file_names[1])
    label_image_0_to_1_to_0_file_name = warp_label_image(registration_driver, label_image_0_to_1_file_name, deformation_field_file_names[0])
    label_image_1_to_0_file_name = warp_label_image(registration_driver, label_file_names[1], deformation_field_file_names[0])
    label_image_1_to_0_to_0_file_name = warp_label_image(registration_driver, label_image_1_to_0_file_name, deformation_field_file_names[1])

    try:
        label_image_0 = sitk.ReadImage(label_file_names[0])
        labelOverlapMeasurer.Execute(label_image_0, sitk.Cast(sitk.ReadImage(label_image_0_to_1_to_0_file_name), label_image_0.GetPixelID()))
        dsc_0 = labelOverlapMeasurer.GetDiceCoefficient()
    except Exception as e:
        logging.error('Failed to compute inverse consistency DSC for %s' % label_file_names[0])
        raise(e)

    try:
        label_image_1 = sitk.ReadImage(label_file_names[1])
        labelOverlapMeasurer.Execute(label_image_1, sitk.Cast(sitk.ReadImage(label_image_1_to_0_to_0_file_name), label_image_1.GetPixelID()))
        dsc_1 = labelOverlapMeasurer.GetDiceCoefficient()
    except Exception as e:
        logging.error('Failed to compute inverse consistency DSC for %s' % label_file_names[1])
        raise(e)

    return (
        { 'InverseConsistencyDSC': dsc_0 },
        { 'InverseConsistencyDSC': dsc_1 }
    )


def dice(registration_driver, label_file_names, deformation_field_file_names):
    label_image_0_to_1_file_name = warp_label_image(registration_driver, label_file_names[0], deformation_field_file_names[1])

    try:
        label_image_1 = sitk.ReadImage(label_file_names[1])
        labelOverlapMeasurer.Execute(label_image_1, sitk.Cast(sitk.ReadImage(label_image_0_to_1_file_name), label_image_1.GetPixelID()))
        dsc_0 = labelOverlapMeasurer.GetDiceCoefficient()
    except Exception as e:
        logging.error('Failed to compute DSC for %s' % label_file_names[0])
        raise(e)

    label_image_1_to_0_file_name = warp_label_image(registration_driver, label_file_names[1], deformation_field_file_names[0])

    try:
        label_image_0 = sitk.ReadImage(label_file_names[0])
        labelOverlapMeasurer.Execute(label_image_0, sitk.Cast(sitk.ReadImage(label_image_1_to_0_file_name), label_image_0.GetPixelID()))
        dsc_1 = labelOverlapMeasurer.GetDiceCoefficient()
    except Exception as e:
        logging.error('Failed to compute DSC for %s' % label_file_names[1])
        raise(e)

    return (
        { 'DSC': dsc_0 },
        { 'DSC': dsc_1 }
    )
