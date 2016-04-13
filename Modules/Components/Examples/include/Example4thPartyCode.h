#ifndef Example4thPartyCode_h
#define Example4thPartyCode_h

#include <iostream>

namespace Example4thParty
{
	
// test case: there are two (slightly) incompatible codebases (i.e. 3rd party and 4th party!), each with an optimizer object and a metric object. 
// goal: make SuperEelastix components from all objects and define a handshake that checks if connections can be made.

/*************** below: example implementations of 3rd and 4th party code base (assume we cannot change that) *********************/

class Metric4thPartyBase{
public:
  virtual int GetCost() = 0; // with different naming convention than 3rd party
};

class Optimizer4thPartyBase{
public:
  virtual int SetMetric(Metric4thPartyBase*) = 0;
  virtual int DoOptimization() = 0; // with different naming convention than 3rd party
protected:
  Metric4thPartyBase* theMetric;
};

class SSDMetric4thParty : public Metric4thPartyBase {
public:
  virtual int GetCost() { return 3; };
};

class GDOptimizer4thParty : public Optimizer4thPartyBase {
public:
  GDOptimizer4thParty();
  ~GDOptimizer4thParty();
  virtual int SetMetric(Metric4thPartyBase*);
  virtual int DoOptimization();
};
} // end namespave Example4thParty
#endif // #define Example4thPartyCode_h