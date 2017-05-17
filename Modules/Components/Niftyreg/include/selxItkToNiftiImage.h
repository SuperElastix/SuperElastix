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

  template<class ItkImageType>
  class ItkImageProperties
  {
  public:

    ItkImageProperties();
    ~ItkImageProperties();


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

    template <typename TPixel>
    static const std::tuple<unsigned int, IOPixelType, IOComponentType> GetPixelTypeInfo(const TPixel *)
    {
      return std::make_tuple(1, IOPixelType::SCALAR, MapPixelType<TPixel>::CType);
    }
    template <typename TPixel>
    static const  std::tuple<unsigned int, IOPixelType, IOComponentType> GetPixelTypeInfo(const itk::RGBPixel< TPixel > *)
    {
      return std::make_tuple(3, IOPixelType::RGB, MapPixelType<TPixel>::CType);
    }
    /*

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
    */
    
    static const  unsigned int GetNumberOfComponents()
    {
      return std::get<0>(GetPixelTypeInfo(static_cast<const ItkImageType::PixelType *>(ITK_NULLPTR)));
    }
    
    static const  IOPixelType GetPixelType()
    {
      return std::get<1>(GetPixelTypeInfo(static_cast<const ItkImageType::PixelType *>(ITK_NULLPTR)));
    }
    
    static const IOComponentType GetComponentType()
    {
      return std::get<2>(GetPixelTypeInfo(static_cast<const ItkImageType::PixelType *>(ITK_NULLPTR)));
    }



    /** Set/Get the image origin on a axis-by-axis basis. */
    
    static const double GetOrigin(typename ItkImageType::Pointer input, unsigned int i)
    {
      return input->GetOrigin()[i];
    }

    /** Set/Get the image spacing on an axis-by-axis basis. The
    * SetSpacing() method is required when writing the image. */
    //virtual void SetSpacing(unsigned int i, double spacing);
    
    static const double GetSpacing(typename ItkImageType::Pointer input, unsigned int i)
    {
      return input->GetSpacing()[i];
    }


    /** Set/Get the image direction on an axis-by-axis basis. */
    
    static const std::vector< double > GetDirection(typename ItkImageType::Pointer input, unsigned int i)
    {
      std::vector<double> v(ItkImageType::ImageDimension);
      for (unsigned int c = 0; c < ItkImageType::ImageDimension; c++)
      {
        v[c] = input->GetDirection()[i][c];
      }

      return v;
    }

    static const itk::SizeValueType GetDimensions(typename ItkImageType::Pointer input, unsigned int i)
    {
      ItkImageType::RegionType region = input->GetLargestPossibleRegion();
      ItkImageType::SizeType size = region.GetSize();
      return size[i];
    }



  };
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
void WriteImageInformation(typename ItkImageType::Pointer input, nifti_image* output);

  /** Calculate the region of the image that can be efficiently read
   *  in response to a given requested region. */

protected:

private:
  /** Converts the image data from the memory buffer provided. Make sure
  * that the IORegions has been set properly. */
  const void*  GetImageBuffer(typename ItkImageType::Pointer input);

  void TransferImageData(const void* buffer, nifti_image* output);

  bool  MustRescale();

  void  SetNIfTIOrientationFromImageIO(typename ItkImageType::Pointer input, nifti_image* output, unsigned short int origdims, unsigned short int dims);

  void  SetImageIOOrientationFromNIfTI(unsigned short int dims);

  void  SetImageIOMetadataFromNIfTI();
  
  /** Enums used to manipulate the pixel type. The pixel type provides
  * context for automatic data conversions (for instance, RGB to
  * SCALAR, VECTOR to SCALAR). */
  typedef itk::ImageIOBase::IOPixelType IOPixelType;

  /** Enums used to manipulate the component type. The component type
  * refers to the actual storage class associated with either a
  * SCALAR pixel type or elements of a compound pixel.
  */
  typedef itk::ImageIOBase::IOComponentType IOComponentType;

  double m_RescaleSlope;
  double m_RescaleIntercept;
};



} // end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkToNiftiImage.hxx"
#endif
#endif // selxItkToNiftiImage_h

