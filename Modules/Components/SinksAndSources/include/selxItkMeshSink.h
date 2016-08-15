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

#ifndef selxItkMeshSink_h
#define selxItkMeshSink_h

#include "selxSuperElastixComponent.h"
#include "selxInterfaces.h"
#include <string.h>
#include "selxMacro.h"
#include "itkMeshFileWriter.h"
#include "selxAnyFileWriter.h"
#include "selxFileWriterDecorator.h"

namespace selx
{
  template<int Dimensionality, class TPixel>
  class ItkMeshSinkComponent :
    public SuperElastixComponent <
    Accepting< itkMeshInterface<Dimensionality, TPixel> >,
    Providing < SinkInterface, AfterRegistrationInterface >
    >
  {
  public:
    selxNewMacro(ItkMeshSinkComponent, ComponentBase);

    //itkStaticConstMacro(Dimensionality, unsigned int, Dimensionality);

    ItkMeshSinkComponent();
    virtual ~ItkMeshSinkComponent();

    typedef itk::Mesh<TPixel, Dimensionality> ItkMeshType;
    typedef typename itk::MeshFileWriter<ItkMeshType> ItkMeshWriterType;
    typedef FileWriterDecorator<ItkMeshWriterType>  DecoratedWriterType;


    virtual int Set(itkMeshInterface<Dimensionality, TPixel>*) override;

    virtual void SetMiniPipelineOutput(itk::DataObject::Pointer) override;
    virtual itk::DataObject::Pointer GetMiniPipelineOutput(void) override;
    virtual AnyFileWriter::Pointer GetOutputFileWriter(void) override;
    virtual itk::DataObject::Pointer GetInitializedOutput(void) override;

    virtual void AfterRegistration() override;

    virtual bool MeetsCriterion(const ComponentBase::CriterionType &criterion) override;
    static const char * GetDescription() { return "ItkMeshSink Component"; };
  private:
    typename ItkMeshType::Pointer m_MiniPipelineOutputMesh;
    typename ItkMeshType::Pointer m_OverlordOutputMesh;

    itkMeshInterface<Dimensionality, TPixel>* m_ProvidingGetItkMeshInterface;
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
    ItkMeshSinkComponent<2, float>
    ::GetPixelTypeNameString()
  {
    return std::string("float");
  }


  template <>
  inline const std::string
    ItkMeshSinkComponent<2, double>
    ::GetPixelTypeNameString()
  {
    return std::string("double");
  }

  template <>
  inline const std::string
    ItkMeshSinkComponent<3, float>
    ::GetPixelTypeNameString()
  {
    return std::string("float");
  }

  template <>
  inline const std::string
    ItkMeshSinkComponent<3, double>
    ::GetPixelTypeNameString()
  {
    return std::string("double");
  }

} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkMeshSink.hxx"
#endif
#endif // #define selxItkMeshSink_h