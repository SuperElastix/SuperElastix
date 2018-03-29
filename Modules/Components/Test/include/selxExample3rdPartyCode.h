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

#ifndef Example3rdPartyCode_h
#define Example3rdPartyCode_h

#include <iostream>
namespace Example3rdParty
{
// test case: there are two (slightly) incompatible codebases (i.e. 3rd party and 4th party!), each with an optimizer object and a metric object.
// goal: make SuperEelastix components from all objects and define a handshake that checks if connections can be made.

/*************** below: example implementations of 3rd and 4th party code base (assume we cannot change that) *********************/

class Metric3rdPartyBase
{
public:

  virtual int GetValue() = 0;

  virtual int GetDerivative() = 0;
};

class Optimizer3rdPartyBase
{
public:

  virtual int SetMetric( Metric3rdPartyBase * ) = 0;

  virtual int Optimize() = 0;

protected:

  Metric3rdPartyBase * theMetric;
};

class SSDMetric3rdParty : public Metric3rdPartyBase
{
public:

  virtual int GetValue() { return 1; }
  virtual int GetDerivative() { return 2; }
};

class GDOptimizer3rdParty : public Optimizer3rdPartyBase
{
public:

  GDOptimizer3rdParty();
  ~GDOptimizer3rdParty();
  virtual int SetMetric( Metric3rdPartyBase * );

  virtual int Optimize();
};
} // end namespave Example3rdParty
#endif // #define Example3rdPartyCode_h
