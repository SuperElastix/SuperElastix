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
#ifndef itkComponentFactory_hxx
#define itkComponentFactory_hxx

#include "itkComponentFactory.h"

namespace itk
{

ComponentFactory::ComponentFactory()
  {
    this->m_PossibleComponents.clear();
  }
ComponentFactory::~ComponentFactory()
  {
  }



ComponentBase::Pointer ComponentFactory::CreateComponent(const CriteriaType &criteria)
{
  std::list< typename ComponentBase::Pointer > possibleComponents;
//  std::list< LightObject::Pointer >     allobjects =
//    ObjectFactoryBase::CreateAllInstance("itkComponentIOBaseTemplate");
  std::list< LightObject::Pointer >     allobjects =
    ObjectFactoryBase::CreateAllInstance("itkComponentBase");

  for ( std::list< LightObject::Pointer >::iterator i = allobjects.begin();
        i != allobjects.end(); ++i )
    {
    ComponentBase *io =
                        dynamic_cast< ComponentBase * >( i->GetPointer() );
    if ( io )
      {
        possibleComponents.push_back(io);
      }
    }
  for (std::list< typename ComponentBase::Pointer >::iterator k = possibleComponents.begin();
    k != possibleComponents.end(); ++k)
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
