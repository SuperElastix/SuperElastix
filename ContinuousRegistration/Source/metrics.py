import numpy as np
import SimpleITK as sitk
from ContinuousRegistration.Source.util import warp_image, warp_point_set, compose_displacement_fields
from ContinuousRegistration.Source.util import logging

def tre(superelastix, point_sets, deformation_field_file_names):
    try:
        point_set_fixed0_to_moving1 = warp_point_set(superelastix, point_sets[0], deformation_field_file_names[0])
        point_set_fixed1_to_moving0 = warp_point_set(superelastix, point_sets[1], deformation_field_file_names[1])
    except Exception:
        logging.error('Failed to compute tre for image pair (%s, %s).' % deformation_field_file_names)
        return (
            {'1. TRE': np.NaN},
            {'1. TRE': np.NaN}
        )

    return (
        {'1. TRE': np.nanmean(np.sqrt(np.nansum((point_set_fixed0_to_moving1 - point_sets[1]) ** 2, -1)))},
        {'1. TRE': np.nanmean(np.sqrt(np.nansum((point_set_fixed1_to_moving0 - point_sets[0]) ** 2, -1)))}
    )


def hausdorff(superelastix, point_sets, deformation_field_file_names):
    try:
        point_set_fixed0_to_moving1 = warp_point_set(superelastix, point_sets[0], deformation_field_file_names[0])
        point_set_fixed1_to_moving0 = warp_point_set(superelastix, point_sets[1], deformation_field_file_names[1])
    except Exception:
        logging.error('Failed to compute hausdorff for image pair (%s, %s).' % deformation_field_file_names)
        return (
            {'2. Hausdorff': np.NaN},
            {'2. Hausdorff': np.NaN}
        )

    return (
        {'2. Hausdorff': np.nanmax(np.sqrt(np.nansum((point_set_fixed0_to_moving1 - point_sets[1]) ** 2, -1)))},
        {'2. Hausdorff': np.nanmax(np.sqrt(np.nansum((point_set_fixed1_to_moving0 - point_sets[0]) ** 2, -1)))}
    )


def inverse_consistency(superelastix, displacement_field_file_names, mask_file_names):
    try:
        composed_0 = sitk.GetArrayFromImage(
            sitk.ReadImage(
                compose_displacement_fields(
                    superelastix, displacement_field_file_names[0], displacement_field_file_names[1])))
        composed_1 = sitk.GetArrayFromImage(
            sitk.ReadImage(
                compose_displacement_fields(
                    superelastix, displacement_field_file_names[1], displacement_field_file_names[0])))
    except Exception:
        return (
            {'3. InverseConsistency': np.NaN},
            {'3. InverseConsistency': np.NaN}
        )

    mask_0 = sitk.GetArrayFromImage(sitk.ReadImage(mask_file_names[0])) > 0
    norm_0 = np.linalg.norm(composed_0[mask_0], axis=-1).flatten()

    mask_1 = sitk.GetArrayFromImage(sitk.ReadImage(mask_file_names[1])) > 0
    norm_1 = np.linalg.norm(composed_1[mask_1], axis=-1).flatten()

    return (
        {'3. InverseConsistency': float(np.nanmean(norm_0))},
        {'3. InverseConsistency': float(np.nanmean(norm_1))}
    )


def label_overlap(superelastix, label_file_names, deformation_field_file_names):
    labelOverlapMeasurer = sitk.LabelOverlapMeasuresImageFilter()
    labelOverlapMeasurer.SetGlobalDefaultCoordinateTolerance(1.0)

    label_image_0_to_1_file_name = warp_image(superelastix, label_file_names[0], deformation_field_file_names[1], 'dsc_label_0_to_1')

    try:
        label_image_1 = sitk.ReadImage(label_file_names[1])
        label_image_0_to_1 = sitk.Cast(sitk.ReadImage(label_image_0_to_1_file_name), label_image_1.GetPixelID())
        labelOverlapMeasurer.Execute(label_image_1, label_image_0_to_1)
        dsc_0 = labelOverlapMeasurer.GetDiceCoefficient()
        jaccard_0 = labelOverlapMeasurer.GetJaccardCoefficient()
    except Exception as e:
        logging.error('Failed to compute DSC for %s: %s' % (label_file_names[0], e))
        dsc_0 = jaccard_0 = np.NaN

    label_image_1_to_0_file_name = warp_image(superelastix, label_file_names[1], deformation_field_file_names[0], 'dsc_label_1_to_0')

    try:
        label_image_0 = sitk.ReadImage(label_file_names[0])
        label_image_1_to_0 = sitk.Cast(sitk.ReadImage(label_image_1_to_0_file_name), label_image_0.GetPixelID())
        labelOverlapMeasurer.Execute(label_image_0, label_image_1_to_0)
        dsc_1 = labelOverlapMeasurer.GetDiceCoefficient()
        jaccard_1 = labelOverlapMeasurer.GetJaccardCoefficient()
    except Exception as e:
        logging.error('Failed to compute DSC for %s' % label_file_names[1])
        dsc_1 = jaccard_1 = np.NaN

    return (
        {'1. Dice Similarity Coefficient': dsc_0, '2. Jaccard Coefficient': jaccard_0},
        {'1. Dice Similarity Coefficient': dsc_1, '2. Jaccard Coefficient': jaccard_1}
    )
