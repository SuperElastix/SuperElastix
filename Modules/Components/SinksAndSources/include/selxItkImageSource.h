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

#ifndef selxItkImageSource_h
#define selxItkImageSource_h

#include "ComponentBase.h"
#include "Interfaces.h"
#include <string.h>
#include "selxMacro.h"
namespace selx
{
  class ItkImageSourceComponent : 
    public Implements<
    Accepting<>,
    Providing< SourceInterface2, itkImageInterface<3,double > >
    >
  {
  public:
    selxNewMacro(ItkImageSourceComponent, ComponentBase);

    ItkImageSourceComponent();
    virtual ~ItkImageSourceComponent();

    typedef itk::ImageSource<itk::Image<double, 3>> ItkImageSourceType;
    typedef itk::Image<double, 3> ItkImageType;
    
    virtual ItkImageType::Pointer GetItkImage() override;
    virtual void SetMiniPipelineInput(itk::DataObject::Pointer) override;
    virtual bool MeetsCriterion(const ComponentBase::CriterionType &criterion) override;
    static const char * GetDescription() { return "ItkImageSource Component"; };
  private:
    ItkImageType::Pointer m_Image;
  };
} //end namespace selx
#endif // #define GDOptimizer3rdPartyComponent_h