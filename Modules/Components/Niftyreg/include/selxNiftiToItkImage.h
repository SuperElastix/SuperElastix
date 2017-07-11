/*=========================================================================
*
*  Copyright Leiden University Medical Center, Erasmus University Medical
*  Center and contributors
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*        http://www.apache.org/licenses/LICENSE-2.0.txt
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*
*=========================================================================*/
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

#ifndef selxNiftiToItkImage_h
#define selxNiftiToItkImage_h


#include <fstream>
// explicitly include Niftyreg's version of nifti1_io, since it differs from 
// ITK's version and we want to be able to communicate the nifti_image pointer with Niftyreg.
#include "../../Niftyreg/reg-io/nifti/nifti1_io.h" 
#include "itkMacro.h"

#include "itkImageIOBase.h"
#include "selxItkImageProperties.h"

#include "itkMetaDataDictionary.h"

#include <tuple>

namespace selx
{

  struct ImageInformationFromNifti
  {
    double rescaleSlope;
    double rescaleIntercept;
    unsigned int numberOfDimensions;
    unsigned int numberOfComponents;
    itk::ImageIOBase::IOPixelType pixelType;
    itk::ImageIOBase::IOComponentType componentType;
    std::vector<size_t> dimensions;
    std::vector<double> spacing;
  };

  struct OrientationFromNifti
  {
    std::vector<double> origin;
    std::vector<std::vector<double>> direction;
  };

  template<class PixelType>
  struct DataFromNifti
  {
    PixelType * buffer;
    size_t numberOfElements;
  };
/** \class NiftiToItkImage
 * Convert a nifti image to an itk image object.
 * Adapted from itkNiftiImageIO that is originally by Hans J. Johnson, The University of Iowa 2002
 */
template<class ItkImageType, class NiftiPixelType>
class NiftiToItkImage
{
public:

  static typename ItkImageType::Pointer Convert(std::shared_ptr<nifti_image> input );



protected:

private:
  // This is a class with static methods only, so construction is not needed.
  NiftiToItkImage(){};

  
  /** Enums used to manipulate the pixel type. The pixel type provides
  * context for automatic data conversions (for instance, RGB to
  * SCALAR, VECTOR to SCALAR). */
  typedef itk::ImageIOBase::IOPixelType IOPixelType;

  /** Enums used to manipulate the component type. The component type
  * refers to the actual storage class associated with either a
  * SCALAR pixel type or elements of a compound pixel.
  */
  typedef itk::ImageIOBase::IOComponentType IOComponentType;
  /*
  struct ImageBaseProperties
  {
    unsigned int numberOfDimensions;
    unsigned int numberOfComponents;
    IOPixelType pixelType;
    IOComponentType componentType;
  };
  */
  /** Set the spacing and dimension information */
  static ImageInformationFromNifti ReadImageInformation(std::shared_ptr<nifti_image> input);

  /** Reads the data from disk into the memory buffer provided. */
  static DataFromNifti<typename ItkImageType::PixelType> Read(std::shared_ptr<nifti_image> input_image, ImageInformationFromNifti const& imageInformationFromNifti);

  static bool MustRescale(double rescaleSlope, double rescaleIntercept);

  //void  DefineHeaderObjectDataType();

  static OrientationFromNifti GetImageIOOrientationFromNIfTI(unsigned short int dims, std::shared_ptr<nifti_image> input);

  static void SetImageIOMetadataFromNIfTI(std::shared_ptr<nifti_image> input, itk::MetaDataDictionary& dict);

};



} // end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxNiftiToItkImage.hxx"
#endif
#endif // selxNiftiToItkImage_h

