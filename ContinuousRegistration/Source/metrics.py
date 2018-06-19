import numpy as np
import SimpleITK as sitk
from ContinuousRegistration.Source.util import warp_image, warp_point_set
from ContinuousRegistration.Source.util import logging
from util import read_vtk, write_vtk

labelOverlapMeasurer = sitk.LabelOverlapMeasuresImageFilter()
labelOverlapMeasurer.SetGlobalDefaultCoordinateTolerance(1)

def tre(superelastix, point_sets, deformation_field_file_names):
    point_set_0_to_1 = warp_point_set(superelastix, point_sets[1], deformation_field_file_names[0])
    point_set_1_to_0 = warp_point_set(superelastix, point_sets[0], deformation_field_file_names[1])
    return (
        {'TRE': np.mean(np.sqrt(np.sum((point_set_0_to_1 - point_sets[1]) ** 2, -1)))},
        {'TRE': np.mean(np.sqrt(np.sum((point_set_1_to_0 - point_sets[0]) ** 2, -1)))}
    )

def hausdorff(superelastix, point_sets, deformation_field_file_names):
    point_set_0_to_1 = warp_point_set(superelastix, point_sets[0], deformation_field_file_names[0])
    point_set_1_to_0 = warp_point_set(superelastix, point_sets[1], deformation_field_file_names[1])

    return (
        {'Hausdorff': np.max(np.sqrt(np.sum((point_set_0_to_1 - point_sets[0]) ** 2, -1)))},
        {'Hausdorff': np.max(np.sqrt(np.sum((point_set_1_to_0 - point_sets[1]) ** 2, -1)))}
    )

def inverse_consistency_points(superelastix, point_sets, deformation_field_file_names):
    point_set_0_to_1 = warp_point_set(superelastix, point_sets[0], deformation_field_file_names[0])
    point_set_0_to_1_to_0 = warp_point_set(superelastix, point_set_0_to_1, deformation_field_file_names[1])
    point_set_1_to_0 = warp_point_set(superelastix, point_sets[1], deformation_field_file_names[1])
    point_set_1_to_0_to_1 = warp_point_set(superelastix, point_set_1_to_0, deformation_field_file_names[0])

    return (
        {'InverseConsistencyTRE': np.mean(np.sqrt(np.sum((point_set_0_to_1_to_0 - point_sets[0]) ** 2, -1)))},
        {'InverseConsistencyTRE': np.mean(np.sqrt(np.sum((point_set_1_to_0_to_1 - point_sets[1]) ** 2, -1)))}
    )


def inverse_consistency_labels(superelastix, label_file_names, deformation_field_file_names):
    label_image_0_to_1_file_name = warp_image(superelastix, label_file_names[0], deformation_field_file_names[1], sitk.sitkNearestNeighbor, 'label')
    label_image_0_to_1_to_0_file_name = warp_image(superelastix, label_image_0_to_1_file_name, deformation_field_file_names[0], sitk.sitkNearestNeighbor, 'label')
    label_image_1_to_0_file_name = warp_image(superelastix, label_file_names[1], deformation_field_file_names[0], sitk.sitkNearestNeighbor, 'label')
    label_image_1_to_0_to_0_file_name = warp_image(superelastix, label_image_1_to_0_file_name, deformation_field_file_names[1], sitk.sitkNearestNeighbor, 'label')

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
        {'InverseConsistencyDSC': dsc_0},
        {'InverseConsistencyDSC': dsc_1}
    )


def dice(superelastix, label_file_names, deformation_field_file_names):
    label_image_0_to_1_file_name = warp_image(superelastix, label_file_names[0], deformation_field_file_names[1], sitk.sitkNearestNeighbor, 'label')

    try:
        label_image_1 = sitk.ReadImage(label_file_names[1])
        labelOverlapMeasurer.Execute(label_image_1, sitk.Cast(sitk.ReadImage(label_image_0_to_1_file_name), label_image_1.GetPixelID()))
        dsc_0 = labelOverlapMeasurer.GetDiceCoefficient()
    except Exception as e:
        logging.error('Failed to compute DSC for %s' % label_file_names[0])
        raise(e)

    label_image_1_to_0_file_name = warp_image(superelastix, label_file_names[1], deformation_field_file_names[0], sitk.sitkNearestNeighbor, 'label')

    try:
        label_image_0 = sitk.ReadImage(label_file_names[0])
        labelOverlapMeasurer.Execute(label_image_0, sitk.Cast(sitk.ReadImage(label_image_1_to_0_file_name), label_image_0.GetPixelID()))
        dsc_1 = labelOverlapMeasurer.GetDiceCoefficient()
    except Exception as e:
        logging.error('Failed to compute DSC for %s' % label_file_names[1])
        raise(e)

    return (
        {'DSC': dsc_0},
        {'DSC': dsc_1}
    )
