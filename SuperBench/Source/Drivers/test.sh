#!/bin/bash

# Write identity deformation field in fixed image space
python - <<END
print("Generating identity deformation field $4 ...")
import SimpleITK as sitk
fixed_image = sitk.ReadImage("$2")
deformation_field = sitk.Image(fixed_image.GetSize(), sitk.sitkVectorFloat32, 3)
deformation_field.CopyInformation(fixed_image)
sitk.WriteImage(deformation_field, "$4")
END
