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

#ifndef selxItkImageSourceFixed_h
#define selxItkImageSourceFixed_h

#include "ComponentBase.h"
#include "Interfaces.h"
#include <string.h>
#include "selxMacro.h"
#include "itkImageFileReader.h"
#include "selxAnyFileReader.h"
#include "selxFileReaderDecorator.h"
namespace selx
{
  template<int Dimensionality, class TPixel>
  class ItkImageSourceFixedComponent : 
    public Implements<
    Accepting<>,
    Providing< SourceInterface, itkImageFixedInterface<Dimensionality, TPixel > >
    >
  {
  public:
    selxNewMacro(ItkImageSourceFixedComponent, ComponentBase);

    //itkStaticConstMacro(Dimensionality, unsigned int, Dimensionality);

    ItkImageSourceFixedComponent();
    virtual ~ItkImageSourceFixedComponent();

    typedef typename itkImageFixedInterface<Dimensionality, TPixel >::ItkImageType ItkImageType;
    
    typedef typename itk::ImageFileReader<ItkImageType> ItkImageReaderType;
    typedef FileReaderDecorator<ItkImageReaderType> DecoratedReaderType;

    virtual typename ItkImageType::Pointer GetItkImageFixed() override;
    virtual void SetMiniPipelineInput(itk::DataObject::Pointer) override;
    virtual AnyFileReader::Pointer GetInputFileReader(void) override;

    virtual bool MeetsCriterion(const ComponentBase::CriterionType &criterion) override;
    static const char * GetDescription() { return "ItkImageSourceFixed Component"; };
  private:
    typename ItkImageType::Pointer m_Image;

  protected:
    /* The following struct returns the string name of computation type */
    /* default implementation */

    static inline const std::string GetTypeNameString()
    {
      itkGenericExceptionMacro(<< "Unknown ScalarType" << typeid(TPixel).name());
      // TODO: provide the user instructions how to enable the compilation of the component with the required template types (if desired)
      // We might define an exception object that can communicate various error messages: for simple user, for developer user, etc
    }

    static inline const std::string GetPixelTypeNameString()
    {
      itkGenericExceptionMacro(<< "Unknown PixelType" << typeid(TPixel).name());
      // TODO: provide the user instructions how to enable the compilation of the component with the required template types (if desired)
      // We might define an exception object that can communicate various error messages: for simple user, for developer user, etc
    }

  };



  template <>
  inline const std::string
    ItkImageSourceFixedComponent<2, float>
    ::GetPixelTypeNameString()
  {
    return std::string("float");
  }


  template <>
  inline const std::string
    ItkImageSourceFixedComponent<2, double>
    ::GetPixelTypeNameString()
  {
    return std::string("double");
  }

  template <>
  inline const std::string
    ItkImageSourceFixedComponent<3, float>
    ::GetPixelTypeNameString()
  {
    return std::string("float");
  }

  template <>
  inline const std::string
    ItkImageSourceFixedComponent<3, double>
    ::GetPixelTypeNameString()
  {
    return std::string("double");
  }

} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkImageSourceFixed.hxx"
#endif
#endif // #define selxItkImageSourceFixed_h