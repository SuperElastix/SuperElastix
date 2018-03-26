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

#ifndef Example4thPartyCode_h
#define Example4thPartyCode_h

#include <iostream>

namespace Example4thParty
{
// test case: there are two (slightly) incompatible codebases (i.e. 3rd party and 4th party!), each with an optimizer object and a metric object.
// goal: make SuperEelastix components from all objects and define a handshake that checks if connections can be made.

/*************** below: example implementations of 3rd and 4th party code base (assume we cannot change that) *********************/

class Metric4thPartyBase
{
public:

  virtual int GetCost() = 0; // with different naming convention than 3rd party
};

class Optimizer4thPartyBase
{
public:

  virtual int SetMetric( Metric4thPartyBase * ) = 0;

  virtual int DoOptimization() = 0; // with different naming convention than 3rd party

protected:

  Metric4thPartyBase * theMetric;
};

class SSDMetric4thParty : public Metric4thPartyBase
{
public:

  virtual int GetCost() { return 3; }
};

class GDOptimizer4thParty : public Optimizer4thPartyBase
{
public:

  GDOptimizer4thParty();
  ~GDOptimizer4thParty();
  virtual int SetMetric( Metric4thPartyBase * );

  virtual int DoOptimization();
};
} // end namespave Example4thParty
#endif // #define Example4thPartyCode_h
