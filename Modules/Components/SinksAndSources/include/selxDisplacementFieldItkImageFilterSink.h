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

#ifndef selxDisplacementFieldItkImageFilterSink_h
#define selxDisplacementFieldItkImageFilterSink_h

#include "ComponentBase.h"
#include "Interfaces.h"
#include <string.h>
#include "selxMacro.h"
#include "itkImageFileWriter.h"
#include "selxAnyFileWriter.h"
#include "selxFileWriterDecorator.h"

namespace selx
{
  template<int Dimensionality, class TPixel>
  class DisplacementFieldItkImageFilterSinkComponent :
    public Implements <
    Accepting< DisplacementFieldItkImageSourceInterface<Dimensionality, TPixel> >,
    Providing < SinkInterface, AfterRegistrationInterface >
    >
  {
  public:
    selxNewMacro(DisplacementFieldItkImageFilterSinkComponent, ComponentBase);

    //itkStaticConstMacro(Dimensionality, unsigned int, Dimensionality);

    DisplacementFieldItkImageFilterSinkComponent();
    virtual ~DisplacementFieldItkImageFilterSinkComponent();

    typedef TPixel PixelType;

    typedef DisplacementFieldItkImageSourceInterface<Dimensionality, TPixel> AcceptingDisplacementFieldInterfaceType;
    typedef typename itk::Image<itk::Vector<PixelType, Dimensionality>, Dimensionality> DeformationFieldImageType;
    typedef typename DeformationFieldImageType::Pointer DeformationFieldImageTypePointer;
    typedef typename itk::ImageFileWriter<DeformationFieldImageType> DeformationFieldImageWriterType;
    typedef typename FileWriterDecorator<DeformationFieldImageWriterType>  DecoratedWriterType;

    virtual int Set(AcceptingDisplacementFieldInterfaceType*) override;
    virtual void SetMiniPipelineOutput(itk::DataObject::Pointer) override;
    virtual itk::DataObject::Pointer GetMiniPipelineOutput(void) override;
    virtual AnyFileWriter::Pointer GetOutputFileWriter(void) override;
    virtual itk::DataObject::Pointer GetInitializedOutput(void) override;

    virtual void AfterRegistration() override;

    virtual bool MeetsCriterion(const ComponentBase::CriterionType &criterion) override;
    static const char * GetDescription() { return "DisplacementFieldItkImageFilterSink Component"; };
  private:
    //itk::ProcessObject::Pointer m_Sink;
    //typename itk::ImageFileWriter<itk::Image<itk::Vector<TPixel,Dimensionality>, Dimensionality>>::Pointer m_SinkWriter;
    typename DeformationFieldImageType::Pointer m_MiniPipelineOutputImage;
    typename DeformationFieldImageType::Pointer m_OverlordOutputImage;
    

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
    DisplacementFieldItkImageFilterSinkComponent<2, float>
    ::GetPixelTypeNameString()
  {
    return std::string("float");
  }


  template <>
  inline const std::string
    DisplacementFieldItkImageFilterSinkComponent<2, double>
    ::GetPixelTypeNameString()
  {
    return std::string("double");
  }

  template <>
  inline const std::string
    DisplacementFieldItkImageFilterSinkComponent<3, float>
    ::GetPixelTypeNameString()
  {
    return std::string("float");
  }

  template <>
  inline const std::string
    DisplacementFieldItkImageFilterSinkComponent<3, double>
    ::GetPixelTypeNameString()
  {
    return std::string("double");
  }

} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxDisplacementFieldItkImageFilterSink.hxx"
#endif
#endif // #define selxDisplacementFieldItkImageFilterSink_h