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
#ifndef itkModuleFactory_h
#define itkModuleFactory_h

#include "itkObjectFactory.h"
#include "itkModuleBase.h"

namespace itk
{
/** \class ModuleFactor
 * \brief Create instances of MetaImageIO objects using an object factory.
 * \ingroup ITKIOMeta
 */
class ModuleFactory:public Object
{
public:
  /** Standard class typedefs. */
  typedef ModuleFactory         Self;
  typedef Object          Superclass;
  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;


  /** Class methods used to interface with the registered factories. */
  //virtual const char * GetITKSourceVersion() const { return ITK_SOURCE_VERSION; }

  /** Run-time type information (and related methods). */
  itkTypeMacro(ModuleFactory, Object);
  
  /** Convenient typedefs. */
  typedef ModuleBase::Pointer ModuleBasePointer;
  typedef ModuleBase::CriteriaType CriteriaType;

  typedef std::list< typename ModuleBasePointer > ModuleListType;
  /** set selection criteria for possibleModules*/
  void SetCriteria(const CriteriaType &criteria);
  
  /** Narrow selection criteria*/
  void AddCriteria(const CriteriaType &criteria);

  
  /** Create the appropriate ModuleIO depending on
  *  the particulars of the file.
  */
  static ModuleBasePointer
    CreateModule(const CriteriaType &criteria);

  
protected:
  CriteriaType m_Criteria;
  mutable ModuleListType m_PossibleModules;
  ModuleFactory();
  ~ModuleFactory();

private:
  ModuleFactory(const Self &); //purposely not implemented
  void operator=(const Self &);     //purposely not implemented
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkModuleFactory.hxx"
#endif

#endif
