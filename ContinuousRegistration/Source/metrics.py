import numpy as np
import SimpleITK as sitk
from ContinuousRegistration.Source.util import warp_image, warp_point_set
from ContinuousRegistration.Source.util import logging
from ContinuousRegistration.Source.util import read_vtk, write_vtk

labelOverlapMeasurer = sitk.LabelOverlapMeasuresImageFilter()
labelOverlapMeasurer.SetGlobalDefaultCoordinateTolerance(1)

def tre(superelastix, point_sets, deformation_field_file_names):
    try:
        point_set_0_to_1 = warp_point_set(superelastix, point_sets[1], deformation_field_file_names[0])
        point_set_1_to_0 = warp_point_set(superelastix, point_sets[0], deformation_field_file_names[1])
    except Exception as e:
        logging.error('Failed to compute tre for image pair (%s, %s).' % deformation_field_file_names)
        return { 'TRE': np.NaN, 'TRE': np.NaN }


    return (
        {'TRE': np.nanmean(np.sqrt(np.nansum((point_set_0_to_1 - point_sets[1]) ** 2, -1)))},
        {'TRE': np.nanmean(np.sqrt(np.nansum((point_set_1_to_0 - point_sets[0]) ** 2, -1)))}
    )

def hausdorff(superelastix, point_sets, deformation_field_file_names):
    try:
        point_set_0_to_1 = warp_point_set(superelastix, point_sets[0], deformation_field_file_names[0])
        point_set_1_to_0 = warp_point_set(superelastix, point_sets[1], deformation_field_file_names[1])
    except Exception as e:
        logging.error('Failed to compute hausdorff for image pair (%s, %s).' % deformation_field_file_names)
        return { 'Hausdorff': np.NaN, 'Hausdorff': np.NaN }

    return (
        {'Hausdorff': np.nanmax(np.sqrt(np.nansum((point_set_0_to_1 - point_sets[0]) ** 2, -1)))},
        {'Hausdorff': np.nanmax(np.sqrt(np.nansum((point_set_1_to_0 - point_sets[1]) ** 2, -1)))}
    )

def inverse_consistency_points(superelastix, point_sets, deformation_field_file_names):
    try:
        point_set_0_to_1 = warp_point_set(superelastix, point_sets[0], deformation_field_file_names[0])
        point_set_0_to_1_to_0 = warp_point_set(superelastix, point_set_0_to_1, deformation_field_file_names[1])
        point_set_1_to_0 = warp_point_set(superelastix, point_sets[1], deformation_field_file_names[1])
        point_set_1_to_0_to_1 = warp_point_set(superelastix, point_set_1_to_0, deformation_field_file_names[0])
    except Exception as e:
        logging.error('Failed to compute inverse consistency points for image pair (%s, %s).' % deformation_field_file_names)
        return ({'InverseConsistencyTRE': np.NaN, 'InverseConsistencyTRE': np.NaN})

    return (
        {'InverseConsistencyTRE': np.nanmean(np.sqrt(np.nansum((point_set_0_to_1_to_0 - point_sets[0]) ** 2, -1)))},
        {'InverseConsistencyTRE': np.nanmean(np.sqrt(np.nansum((point_set_1_to_0_to_1 - point_sets[1]) ** 2, -1)))}
    )


def inverse_consistency_labels(superelastix, label_file_names, deformation_field_file_names):
    label_image_0_to_1_file_name = warp_image(superelastix, label_file_names[0], deformation_field_file_names[1], 'label')
    label_image_0_to_1_to_0_file_name = warp_image(superelastix, label_image_0_to_1_file_name, deformation_field_file_names[0], 'label')
    label_image_1_to_0_file_name = warp_image(superelastix, label_file_names[1], deformation_field_file_names[0], 'label')
    label_image_1_to_0_to_0_file_name = warp_image(superelastix, label_image_1_to_0_file_name, deformation_field_file_names[1], 'label')

    try:
        label_image_0 = sitk.ReadImage(label_file_names[0])
        labelOverlapMeasurer.Execute(label_image_0, sitk.Cast(sitk.ReadImage(label_image_0_to_1_to_0_file_name), label_image_0.GetPixelID()))
        dsc_0 = labelOverlapMeasurer.GetDiceCoefficient()
    except Exception as e:
        logging.error('Failed to compute inverse consistency DSC for %s' % label_file_names[0])
        return ({'InverseConsistencyDSC': np.NaN }, {'InverseConsistencyDSC': np.NaN })

    try:
        label_image_1 = sitk.ReadImage(label_file_names[1])
        labelOverlapMeasurer.Execute(label_image_1, sitk.Cast(sitk.ReadImage(label_image_1_to_0_to_0_file_name), label_image_1.GetPixelID()))
        dsc_1 = labelOverlapMeasurer.GetDiceCoefficient()
    except Exception as e:
        logging.error('Failed to compute inverse consistency DSC for %s' % label_file_names[1])
        return ({'InverseConsistencyDSC': np.NaN}, {'InverseConsistencyDSC': np.NaN})

    return ({'InverseConsistencyDSC': dsc_0}, {'InverseConsistencyDSC': dsc_1})


def dice(superelastix, label_file_names, deformation_field_file_names):
    label_image_0_to_1_file_name = warp_image(superelastix, label_file_names[0], deformation_field_file_names[1], 'label')

    try:
        label_image_1 = sitk.ReadImage(label_file_names[1])
        labelOverlapMeasurer.Execute(label_image_1, sitk.Cast(sitk.ReadImage(label_image_0_to_1_file_name), label_image_1.GetPixelID()))
        dsc_0 = labelOverlapMeasurer.GetDiceCoefficient()
    except Exception as e:
        logging.error('Failed to compute DSC for %s' % label_file_names[0])
        return ({'DSC': np.NaN}, {'DSC': np.NaN})

    label_image_1_to_0_file_name = warp_image(superelastix, label_file_names[1], deformation_field_file_names[0], 'label')

    try:
        label_image_0 = sitk.ReadImage(label_file_names[0])
        labelOverlapMeasurer.Execute(label_image_0, sitk.Cast(sitk.ReadImage(label_image_1_to_0_file_name), label_image_0.GetPixelID()))
        dsc_1 = labelOverlapMeasurer.GetDiceCoefficient()
    except Exception as e:
        logging.error('Failed to compute DSC for %s' % label_file_names[1])
        return ({'DSC': np.NaN}, {'DSC': np.NaN})

    return (
        {'DSC': dsc_0},
        {'DSC': dsc_1}
    )
