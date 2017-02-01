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

#ifndef selxNiftyregf3dComponent_h
#define selxNiftyregf3dComponent_h

#include "selxSuperElastixComponent.h"
#include "selxInterfaces.h"
#include "selxNiftyregInterfaces.h"
#include "_reg_f3d.h"

#include <string.h>
namespace selx
{
template< class TPixel >
class Niftyregf3dComponent :
  public SuperElastixComponent<
  Accepting< NiftyregReferenceImageInterface<TPixel >, NiftyregFloatingImageInterface<TPixel > >,
  Providing< RunRegistrationInterface >
  >
{
public:

  /** Standard ITK typedefs. */
  typedef Niftyregf3dComponent          Self;
  typedef ComponentBase                       Superclass;
  typedef itk::SmartPointer< Self >           Pointer;
  typedef itk::SmartPointer< const Self >     ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( Self, ComponentBase );

  // itkStaticConstMacro(Dimensionality, unsigned int, Dimensionality);

  Niftyregf3dComponent();
  virtual ~Niftyregf3dComponent();


  virtual int Set( NiftyregReferenceImageInterface< TPixel > * ) override;
  virtual int Set( NiftyregFloatingImageInterface< TPixel > * ) override;
  
  virtual void RunRegistration() override;
  
  virtual bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;

  static const char * GetDescription() { return "Niftyregf3d Component"; }

private:

  reg_f3d<TPixel>* m_reg_f3d;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return{ { keys::NameOfClass, "Niftyregf3dComponent" }, { keys::PixelType, PodString< TPixel >::Get() } };
  }
};

} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxNiftyregf3dComponent.hxx"
#endif
#endif // #define selxNiftyregf3dComponent_h
