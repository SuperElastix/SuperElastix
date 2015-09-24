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
#ifndef itkTransformModule1_hxx
#define itkTransformModule1_hxx

#include "itkTransformModule1.h"

namespace itk
{

template <typename TScalar,
          unsigned int NInputDimensions,
          unsigned int NOutputDimensions>
TransformModule1<TScalar, NInputDimensions, NOutputDimensions>
::TransformModule1()
{
}



template <typename TScalar,
          unsigned int NInputDimensions,
          unsigned int NOutputDimensions>
    std::string TransformModule1<TScalar, NInputDimensions, NOutputDimensions>
::GetModuleTypeAsString() const
{
  std::ostringstream n;

  n << GetNameOfClass();
  n << "_";
  n << this->GetModuleTypeAsString(static_cast<TScalar *>(ITK_NULLPTR) );
  n << "_" << this->GetInputSpaceDimension() << "_" << this->GetOutputSpaceDimension();
  return n.str();
}


template <typename TScalar,
          unsigned int NInputDimensions,
          unsigned int NOutputDimensions>
typename LightObject::Pointer
TransformModule1<TScalar, NInputDimensions, NOutputDimensions>
::InternalClone() const
{
  // Default implementation just copies the parameters from
  // this to new transform.
  typename LightObject::Pointer loPtr = Superclass::InternalClone();

  typename Self::Pointer rval =
    dynamic_cast<Self *>(loPtr.GetPointer());
  if(rval.IsNull())
    {
    itkExceptionMacro(<< "downcast to type "
                      << this->GetNameOfClass()
                      << " failed.");
    }
  //rval->SetFixedParameters(this->GetFixedParameters());
  //rval->SetParameters(this->GetParameters());
  return loPtr;
}



} // end namespace itk

#endif
