/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#ifndef selxItkToNiftiImage_h
#define selxItkToNiftiImage_h


#include <fstream>
// explicitly include Niftyreg's version of nifti1_io, since it differs from 
// ITK's version and we want to be able to communicate the nifti_image pointer with Niftyreg.
#include "../../Niftyreg/reg-io/nifti/nifti1_io.h" 
#include "itkMacro.h"

#include "itkImageIOBase.h"
#include "selxItkImageProperties.h"

namespace selx
{


/** \class ItkToNiftiImage
 * Convert an itk image to an nifti image object.
 * Adapted from itkNiftiImageIO that is originally by Hans J. Johnson, The University of Iowa 2002
 */
template<class ItkImageType, class NiftiPixelType>
class ItkToNiftiImage
{
public:

   static std::shared_ptr<nifti_image> Convert(typename ItkImageType::Pointer input);

protected:

private:
  // This is a class with static methods only, so construction is not needed.
  ItkToNiftiImage(){};


  /** Set the spacing and dimension information.
  *
  * For Nifti this does not write a file, it only fills in the
  * appropriate header information.
  */
  static void SetHeaderInformation(typename ItkImageType::Pointer input, nifti_image* output);

  /** Converts the image data from the memory buffer provided. Make sure
  * that the IORegions has been set properly. */
  //static const void*  GetImageBuffer(typename ItkImageType::Pointer input);

  static bool TransferImageData(typename ItkImageType::PixelType* buffer, nifti_image* output);

  static void  SetNIfTIOrientationFromImageIO(typename ItkImageType::Pointer input, nifti_image* output, unsigned short int origdims, unsigned short int dims);

  
  /** Enums used to manipulate the pixel type. The pixel type provides
  * context for automatic data conversions (for instance, RGB to
  * SCALAR, VECTOR to SCALAR). */
  typedef itk::ImageIOBase::IOPixelType IOPixelType;

  /** Enums used to manipulate the component type. The component type
  * refers to the actual storage class associated with either a
  * SCALAR pixel type or elements of a compound pixel.
  */
  typedef itk::ImageIOBase::IOComponentType IOComponentType;

};

} // end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkToNiftiImage.hxx"
#endif
#endif // selxItkToNiftiImage_h

