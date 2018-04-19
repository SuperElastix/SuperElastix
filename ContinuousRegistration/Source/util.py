from itertools import islice
import os
import SimpleITK as sitk
import numpy as np

def take(iterable, n):
    "Return first n items of the iterable as a list"
    return list(islice(iterable, n))


def sort_file_names(file_names):
    return sorted(file_names, key=lambda dictionary: dictionary['image_file_names'][0])


# Some data sets come without world coordinate information for the label images. This is a problem
# when measuring overlap. Here, we write a new header file for the labels with the correct
# information.
copy_information_from_images_to_labels_ext = '.mhd'
def copy_information_from_images_to_labels(image_file_names, label_file_names, displacement_field_file_names, output_directory, mhd_pixel_type):
        new_label_file_names = []
        for image_file_name, label_file_name, displacement_field_file_name in zip(image_file_names, label_file_names, displacement_field_file_names):
            label_file_name_we, label_file_name_ext = os.path.splitext(label_file_name)
            dataset_output_directory = os.path.join(output_directory, 'tmp', 'labels_with_world_info', os.path.dirname(displacement_field_file_name))
            output_file_name = os.path.join(dataset_output_directory, os.path.basename(label_file_name_we) + '_label_with_world_info' + copy_information_from_images_to_labels_ext)

            if not os.path.isdir(dataset_output_directory):
                os.makedirs(dataset_output_directory)

            # File info is read from corresponding image file
            image = sitk.ReadImage(image_file_name)

            if not os.path.isfile(output_file_name):
                with open(output_file_name, 'w') as mhd:
                    mhd.write('ObjectType = Image\n')
                    mhd.write('NDims = 3\n')
                    mhd.write('BinaryData = True\n')
                    mhd.write('DimSize = %d %d %d\n' % image.GetSize())
                    mhd.write('Offset = %f %f %f\n' % image.GetOrigin())
                    mhd.write('ElementSpacing = %f %f %f\n' % image.GetSpacing())
                    mhd.write('Orientation = %f %f %f %f %f %f %f %f %f\n' % image.GetDirection())
                    mhd.write('ElementType = %s\n' % mhd_pixel_type)
                    mhd.write('ElementDataFile = %s\n' % (label_file_name_we + '.img'))

                print('Created label with world information %s.' % output_file_name)

            new_label_file_names.append(output_file_name)


        return tuple(new_label_file_names)

mask_ext = '.nii.gz'
def create_mask_by_thresholding(label_file_names, displacement_field_file_names, output_directory, threshold, dilate, erode):
    mask_file_names = []
    for label_file_name, displacement_field_file_name in zip(label_file_names, displacement_field_file_names):
        label_file_name_we, label_file_name_ext = os.path.splitext(label_file_name)
        dataset_output_directory = os.path.join(output_directory, 'tmp', 'masks', os.path.dirname(displacement_field_file_name))
        output_file_name = os.path.join(dataset_output_directory, os.path.basename(
            label_file_name_we) + '_mask' + mask_ext)

        if not os.path.isdir(dataset_output_directory):
            os.makedirs(dataset_output_directory)

        if not os.path.isfile(output_file_name):
            label = sitk.ReadImage(label_file_name)
            mask = label > threshold
            padding = (erode,)*mask.GetDimension()
            padded_mask = sitk.ConstantPad(mask, padding, padding)
            dilated_mask = sitk.BinaryDilate(padded_mask, dilate, sitk.sitkAnnulus, 0, 1, False) # pixels
            filled_mask = sitk.BinaryErode(dilated_mask, erode, sitk.sitkAnnulus, 0, 1, False)
            cropped_filled_mask = sitk.Crop(filled_mask, padding, padding)
            sitk.WriteImage(cropped_filled_mask, output_file_name)
            print('Created mask %s.' % output_file_name)

        mask_file_names.append(output_file_name)


    return tuple(mask_file_names)

def create_mask_by_size(image_file_name, mask_file_name):
    mask_directory = os.path.dirname(mask_file_name)

    if not mask_file_name.endswith(mask_ext):
        mask_file_name = os.path.splitext(mask_file_name)[0] + mask_ext

    if mask_directory is not None:
        os.makedirs(mask_directory, exist_ok=True)

    if not os.path.exists(mask_file_name):
        image = sitk.ReadImage(image_file_name)

        siz = image.GetSize()
        siz = siz[len(siz)-1:] + siz[:len(siz)-1]  # left-shift size
        mask = sitk.GetImageFromArray(np.ones(siz))
        mask.CopyInformation(image)

        os.makedirs(os.path.dirname(mask_file_name), exist_ok=True)
        sitk.WriteImage(sitk.Cast(mask, sitk.sitkUInt8), mask_file_name)

    return mask_file_name
