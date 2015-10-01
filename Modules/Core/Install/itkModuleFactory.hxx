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
#ifndef itkModuleFactory_hxx
#define itkModuleFactory_hxx

#include "itkModuleFactory.h"

namespace itk
{

ModuleFactory::ModuleFactory()
  {
    this->m_PossibleModules.clear();
  }
ModuleFactory::~ModuleFactory()
  {
  }



ModuleBase::Pointer ModuleFactory::CreateModule(const CriteriaType &criteria)
{
  std::list< typename ModuleBase::Pointer > possibleModules;
//  std::list< LightObject::Pointer >     allobjects =
//    ObjectFactoryBase::CreateAllInstance("itkModuleIOBaseTemplate");
  std::list< LightObject::Pointer >     allobjects =
    ObjectFactoryBase::CreateAllInstance("itkModuleBase");

  for ( std::list< LightObject::Pointer >::iterator i = allobjects.begin();
        i != allobjects.end(); ++i )
    {
    ModuleBase *io =
                        dynamic_cast< ModuleBase * >( i->GetPointer() );
    if ( io )
      {
        possibleModules.push_back(io);
      }
    }
  for (std::list< typename ModuleBase::Pointer >::iterator k = possibleModules.begin();
    k != possibleModules.end(); ++k)
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
