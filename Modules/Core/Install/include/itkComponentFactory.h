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
#ifndef itkComponentFactory_h
#define itkComponentFactory_h

#include "itkObjectFactory.h"
#include "itkComponentBase.h"

namespace itk
{
/** \class ComponentFactor
 * \brief Create instances of MetaImageIO objects using an object factory.
 * \ingroup ITKIOMeta
 */
class ComponentFactory:public Object
{
public:
  /** Standard class typedefs. */
  typedef ComponentFactory         Self;
  typedef Object          Superclass;
  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;


  /** Class methods used to interface with the registered factories. */
  //virtual const char * GetITKSourceVersion() const { return ITK_SOURCE_VERSION; }

  /** Run-time type information (and related methods). */
  itkTypeMacro(ComponentFactory, Object);

  /** New macro for creation of through the object factory. */
  itkNewMacro(Self);
  
  /** Convenient typedefs. */
  typedef ComponentBase::Pointer ComponentBasePointer;
  typedef ComponentBase::CriteriaType CriteriaType;

  typedef std::list< typename ComponentBasePointer > ComponentListType;
  /** set selection criteria for possibleComponents*/
  void Initialize();
  void SetCriteria(const CriteriaType &criteria);
  
  /** Narrow selection criteria*/
  void AddCriteria(const CriteriaType &criteria);
  
  void UpdatePossibleComponents(void);
  
  /** Create the appropriate ComponentIO depending on
  *  the particulars of the file.
  */
  ComponentBasePointer GetComponent(void);
  
  
protected:
  mutable CriteriaType m_Criteria;
  mutable ComponentListType m_PossibleComponents;
  ComponentFactory();
  ~ComponentFactory();

private:
  ComponentFactory(const Self &); //purposely not implemented
  void operator=(const Self &);     //purposely not implemented
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkComponentFactory.hxx"
#endif

#endif
