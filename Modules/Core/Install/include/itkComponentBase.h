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
#ifndef itkComponentBase_h
#define itkComponentBase_h

#include "itkLightProcessObject.h"
//#include "itkComponentBase.h"
#include <list>
#include <iostream>
#include <fstream>
#include <string>
#include <map>

namespace itk
{

/** \class ComponentBaseTemplate
 *
 * \brief Abstract superclass defining the Component IO interface.
 *
 * ComponentBaseTemplate is a pure virtual base class for derived classes that
 * read/write Component data considering the type of input Component.
 * First, ComponentBase is derived from this class for legacy read/write Component.
 * This class also is used by the ComponentFileReader and ComponentFileWriter
 * classes. End users don't directly manipulate classes derived from ComponentBaseTemplate;
 * the ComponentFactory is used by the Reader/Writer to pick a concrete derived class to do
 * the actual reading/writing of Components.
 *
 * \ingroup ITKIOComponentBase
 */
class ComponentBase:public LightProcessObject
{
public:
  /** Standard class typedefs */
  typedef ComponentBase   Self;
  typedef LightProcessObject        Superclass;
  typedef SmartPointer< Self >      Pointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(ComponentBase, Superclass);

  /** Component types */
  //typedef TScalar                           ScalarType;
  //typedef ComponentBaseTemplate<ScalarType> ComponentType;
  /** For writing, a const Component list gets passed in, for
   * reading, a non-const Component list is created from the file.
   */
  //typedef typename ComponentType::Pointer             ComponentPointer;
  //typedef std::list< ComponentPointer >               ComponentListType;
  //typedef typename ComponentType::ConstPointer        ConstComponentPointer;
  //typedef std::list< ConstComponentPointer >          ConstComponentListType;

  typedef std::map<std::string, std::string> CriteriaType;
  typedef std::pair<std::string, std::string> CriteriumType;

  /** Set/Get the name of the file to be read. */
  itkSetStringMacro(FileName);
  itkGetStringMacro(FileName);

  /** Reads the data from disk into the memory buffer provided. */
  virtual void Read() = 0;

  /** Writes the Component list to disk. */
  virtual void Write() = 0;

  /** Determine the file type. Returns true if this Component can read the
   * file specified. */
  virtual bool MeetsCriteria(const CriteriaType &criteria) = 0;

protected:
  ComponentBase() {};
  virtual ~ComponentBase() {};
  //virtual void PrintSelf(std::ostream & os, Indent indent) const ITK_OVERRIDE;

  std::string            m_FileName;

};

} // end namespace itk


#endif // itkComponentBase_h
