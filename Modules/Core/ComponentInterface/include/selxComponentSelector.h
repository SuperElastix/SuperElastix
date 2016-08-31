/*=========================================================================
 *
 *  Copyright Leiden University Medical Center, Erasmus University Medical
 *  Center and contributors
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
#ifndef ComponentSelector_h
#define ComponentSelector_h

#include "itkObjectFactory.h"
#include "selxComponentBase.h"

//#pragma once

namespace selx
{
/** \class ComponentFactor
 * \brief Create instances of MetaImageIO objects using an object factory.
 * \ingroup ITKIOMeta
 */
class ComponentSelector : public itk::Object
{
public:

  /** Standard class typedefs. */
  typedef ComponentSelector               Self;
  typedef Object                          Superclass;
  typedef itk::SmartPointer< Self >       Pointer;
  typedef itk::SmartPointer< const Self > ConstPointer;

  /** Class methods used to interface with the registered factories. */
  //virtual const char * GetITKSourceVersion() const { return ITK_SOURCE_VERSION; }

  /** Run-time type information (and related methods). */
  itkTypeMacro( ComponentSelector, Object );

  /** New macro for creation of through the object factory. */
  itkNewMacro( Self );

  /** Convenient typedefs. */
  typedef ComponentBase::Pointer       ComponentBasePointer;
  typedef ComponentBase::CriteriaType  CriteriaType;
  typedef ComponentBase::CriterionType CriterionType;
  typedef ComponentBase::InterfaceCriteriaType  InterfaceCriteriaType;

  typedef std::list< ComponentBasePointer > ComponentListType;
  typedef ComponentListType::size_type      NumberOfComponentsType;
  /** set selection criteria for possibleComponents*/

  /** Narrow selection criteria*/
  void AddCriterion( const CriterionType & criterion );

  void AddAcceptingInterfaceCriteria(const InterfaceCriteriaType & interfaceCriteria);
  void AddProvidingInterfaceCriteria(const InterfaceCriteriaType & interfaceCriteria);

  unsigned int NumberOfComponents(void);
  /** Check for multiple versus 1 or 0 components*/
  bool HasMultipleComponents( void );

  unsigned int RequireAcceptingInterfaceFrom(ComponentBasePointer other, const InterfaceCriteriaType & interfaceCriteria);
  unsigned int RequireProvidingInterfaceTo(ComponentBasePointer other, const InterfaceCriteriaType & interfaceCriteria);
  
  /** Return Component or Nullptr*/
  ComponentBasePointer GetComponent( void );

protected:

  CriteriaType      m_Criteria;
  ComponentListType m_PossibleComponents;
  ComponentSelector();
  ~ComponentSelector();

private:

  ComponentSelector( const Self & ); //purposely not implemented
  void operator=( const Self & );    //purposely not implemented
};
} // end namespace selx

//#ifndef ITK_MANUAL_INSTANTIATION
//#include "ComponentSelector.hxx"
//#endif

#endif
