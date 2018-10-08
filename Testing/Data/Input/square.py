import SimpleITK as sitk
import numpy as np

def square2d(w, h, c, r):
    array = np.zeros((w, h))
    for i in range(c[0] - r[0], c[0] + r[0]):
        for j in range(c[1] - r[1], c[1] + r[1]):
            x = 1 if i < c[0] else 2
            y = 1 if j < c[1] else 2
            array[i, j] = x + y

    image = sitk.GetImageFromArray(array)
    image = sitk.SmoothingRecursiveGaussian(image, 2.)
    return image

def square3d(w, h, d, c, r):
    array = np.zeros((w, h, d))
    for i in range(c[0] - r[0], c[0] + r[0]):
        for j in range(c[1] - r[1], c[1] + r[1]):
            for k in range(c[2] - r[2], c[2] + r[2]):
                x = 1 if i < c[0] else 2
                y = 1 if j < c[1] else 2
                z = 1 if k < c[2] else 2
                array[i, j, k] = x + y + z

    image = sitk.GetImageFromArray(array)
    image = sitk.SmoothingRecursiveGaussian(image, 2.)
    return image


if __name__ == '__main__':
    # 2D
    w = h = 128

    # Square
    c = [w // 2, h // 2]
    r = [w // 4, h // 4]
    image = square2d(w, h, c, r)
    sitk.WriteImage(image, './square2dIsotropic.nii.gz')

    # Rectangle
    r = [w // 6, h // 4]
    image = square2d(w, h, c, r)
    sitk.WriteImage(image, './square2dAnisotropic.nii.gz')
    image.SetOrigin((8, 12))
    sitk.WriteImage(image, './square2dAnisotropicOriginX8OriginY12.nii.gz')

    # Rectangle + translation
    c = [w // 2 + 8, h // 2 + 16]
    r = [w // 6, h // 4]
    image = square2d(w, h, c, r)
    sitk.WriteImage(image, './square2dAnisotropicX8Y16.nii.gz')
    image.SetOrigin((8, 12))
    sitk.WriteImage(image, './square2dAnisotropicX8Y16OriginX8OriginY12.nii.gz')

    # 3D
    w = h = d = 128

    # Square
    c = [w // 2, h // 2, d // 2]
    r = [w // 4, h // 4, d // 4]
    image = square3d(w, h, d, c, r)
    sitk.WriteImage(image, './square3dIsotropic.nii.gz')
    image.SetOrigin((8, 12, 16))
    sitk.WriteImage(image, './square3dIsotropicOriginX8OriginY12OriginZ16.nii.gz')
    image.SetOrigin((0, 0, 0))
    image.SetSpacing((2, 2, 2))
    sitk.WriteImage(image, './square3dIsotropicSpacingX2SpacingY2SpacingZ2.nii.gz')
    image.SetSpacing((1.1, 1.1, 1.1))
    sitk.WriteImage(image, './square3dIsotropicSpacingX1.1SpacingY1.1SpacingZ1.1.nii.gz')

    # Rectangle
    r = [w // 4, h // 4, d // 6]
    image = square3d(w, h, d, c, r)
    sitk.WriteImage(image, './square3dAnisotropic.nii.gz')
    image.SetOrigin((8, 12, 16))
    sitk.WriteImage(image, './square3dAnisotropicOriginX8OriginY12OriginZ16.nii.gz')

    # Rectangle + translation
    c = [w // 2 + 8, h // 2 + 16, d // 2 + 24]
    r = [w // 4, h // 4, d // 6]
    image = square3d(w, h, d, c, r)
    sitk.WriteImage(image, './square3dAnisotropicX8Y16Z24.nii.gz')
    image.SetOrigin((8, 12, 16))
    sitk.WriteImage(image, './square3dAnisotropicX8Y16Z24OriginX8OriginY12OriginZ16.nii.gz')

