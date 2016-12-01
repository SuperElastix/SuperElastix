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

#ifndef selxInterfaces_h
#define selxInterfaces_h

#include "selxInterfaceTraits.h"

namespace selx
{
// Define the providing interfaces abstractly

class RegistrationControllerStartInterface
{
  // A special interface: the Overlord checks components for this type of interface.
  // This interface is to control the execution of the network

public:

  virtual void RegistrationControllerStart() = 0;
};

class RunRegistrationInterface
{
  // This interface is to control the execution of the network

public:

  virtual void RunRegistration() = 0;
};

class AfterRegistrationInterface
{
  // This interface is to control the execution of the network

public:

  virtual void AfterRegistration() = 0;
};

class RunResolutionInterface
{
  // This interface is to control the execution of the network

public:

  virtual bool RunResolution() = 0;
};

class ReconnectTransformInterface
{
  // This interface is to control the execution of the network

public:

  virtual void ReconnectTransform() = 0;
};

} // end namespace selx

#endif // #define selxInterfaces_h
