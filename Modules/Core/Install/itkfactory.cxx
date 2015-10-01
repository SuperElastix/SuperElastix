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
//#include "itkModuleIOFactoryRegisterManager.h"

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

//Floris: The module factory is based on the transformIO factory. 
// By default it creates 2 dummy modules: itkTransformModule1 and itkMetricModule1
//CMake\UseITK.cmake sets up the IOFactory_Register_Manager for transforms and images
//#include "itkModuleFactoryBase.h"
//#include "itkModuleFactory.h"

#include "itkModuleIOBase.h"
//#include "itkModuleIOFactory.h"

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
    itk::ObjectFactoryBase::CreateAllInstance("itkModuleIOBase");
  std::cout << "When CMake is not used to register the IO classes, there are\n"
    << registeredIOs.size()
    << " IO objects available to the Overlord.\n" << std::endl;

  std::cout << "After registering the TransformModule1 object, ";
  itk::TransformModule1Factory::RegisterOneFactory();
  itk::MetricModule1Factory::RegisterOneFactory();
  std::cout << "there are\n";
  registeredIOs = itk::ObjectFactoryBase::CreateAllInstance("itkModuleIOBase");
  std::cout << registeredIOs.size()
    << " IO objects available to the Overlord.\n" << std::endl;

  std::cout << "Now, when we try to read a MetaImage, we will ";
  //typedef itk::ModuleBaseTemplate< ScalarType >       ModuleType;
  //typedef itk::ModuleFactoryBase                      ModuleFactoryType;

  typedef itk::ModuleIOBase       ModuleIOType;
  //typedef itk::ModuleIOFactoryTemplate< ScalarType > ModuleFactoryIOType;
  std::string moduleName("Metric");

  //register a 3rd module
  //typedef itk::TransformModule1< double, 3, 3 > TransformModule1Type;
  //itk::ModuleFactory<TransformModule1Type>::RegisterModule();

  typedef std::map<std::string, std::string> CriteriaType;
  typedef std::pair<std::string, std::string> CriteriumType;

  CriteriaType criteria1;
  //criteria1.insert(CriteriumType("ModuleOutput","Metric")); 
  criteria1["ModuleOutput"] = "Transform";
  CriteriaType criteria2;
  criteria2["ModuleInput"] = "Transform";
  //criteria1.insert(CriteriumType("ModuleInput", "Metric"));


  ModuleIOType::Pointer Node1 = itk::ModuleFactoryBase::CreateModuleIO(criteria1);

  ModuleIOType::Pointer Node2 = itk::ModuleFactoryBase::CreateModuleIO(criteria2);

  
  

  return EXIT_SUCCESS;
}