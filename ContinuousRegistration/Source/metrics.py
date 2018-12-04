import numpy as np
import SimpleITK as sitk
from ContinuousRegistration.Source.util import warp_image, warp_point_set
from ContinuousRegistration.Source.util import logging

def tre(superelastix, point_sets, deformation_field_file_names):
    try:
        point_set_fixed0_to_moving1 = warp_point_set(superelastix, point_sets[0], deformation_field_file_names[0])
        point_set_fixed1_to_moving0 = warp_point_set(superelastix, point_sets[1], deformation_field_file_names[1])
    except Exception as e:
        logging.error('Failed to compute tre for image pair (%s, %s).' % deformation_field_file_names)
        return (
            {
                '1. TRE': np.NaN
            },
            {
                '1. TRE': np.NaN
            }
        )

    return (
        {
            '1. TRE': np.nanmean(np.sqrt(np.nansum((point_set_fixed0_to_moving1 - point_sets[1]) ** 2, -1)))
         },
        {
            '1. TRE': np.nanmean(np.sqrt(np.nansum((point_set_fixed1_to_moving0 - point_sets[0]) ** 2, -1)))
         }
    )

def hausdorff(superelastix, point_sets, deformation_field_file_names):
    try:
        point_set_fixed0_to_moving1 = warp_point_set(superelastix, point_sets[0], deformation_field_file_names[0])
        point_set_fixed1_to_moving0 = warp_point_set(superelastix, point_sets[1], deformation_field_file_names[1])
    except Exception as e:
        logging.error('Failed to compute hausdorff for image pair (%s, %s).' % deformation_field_file_names)
        return (
            {
                '2. Hausdorff': np.NaN
            },
            {
                '2. Hausdorff': np.NaN
            }
        )

    return (
        {
            '2. Hausdorff': np.nanmax(np.sqrt(np.nansum((point_set_fixed0_to_moving1 - point_sets[1]) ** 2, -1)))
        },
        {
            '2. Hausdorff': np.nanmax(np.sqrt(np.nansum((point_set_fixed1_to_moving0 - point_sets[0]) ** 2, -1)))
        }
    )

def inverse_consistency_points(superelastix, point_sets, deformation_field_file_names):
    try:
        point_set_fixed0_to_moving1 = warp_point_set(superelastix, point_sets[0], deformation_field_file_names[0])
        point_set_fixed0_to_moving1_to_fixed0 = warp_point_set(superelastix, point_set_fixed0_to_moving1, deformation_field_file_names[1])
        point_set_fixed1_to_moving0 = warp_point_set(superelastix, point_sets[1], deformation_field_file_names[1])
        point_set_fixed1_to_moving0_to_fixed1 = warp_point_set(superelastix, point_set_fixed1_to_moving0, deformation_field_file_names[0])
    except Exception as e:
        logging.error('Failed to compute inverse consistency points for image pair (%s, %s).' % deformation_field_file_names)
        return ({'3. InverseConsistencyTRE': np.NaN }, { '3. InverseConsistencyTRE': np.NaN })

    return (
        {
            '3. InverseConsistencyTRE': np.nanmean(np.sqrt(np.nansum((point_set_fixed0_to_moving1_to_fixed0 - point_sets[0]) ** 2, -1)))
        },
        {
            '3. InverseConsistencyTRE': np.nanmean(np.sqrt(np.nansum((point_set_fixed1_to_moving0_to_fixed1 - point_sets[1]) ** 2, -1)))
        }
    )


