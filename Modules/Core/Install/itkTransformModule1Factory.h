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
#ifndef itkTransformModule1Factory_h
#define itkTransformModule1Factory_h

#include "itkModuleFactoryBase.h"
#include "itkModuleIOBase.h"

namespace itk
{
/** \class TransformModule1Factory
 * \brief Create instances of MetaImageIO objects using an object factory.
 * \ingroup ITKIOMeta
 */
class TransformModule1Factory:public ModuleFactoryBase
{
public:
  /** Standard class typedefs. */
  typedef TransformModule1Factory         Self;
  typedef ModuleFactoryBase          Superclass;
  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  /** Class methods used to interface with the registered factories. */
  virtual const char * GetITKSourceVersion() const ITK_OVERRIDE;

  virtual const char * GetDescription() const ITK_OVERRIDE;

  /** Method for class instantiation. */
  itkFactorylessNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(TransformModule1Factory, ModuleFactoryBase);

  /** Register one factory of this type  */
  static void RegisterOneFactory()
  {
    TransformModule1Factory::Pointer transformModule1Factory = TransformModule1Factory::New();

    ModuleFactoryBase::RegisterFactory(transformModule1Factory);
  }

protected:
  TransformModule1Factory();
  ~TransformModule1Factory(){};

private:
  TransformModule1Factory(const Self &); //purposely not implemented
  void operator=(const Self &);     //purposely not implemented
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkTransformModule1Factory.hxx"
#endif

#endif
