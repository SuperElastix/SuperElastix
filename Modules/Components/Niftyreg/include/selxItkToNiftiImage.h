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
#include <nifti1_io.h>
#include "itkMacro.h"

#include "itkImageIOBase.h"
// support exotic PixelTypes
#include "itkRGBPixel.h"
#include "itkRGBAPixel.h"
#include "itkCovariantVector.h"
#include "itkSymmetricSecondRankTensor.h"
#include "itkDiffusionTensor3D.h"
#include "itkVariableLengthVector.h"
  
namespace selx
{
/** \class NiftiImageIO
 *
 * \author Hans J. Johnson, The University of Iowa 2002
 * \brief Class that defines how to read Nifti file format.
 * Nifti IMAGE FILE FORMAT - As much information as I can determine from sourceforge.net/projects/Niftilib
 *
 * The specification for this file format is taken from the
 * web site http://analyzedirect.com/support/10.0Documents/Analyze_Resource_01.pdf
 *
 * \ingroup IOFilters
 * \ingroup ITKIONIFTI
 */
template<class ItkImageType, class NiftiPixelType>
class ItkToNiftiImage
{
public:

ItkToNiftiImage();
~ItkToNiftiImage();

nifti_image * Convert(typename ItkImageType::Pointer input);

typedef ::itk::SizeValueType   SizeValueType;
typedef typename ItkImageType::SpacingType SpacingType;
  //-------- This part of the interfaces deals with writing data. -----

  /** Determine if the file can be written with this ImageIO implementation.
   * \param FileNameToWrite The name of the file to test for writing.
   * \author Hans J. Johnson
   * \post Sets classes ImageIOBase::m_FileName variable to be FileNameToWrite
   * \return Returns true if this ImageIO can write the file specified.
   */


  /** Set the spacing and dimension information for the set filename.
   *
   * For Nifti this does not write a file, it only fills in the
   * appropriate header information.
   */
  virtual void WriteImageInformation();



  /** Calculate the region of the image that can be efficiently read
   *  in response to a given requested region. */

protected:

private:
  /** Converts the image data from the memory buffer provided. Make sure
  * that the IORegions has been set properly. */
  const void*  GetImageBuffer(typename ItkImageType::Pointer input);

  nifti_image* TransferImageData(const void* buffer);

  bool  MustRescale();

  void  SetNIfTIOrientationFromImageIO(unsigned short int origdims, unsigned short int dims);

  void  SetImageIOOrientationFromNIfTI(unsigned short int dims);

  void  SetImageIOMetadataFromNIfTI();

  nifti_image *m_NiftiImage;
  
  typename ItkImageType::Pointer m_ItkImage;

  /** Enums used to manipulate the pixel type. The pixel type provides
  * context for automatic data conversions (for instance, RGB to
  * SCALAR, VECTOR to SCALAR). */
  typedef itk::ImageIOBase::IOPixelType IOPixelType;

  /** Enums used to manipulate the component type. The component type
  * refers to the actual storage class associated with either a
  * SCALAR pixel type or elements of a compound pixel.
  */
  typedef itk::ImageIOBase::IOComponentType IOComponentType;

  template<class TPixel>
  using MapPixelType = itk::ImageIOBase::MapPixelType<TPixel>;

  double m_RescaleSlope;
  double m_RescaleIntercept;
  
  const unsigned int m_NumberOfDimensions;
  unsigned int m_NumberOfComponents;


  /** The array which stores the number of pixels in the x, y, z directions. */
  std::vector< SizeValueType > m_Dimensions;

  /** The array which stores the spacing of pixels in the
  * x, y, z directions. */
  std::vector< double > m_Spacing;

  /** The array which stores the origin of the image. */
  std::vector< double > m_Origin;

  /** The arrays which store the direction cosines of the image. */
  std::vector< std::vector< double > > m_Direction;



  /** Set/Get the type of the pixel. The PixelTypes provides context
  * to the IO mechanisms for data conversions.  PixelTypes can be
  * SCALAR, RGB, RGBA, VECTOR, COVARIANTVECTOR, POINT, INDEX. If
  * the PIXELTYPE is SCALAR, then the NumberOfComponents should be 1.
  * Any other of PIXELTYPE will have more than one component. */
  IOPixelType m_PixelType;
  
  void SetPixelType(const IOPixelType pixelType)
  {
    this->m_PixelType = pixelType;
  }

  const IOPixelType GetPixelType() const
  {
    return this->m_PixelType;
  }

  /** Set/Get the component type of the image. This is always a native
  * type. */
  IOComponentType m_ComponentType;
  void SetComponentType(const IOComponentType componentType)
  {
    this->m_ComponentType = componentType;
  }

  const IOComponentType GetComponentType() const
  {
    return this->m_ComponentType;
  }


  SizeValueType GetDimensions(unsigned int i) const
  {
    ItkImageType::RegionType region = this->m_ItkImage->GetLargestPossibleRegion();
    ItkImageType::SizeType size = region.GetSize();
    return size[i];
  }

  
  template <typename TPixel>
  void SetTypeInfo(const TPixel *);