def inverse_consistency_labels(superelastix, label_file_names, deformation_field_file_names):
    labelOverlapMeasurer = sitk.LabelOverlapMeasuresImageFilter()
    labelOverlapMeasurer.SetGlobalDefaultCoordinateTolerance(1.0)

    label_image_moving0_to_fixed1_file_name = warp_image(superelastix, label_file_names[0], deformation_field_file_names[1], 'inverse_consistency_label_0_to_1')
    label_image_moving0_to_fixed1_to_moving0_file_name = warp_image(superelastix, label_image_moving0_to_fixed1_file_name, deformation_field_file_names[0], 'inverse_consistency_label_0_to_1_to_0')
    label_image_moving1_to_fixed0_file_name = warp_image(superelastix, label_file_names[1], deformation_field_file_names[0], 'inverse_consistency_label_1_to_0')
    label_image_moving1_to_fixed0_to_moving1_file_name = warp_image(superelastix, label_image_moving1_to_fixed0_file_name, deformation_field_file_names[1], 'inverse_consistency_label_1_to_0_to_1')

    try:
        label_image_0 = sitk.ReadImage(label_file_names[0])
        labelOverlapMeasurer.Execute(label_image_0, sitk.Cast(sitk.ReadImage(label_image_moving0_to_fixed1_to_moving0_file_name), label_image_0.GetPixelID()))
        dsc_0 = labelOverlapMeasurer.GetDiceCoefficient()
    except Exception as e:
        logging.error('Failed to compute inverse consistency DSC for %s' % label_file_names[0])
        dsc_0 = np.NaN

    try:
        label_image_1 = sitk.ReadImage(label_file_names[1])
        labelOverlapMeasurer.Execute(label_image_1, sitk.Cast(sitk.ReadImage(label_image_moving1_to_fixed0_to_moving1_file_name), label_image_1.GetPixelID()))
        dsc_1 = labelOverlapMeasurer.GetDiceCoefficient()
    except Exception as e:
        logging.error('Failed to compute inverse consistency DSC for %s' % label_file_names[1])
        dsc_1 = np.NaN

    return (
        {
            '2. InverseConsistencyDSC': dsc_0
        },
        {
            '2. InverseConsistencyDSC': dsc_1
         }
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
        fne_0 = labelOverlapMeasurer.GetFalseNegativeError()
        fpe_0 = labelOverlapMeasurer.GetFalsePositiveError()
        jaccard_0 = labelOverlapMeasurer.GetJaccardCoefficient()
        union_0 = labelOverlapMeasurer.GetUnionOverlap()
        vol_0 = labelOverlapMeasurer.GetVolumeSimilarity()
    except Exception as e:
        logging.error('Failed to compute DSC for %s: %s' % (label_file_names[0], e))
        dsc_0 = fne_0 = fpe_0 = jaccard_0 = union_0 = vol_0 = np.NaN

    label_image_1_to_0_file_name = warp_image(superelastix, label_file_names[1], deformation_field_file_names[0], 'dsc_label_1_to_0')

    try:
        label_image_0 = sitk.ReadImage(label_file_names[0])
        label_image_1_to_0 = sitk.Cast(sitk.ReadImage(label_image_1_to_0_file_name), label_image_0.GetPixelID())
        labelOverlapMeasurer.Execute(label_image_0, label_image_1_to_0)
        dsc_1 = labelOverlapMeasurer.GetDiceCoefficient()
        fne_1 = labelOverlapMeasurer.GetFalseNegativeError()
        fpe_1 = labelOverlapMeasurer.GetFalsePositiveError()
        jaccard_1 = labelOverlapMeasurer.GetJaccardCoefficient()
        union_1 = labelOverlapMeasurer.GetUnionOverlap()
        vol_1 = labelOverlapMeasurer.GetVolumeSimilarity()
    except Exception as e:
        logging.error('Failed to compute DSC for %s' % label_file_names[1])
        dsc_1 = fne_1 = fpe_1 = jaccard_1 = union_1 = vol_1 = np.NaN

    return (
        {
            '1. Dice Similarity Coefficient': dsc_0,
            '3. Jaccard Coefficient': jaccard_0,
            '4. Union Coefficient': union_0,
            '5. VolumeSimilarity': vol_0,
            '6. FalseNegativeError': fne_0,
            '7. FalsePositiveError': fpe_0
        },
        {
            '1. Dice Similarity Coefficient': dsc_1,
            '3. Jaccard Coefficient': jaccard_1,
            '4. Union Coefficient': union_1,
            '5. VolumeSimilarity': vol_1,
            '6. FalseNegativeError': fne_1,
            '7. FalsePositiveError': fpe_1
        }
    )
