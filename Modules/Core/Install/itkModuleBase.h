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
#ifndef itkModuleBase_h
#define itkModuleBase_h

#include "itkLightProcessObject.h"
//#include "itkModuleBase.h"
#include <list>
#include <iostream>
#include <fstream>
#include <string>
#include <map>

namespace itk
{

/** \class ModuleBaseTemplate
 *
 * \brief Abstract superclass defining the Module IO interface.
 *
 * ModuleBaseTemplate is a pure virtual base class for derived classes that
 * read/write Module data considering the type of input Module.
 * First, ModuleBase is derived from this class for legacy read/write Module.
 * This class also is used by the ModuleFileReader and ModuleFileWriter
 * classes. End users don't directly manipulate classes derived from ModuleBaseTemplate;
 * the ModuleFactory is used by the Reader/Writer to pick a concrete derived class to do
 * the actual reading/writing of Modules.
 *
 * \ingroup ITKIOModuleBase
 */
class ModuleBase:public LightProcessObject
{
public:
  /** Standard class typedefs */
  typedef ModuleBase   Self;
  typedef LightProcessObject        Superclass;
  typedef SmartPointer< Self >      Pointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(ModuleBase, Superclass);

  /** Module types */
  //typedef TScalar                           ScalarType;
  //typedef ModuleBaseTemplate<ScalarType> ModuleType;
  /** For writing, a const Module list gets passed in, for
   * reading, a non-const Module list is created from the file.
   */
  //typedef typename ModuleType::Pointer             ModulePointer;
  //typedef std::list< ModulePointer >               ModuleListType;
  //typedef typename ModuleType::ConstPointer        ConstModulePointer;
  //typedef std::list< ConstModulePointer >          ConstModuleListType;

  typedef std::map<std::string, std::string> CriteriaType;
  typedef std::pair<std::string, std::string> CriteriumType;

  /** Set/Get the name of the file to be read. */
  itkSetStringMacro(FileName);
  itkGetStringMacro(FileName);

  /** Reads the data from disk into the memory buffer provided. */
  virtual void Read() = 0;

  /** Writes the Module list to disk. */
  virtual void Write() = 0;

  /** Determine the file type. Returns true if this Module can read the
   * file specified. */
  virtual bool MeetsCriteria(const CriteriaType &criteria) = 0;

protected:
  ModuleBase() {};
  virtual ~ModuleBase() {};
  //virtual void PrintSelf(std::ostream & os, Indent indent) const ITK_OVERRIDE;

  std::string            m_FileName;

};

} // end namespace itk


#endif // itkModuleBase_h