  template <typename TPixel>
  void SetPixelTypeInfo(const TPixel *)
  {
    this->SetNumberOfComponents(1);
    this->SetPixelType(IOPixelType::SCALAR);
    this->SetComponentType(MapPixelType<TPixel>::CType);
  }
  template <typename TPixel>
  void SetPixelTypeInfo(const itk::RGBPixel< TPixel > *)
  {
    this->SetNumberOfComponents(3);
    this->SetPixelType(IOPixelType::RGB);
    this->SetComponentType(MapPixelType<TPixel>::CType);
  }
  template <typename TPixel>
  void SetPixelTypeInfo(const itk::RGBAPixel< TPixel > *)
  {
    this->SetNumberOfComponents(4);
    this->SetPixelType(IOPixelType::RGBA);
    this->SetComponentType(MapPixelType<TPixel>::CType);
  }
  template <typename TPixel, unsigned VLength>
  void SetPixelTypeInfo(const itk::Vector< TPixel, VLength > *)
  {
    this->SetNumberOfComponents(VLength);
    this->SetPixelType(IOPixelType::VECTOR);
    this->SetComponentType(MapPixelType<TPixel>::CType);
  }
  template <typename TPixel>
  void SetPixelTypeInfo(const itk::VariableLengthVector< TPixel > *)
  {
    this->SetNumberOfComponents(1);
    this->SetPixelType(IOPixelType::VECTOR);
    this->SetComponentType(MapPixelType<TPixel>::CType);
  }
  template <typename TPixel, unsigned VLength>
  void SetPixelTypeInfo(const itk::CovariantVector< TPixel, VLength > *)
  {
    this->SetNumberOfComponents(VLength);
    this->SetPixelType(IOPixelType::COVARIANTVECTOR);
    this->SetComponentType(MapPixelType<TPixel>::CType);
  }
  template <typename TPixel, unsigned VLength>
  void SetPixelTypeInfo(const itk::FixedArray< TPixel, VLength > *)
  {
    this->SetNumberOfComponents(VLength);
    this->SetPixelType(IOPixelType::COVARIANTVECTOR);
    this->SetComponentType(MapPixelType<TPixel>::CType);
  }

  template <typename TPixel, unsigned VLength>
  void SetPixelTypeInfo(const itk::SymmetricSecondRankTensor<TPixel, VLength> *)
  {
    this->SetNumberOfComponents(VLength * (VLength + 1) / 2);
    this->SetPixelType(IOPixelType::SYMMETRICSECONDRANKTENSOR);
    this->SetComponentType(MapPixelType<TPixel>::CType);
  }

  template <typename TPixel>
  inline void SetPixelTypeInfo(const itk::DiffusionTensor3D< TPixel > *)
  {
    this->SetNumberOfComponents(6);
    this->SetPixelType(IOPixelType::DIFFUSIONTENSOR3D);
    this->SetComponentType(MapPixelType<TPixel>::CType);
  }

  template <typename TPixel, unsigned VLength>
  void SetPixelTypeInfo(const itk::Matrix< TPixel, VLength, VLength > *)
  {
    this->SetNumberOfComponents(VLength * VLength);
    this->SetPixelType(IOPixelType::MATRIX);
    this->SetComponentType(MapPixelType<TPixel>::CType);
  }

  template <typename TPixel>
  void SetPixelTypeInfo(const std::complex< TPixel > *)
  {
    this->SetNumberOfComponents(2);
    this->SetPixelType(IOPixelType::COMPLEX);
    this->SetComponentType(MapPixelType<TPixel>::CType);
  }

  template <unsigned VLength>
  void SetPixelTypeInfo(const itk::Offset< VLength > *)
  {
    this->SetNumberOfComponents(VLength);
    this->SetPixelType(IOPixelType::::OFFSET);
    this->SetComponentType(ImageIOBase::LONG);
  }

  /** Set/Get the image origin on a axis-by-axis basis. */

  double GetOrigin(unsigned int i) const
  {
    return this->m_ItkImage->GetOrigin()[i];
  }

  /** Set/Get the image spacing on an axis-by-axis basis. The
  * SetSpacing() method is required when writing the image. */
  //virtual void SetSpacing(unsigned int i, double spacing);

  const double GetSpacing(unsigned int i) const
  {
    return this->m_ItkImage->GetSpacing()[i];
  }


  /** Set/Get the image direction on an axis-by-axis basis. The
  * SetDirection() method is required when writing the image. */
  void SetDirection(const typename ItkImageType::DirectionType & direction);

  virtual std::vector< double > GetDirection(unsigned int i) const
  {
    return m_Direction[i];
  }

  // TODo remove
  void SetNumberOfComponents(unsigned int){};

  const unsigned int GetNumberOfComponents()
  {
    return this->m_ItkImage->GetNumberOfComponentsPerPixel();
  }
};



} // end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkToNiftiImage.hxx"
#endif
#endif // selxItkToNiftiImage_h

