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

#ifndef selxMacro_h
#define selxMacro_h

/**
 * Register class with the object factory and set RTTI (Run-Time Type
 * Information)
 */
#define selxNewMacro( className, superClassName )        \
  typedef className                       Self;         \
  typedef itk::SmartPointer< Self >       Pointer;      \
  typedef itk::SmartPointer< const Self > ConstPointer; \
  itkNewMacro( Self );                                  \
  itkTypeMacro( className, superClassName );            \

#endif // selxMacro_h
