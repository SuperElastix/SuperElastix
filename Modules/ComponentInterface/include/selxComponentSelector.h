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

#include "selxTypeList.h"

namespace selx
{
/** \class ComponentSelector
 * \brief A Component factory that accepts criteria, possibly in multiple passes, to construct and return the right Component
 */

template< class ComponentList >
class ComponentSelector
{
public:

  /** Standard class typedefs. */
  typedef ComponentSelector< ComponentList > Self;
  typedef std::shared_ptr< Self >            Pointer;
  typedef std::shared_ptr< const Self >      ConstPointer;

  /** Convenient typedefs. */
  typedef ComponentBase::Pointer               ComponentBasePointer;
  typedef ComponentBase::CriteriaType          CriteriaType;
  typedef ComponentBase::CriterionType         CriterionType;
  typedef ComponentBase::InterfaceCriteriaType InterfaceCriteriaType;

  typedef std::list< ComponentBasePointer > ComponentListType;
  typedef ComponentListType::size_type      NumberOfComponentsType;
  /** set selection criteria for possibleComponents*/

  ComponentSelector( const std::string & name );

  /** Narrow selection criteria*/
  void AddCriterion( const CriterionType & criterion );

  void AddAcceptingInterfaceCriteria( const InterfaceCriteriaType & interfaceCriteria );

  void AddProvidingInterfaceCriteria( const InterfaceCriteriaType & interfaceCriteria );

  unsigned int NumberOfComponents( void );

  unsigned int RequireAcceptingInterfaceFrom( ComponentBasePointer other, const InterfaceCriteriaType & interfaceCriteria );

  unsigned int RequireProvidingInterfaceTo( ComponentBasePointer other, const InterfaceCriteriaType & interfaceCriteria );

  /** Return Component or Nullptr*/
  ComponentBasePointer GetComponent( void );

  void PrintComponents( void );

protected:

  ComponentListType m_PossibleComponents;

private:

  ComponentSelector( const Self & ); //purposely not implemented
  void operator=( const Self & );    //purposely not implemented
};

template< typename >
struct ContructComponentsFromTypeList;
} // end namespace selx

#ifndef ITK_MANUAL_INSTANTIATION
#include "selxComponentSelector.hxx"
#endif

#endif
