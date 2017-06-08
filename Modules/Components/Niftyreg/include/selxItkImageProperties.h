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

#ifndef selxItkImageProperties_h
#define selxItkImageProperties_h

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

    template <typename TPixel>
    static const  std::tuple<unsigned int, IOPixelType, IOComponentType> GetPixelTypeInfo(const itk::RGBAPixel< TPixel > *)
    {
      return std::make_tuple(4, IOPixelType::RGBA, MapPixelType<TPixel>::CType);
    }

    template <typename TPixel, unsigned VLength>
    static const  std::tuple<unsigned int, IOPixelType, IOComponentType> GetPixelTypeInfo(const itk::Vector< TPixel, VLength > *)
    {
      return std::make_tuple(VLength, IOPixelType::VECTOR, MapPixelType<TPixel>::CType);
    }

    template <typename TPixel>
    static const  std::tuple<unsigned int, IOPixelType, IOComponentType> GetPixelTypeInfo(const itk::VariableLengthVector< TPixel > *)
    {
      return std::make_tuple(1, IOPixelType::VECTOR, MapPixelType<TPixel>::CType);
    }

    template <typename TPixel, unsigned VLength>
    static const  std::tuple<unsigned int, IOPixelType, IOComponentType> GetPixelTypeInfo(const itk::CovariantVector< TPixel, VLength > *)
    {
      return std::make_tuple(VLength, IOPixelType::COVARIANTVECTOR, MapPixelType<TPixel>::CType);
    }

    template <typename TPixel, unsigned VLength>
    static const  std::tuple<unsigned int, IOPixelType, IOComponentType> GetPixelTypeInfo(const itk::FixedArray< TPixel, VLength > *)
    {
      return std::make_tuple(VLength, IOPixelType::COVARIANTVECTOR, MapPixelType<TPixel>::CType);
    }

    template <typename TPixel, unsigned VLength>
    static const  std::tuple<unsigned int, IOPixelType, IOComponentType> GetPixelTypeInfo(const itk::SymmetricSecondRankTensor< TPixel, VLength > *)
    {
      return std::make_tuple(VLength * (VLength + 1) / 2, IOPixelType::SYMMETRICSECONDRANKTENSOR, MapPixelType<TPixel>::CType);
    }

    template <typename TPixel>
    static const  std::tuple<unsigned int, IOPixelType, IOComponentType> GetPixelTypeInfo(const itk::DiffusionTensor3D< TPixel > *)
    {
      return std::make_tuple(6, IOPixelType::DIFFUSIONTENSOR3D, MapPixelType<TPixel>::CType);
    }

    template <typename TPixel, unsigned VLength>
    static const  std::tuple<unsigned int, IOPixelType, IOComponentType> GetPixelTypeInfo(const itk::Matrix< TPixel, VLength, VLength > *)
    {
      return std::make_tuple(VLength * VLength, IOPixelType::MATRIX, MapPixelType<TPixel>::CType);
    }

    template <typename TPixel>
    static const  std::tuple<unsigned int, IOPixelType, IOComponentType> GetPixelTypeInfo(const std::complex< TPixel > *)
    {
      return std::make_tuple(2, IOPixelType::COMPLEX, MapPixelType<TPixel>::CType);
    }

    template <unsigned VLength>
    static const  std::tuple<unsigned int, IOPixelType, IOComponentType> GetPixelTypeInfo(const itk::Offset< VLength > *)
    {
      return std::make_tuple(VLength, IOPixelType::OFFSET, ImageIOBase::LONG);
    }
    
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

    private:
      // This is a class with static methods only, so construction is not needed.
      ItkImageProperties(){};



  };
} // end namespace selx

  #endif // selxItkImageProperties_h