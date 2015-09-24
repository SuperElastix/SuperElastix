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
#ifndef itkModuleBase_h
#define itkModuleBase_h

#include "itkObject.h"
#include "itkObjectFactory.h"
#include "itkIntTypes.h"

namespace itk
{
  /** \class itkModuleBaseTemplate
   *
   * This class is an abstract class to represent a spatial Module.
   *
   * This class is templated over the scalar type used to store the Module's
   * parameters.
   *
   * \ingroup ITKModule
   */
  template< typename TScalar >
  class ModuleBaseTemplate :public Object
  {
  public:
    /** Standard class typedefs. */
    typedef ModuleBaseTemplate      Self;
    typedef Object                     Superclass;
    typedef SmartPointer< Self >       Pointer;
    typedef SmartPointer< const Self > ConstPointer;

    /** Type of the input parameters. */
    typedef  TScalar                                    ParametersValueType;
    //typedef  OptimizerParameters< ParametersValueType > ParametersType;

    /** Run-time type information (and related methods). */
    itkTypeMacro(ModuleBaseTemplate, Object);

    /** Generate a platform independent name */
    virtual std::string GetModuleTypeAsString() const = 0;
    /*
      typedef enum {
      UnknownModuleCategory=0,
      Linear=1,
      BSpline=2,
      Spline=3,
      DisplacementField=4,
      VelocityField=5
      } ModuleCategoryType;
      */
    /** Get Module category */
    //virtual ModuleCategoryType GetModuleCategory() const = 0;

  protected:
    ModuleBaseTemplate(){}
    virtual ~ModuleBaseTemplate() {}

  private:
    ModuleBaseTemplate(const Self &);  //purposely not implemented
    void operator=(const Self &); //purposely not implemented
  };

} // end namespace itk

#endif
