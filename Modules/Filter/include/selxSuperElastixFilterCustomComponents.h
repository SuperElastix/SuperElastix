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

#ifndef selxSuperElastixFilterCustomComponents_h
#define selxSuperElastixFilterCustomComponents_h

#include "selxSuperElastixFilter.h"

/**
 * \class SuperElastixFilter
 * \brief ITK Filter interface to the SuperElastix registration library.
 */

namespace selx
{
template< typename ComponentList >
class SuperElastixFilterCustomComponents : public SuperElastixFilter
{
public:

  /** Standard ITK typedefs. */
  typedef SuperElastixFilterCustomComponents Self;
  typedef itk::ProcessObject                 Superclass;
  typedef itk::SmartPointer< Self >          Pointer;
  typedef itk::SmartPointer< const Self >    ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( Self, itk::ProcessObject );

protected:

  // default constructor
  SuperElastixFilterCustomComponents( void );

private:
};
} // namespace elx

#ifndef ITK_MANUAL_INSTANTIATION
#include "selxSuperElastixFilterCustomComponents.hxx"
#endif
#endif // selxSuperElastixFilterCustomComponents_h
