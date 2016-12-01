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

#ifndef selxItkAffineTransformComponent_h
#define selxItkAffineTransformComponent_h

#include "selxSuperElastixComponent.h"

#include "selxItkRegistrationMethodv4Interfaces.h"
#include "selxSinksAndSourcesInterfaces.h"
#include "itkAffineTransform.h"

namespace selx
{
template< class InternalComputationValueType, int Dimensionality >
class ItkAffineTransformComponent :
  public SuperElastixComponent<
  Accepting< >,
  Providing< itkTransformInterface< InternalComputationValueType, Dimensionality > >
  >
{
public:

  selxNewMacro( ItkAffineTransformComponent, ComponentBase );

  //itkStaticConstMacro(Dimensionality, unsigned int, Dimensionality);

  ItkAffineTransformComponent();
  virtual ~ItkAffineTransformComponent();

  //typedef double InternalComputationValueType;

  /**  Type of the optimizer. */
  using TransformType    = typename itkTransformInterface< InternalComputationValueType, Dimensionality >::TransformType;
  using TransformPointer = typename itkTransformInterface< InternalComputationValueType, Dimensionality >::TransformPointer;

  typedef typename itk::AffineTransform< InternalComputationValueType, Dimensionality > AffineTransformType;

  virtual TransformPointer GetItkTransform() override;

  virtual bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;

  //static const char * GetName() { return "ItkAffineTransform"; } ;
  static const char * GetDescription() { return "ItkAffineTransform Component"; }

private:

  typename AffineTransformType::Pointer m_Transform;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "ItkAffineTransformComponent" }, { keys::InternalComputationValueType, PodString< InternalComputationValueType >::Get() }, { keys::Dimensionality, std::to_string( Dimensionality ) } };
  }
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkAffineTransform.hxx"
#endif
#endif // #define selxItkAffineTransformComponent_h
