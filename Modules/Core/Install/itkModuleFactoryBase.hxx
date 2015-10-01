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
#ifndef itkModuleFactoryBase_hxx
#define itkModuleFactoryBase_hxx

#include "itkModuleFactoryBase.h"

namespace itk
{

ModuleIOBase::Pointer ModuleFactoryBase::CreateModuleIO(const CriteriaType &criteria)
{
  std::list< typename ModuleIOBase::Pointer > possibleModuleIO;
//  std::list< LightObject::Pointer >     allobjects =
//    ObjectFactoryBase::CreateAllInstance("itkModuleIOBaseTemplate");
  std::list< LightObject::Pointer >     allobjects =
    ObjectFactoryBase::CreateAllInstance("itkModuleIOBase");

  for ( std::list< LightObject::Pointer >::iterator i = allobjects.begin();
        i != allobjects.end(); ++i )
    {
    ModuleIOBase *io =
                        dynamic_cast< ModuleIOBase * >( i->GetPointer() );
    if ( io )
      {
      possibleModuleIO.push_back(io);
      }
    }
  for ( std::list< typename ModuleIOBase::Pointer >::iterator k = possibleModuleIO.begin();
        k != possibleModuleIO.end(); ++k )
    {
      if ( ( *k )->MeetsCriteria(criteria) )
        {
        return *k;
        }
    }
  return ITK_NULLPTR;
}
} // end namespace itk

#endif
