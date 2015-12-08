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
#ifndef ComponentFactory_h
#define ComponentFactory_h

#include "ComponentSelector.h"
#include "ComponentBase.h"

namespace selx
{
/** \class MetricComponent1Factory
 * \brief Create instances of MetaImageIO objects using an object factory.
 * \ingroup ITKIOMeta
 */
  template <class ComponentT>
  class ComponentFactory : public itk::ObjectFactoryBase
{
public:
  /** Standard class typedefs. */
  typedef ComponentFactory         Self;
  typedef itk::ObjectFactoryBase          Superclass;
  typedef itk::SmartPointer< Self >       Pointer;
  typedef itk::SmartPointer< const Self > ConstPointer;

  /** Class methods used to interface with the registered factories. */
  virtual const char * GetITKSourceVersion() const ITK_OVERRIDE;

  virtual const char * GetDescription() const ITK_OVERRIDE;

  /** Method for class instantiation. */
  itkFactorylessNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ComponentFactory, ObjectFactoryBase);

  /** Register one factory of this type  */
  static void RegisterOneFactory()
  {
    Pointer thisFactory = Self::New();

    ObjectFactoryBase::RegisterFactory(thisFactory);
  }

protected:
  ComponentFactory();
  ~ComponentFactory() {};

private:
  ComponentFactory(const Self &); //purposely not implemented
  void operator=(const Self &);     //purposely not implemented
};
} // end namespace selx


#ifndef ITK_MANUAL_INSTANTIATION
#include "ComponentFactory.hxx"
#endif
#endif
