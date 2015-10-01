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
//#include "itkModuleFactoryRegisterManager.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkDisplacementFieldTransform.h"
#include "itkComposeDisplacementFieldsImageFilter.h"
#include "itkCompositeTransform.h"
#include "itkVector.h"
#include "itkPoint.h"

#include "itkTransformFileReader.h"
#include "itkTransformFactoryBase.h"
#include "itkTransformFactory.h"
#include "itkMatrixOffsetTransformBase.h"

//Floris: The module factory is based on the ImageIO factory. 
// We manually register 2 dummy modules: itkTransformModule1 and itkMetricModule1. 
// in ITK CMake\UseITK.cmake sets up the IOFactory_Register_Manager for transforms and images
// Elastix (by Denis) uses (simpleITK's) Typelists to register all GPU filters 
//#include "itkModuleFactoryBase.h"
//#include "itkModuleFactory.h"

#include "itkModuleBase.h"
//#include "itkModuleFactory.h"

#include <map>
#include <string>

#include "itkTransformModule1.h"
#include "itkTransformModule1Factory.h"

#include "itkMetricModule1.h"
#include "itkMetricModule1Factory.h"

int main(int argc, char *argv[])
{
  typedef float ScalarType;

  typedef std::list< itk::LightObject::Pointer > RegisteredObjectsContainerType;
  RegisteredObjectsContainerType registeredIOs =
    itk::ObjectFactoryBase::CreateAllInstance("itkModuleBase");
  std::cout << "When CMake is not used to register the IO classes, there are\n"
    << registeredIOs.size()
    << " IO objects available to the Overlord.\n" << std::endl;

  std::cout << "After registering the TransformModule1 object, ";
  itk::TransformModule1Factory::RegisterOneFactory();
  itk::MetricModule1Factory::RegisterOneFactory();
  std::cout << "there are\n";
  registeredIOs = itk::ObjectFactoryBase::CreateAllInstance("itkModuleBase");
  std::cout << registeredIOs.size()
    << " IO objects available to the Overlord.\n" << std::endl;

  typedef itk::ModuleBase       ModuleType;

  typedef std::map<std::string, std::string> CriteriaType;
  typedef std::pair<std::string, std::string> CriteriumType;

  CriteriaType criteria1;
  //criteria1.insert(CriteriumType("ModuleOutput","Metric")); 
  criteria1["ModuleOutput"] = "Transform";
  CriteriaType criteria2;
  criteria2["ModuleInput"] = "Transform";
  //criteria1.insert(CriteriumType("ModuleInput", "Metric"));


  ModuleType::Pointer Node1 = itk::ModuleFactory::CreateModule(criteria1);
  
  ModuleType::Pointer Node2 = itk::ModuleFactory::CreateModule(criteria2);

  


  return EXIT_SUCCESS;
}