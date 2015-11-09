// multiple inheritance
#include <iostream>

// test case: there are two (slightly) incompatible codebases (i.e. 3rd party and 4th party!), each with an optimizer object and a metric object. 
// goal: make elastix components of all objects and define a handshake that checks if connections can be made.

/*************** below: example implementations of 3rd and 4th party code base (assume we cannot change that) *********************/
class Metric3rdPartyBase{
public:
  virtual int GetValue() = 0;
  virtual int GetDerivative() = 0;
};

class Optimizer3rdPartyBase{
public:
  virtual int SetMetric(Metric3rdPartyBase*) = 0;
  virtual int Optimize() = 0;
protected:
  Metric3rdPartyBase* theMetric;
};

class Metric4rdPartyBase{
public:
  virtual int GetCost() = 0; // with different naming convention than 3rd party
};

class Optimizer4rdPartyBase{
public:
  virtual int SetMetric(Metric4rdPartyBase*) = 0;
  virtual int DoOptimization() = 0; // with different naming convention than 3rd party
protected:
  Metric4rdPartyBase* theMetric;
};

class SSDMetric3rdParty : public Metric3rdPartyBase {
public:
  virtual int GetValue() { return 1; };
  virtual int GetDerivative() { return 2; };
};

class GDOptimizer3rdParty : public Optimizer3rdPartyBase {
public:
  GDOptimizer3rdParty();
  ~GDOptimizer3rdParty();
  virtual int SetMetric(Metric3rdPartyBase*);
  virtual int Optimize();
};


GDOptimizer3rdParty::GDOptimizer3rdParty()
{
  this->theMetric = nullptr;
}
GDOptimizer3rdParty::~GDOptimizer3rdParty()
{
}
int GDOptimizer3rdParty::SetMetric(Metric3rdPartyBase* metric)
{
  this->theMetric = metric;
  return 0;
}
int GDOptimizer3rdParty::Optimize()
{
  if (this->theMetric != nullptr)
  {
    std::cout << "GDOptimizer3rdParty->Optimize():" << std::endl;
    std::cout << "  theMetric->GetValue():" << theMetric->GetValue() << std::endl;
    std::cout << "  theMetric->GetDerivative():" << theMetric->GetDerivative() << std::endl;
  }
  return 0;
}


class SSDMetric4rdParty : public Metric4rdPartyBase {
public:
  virtual int GetCost() { return 3; };
};

class GDOptimizer4rdParty : public Optimizer4rdPartyBase {
public:
  GDOptimizer4rdParty();
  ~GDOptimizer4rdParty();
  virtual int SetMetric(Metric4rdPartyBase*);
  virtual int DoOptimization();
};

GDOptimizer4rdParty::GDOptimizer4rdParty()
{
  this->theMetric = nullptr;
}
GDOptimizer4rdParty::~GDOptimizer4rdParty()
{
}
int GDOptimizer4rdParty::SetMetric(Metric4rdPartyBase* metric)
{
  this->theMetric = metric;
  return 0;
}
int GDOptimizer4rdParty::DoOptimization()
{
  if (this->theMetric != nullptr)
  {
    std::cout << "GDOptimizer4rdParty->DoOptimization():" << std::endl;
    std::cout << "  theMetric->GetCost():" << theMetric->GetCost() << std::endl;
  }
  return 0;
}
/*************** above: example implementations of 3rd and 4th party code base (assume we cannot change that) *********************/
/*************** below: our elastix wrappers to connect varoius components (we can redesign that) *********************/
class ComponentBase {
public:
  virtual ~ComponentBase(){};
};

class MetricDerivativeInterface {
  public:
    virtual int GetDerivative() = 0;
};

class MetricValueInterface {
  public:
    virtual int GetValue() = 0;
};
template<class InterfaceT>
class InterfaceAcceptor {
public:
  virtual int Set(InterfaceT*) = 0;
  int Connect(ComponentBase*); 
};

template<class InterfaceT>
int InterfaceAcceptor<InterfaceT>::Connect(ComponentBase* providerComponent){

    InterfaceT* providerInterface = dynamic_cast<InterfaceT*> (providerComponent);
    if (!providerInterface)
    {
      std::cout << "providerComponent does not have required interface" << std::endl;
      return 0;
    }
    // connect value interfaces
    this->Set(providerInterface);
    return 1;
  }


class OptimizerUpdateInterface {
public:
  virtual int Update() = 0;
};


// wrapping into components: 
// SSDMetric3rdPartyComponent provides a value and a derivative
class SSDMetric3rdPartyComponent: public ComponentBase, public MetricDerivativeInterface, public MetricValueInterface {
public:
  SSDMetric3rdPartyComponent();
  ~SSDMetric3rdPartyComponent();
  SSDMetric3rdParty* theImplementation;
  int GetValue();
  int GetDerivative();
};

SSDMetric3rdPartyComponent::SSDMetric3rdPartyComponent()
{
  this->theImplementation = new SSDMetric3rdParty();
}
SSDMetric3rdPartyComponent::~SSDMetric3rdPartyComponent()
{
  delete this->theImplementation; 
}

int SSDMetric3rdPartyComponent::GetDerivative()
{
  return this->theImplementation->GetDerivative();
};

int SSDMetric3rdPartyComponent::GetValue()
{
  return this->theImplementation->GetValue();
};

// the GDOptimizer3rdParty expects that Metric3rdParty is will be set as input. All required interfaces will be delegated to a Metric3rdParty object.
class Metric3rdPartyWrapper : public Metric3rdPartyBase  {
public:
  void SetMetricValueComponent(MetricValueInterface*);
  void SetMetricDerivativeComponent(MetricDerivativeInterface*);
  virtual int GetValue();
  virtual int GetDerivative();
private:
  MetricValueInterface* metricval;
  MetricDerivativeInterface* metricderiv;
};

void Metric3rdPartyWrapper::SetMetricValueComponent(MetricValueInterface* metricValueComponent)
{
  this->metricval = metricValueComponent;
}

int Metric3rdPartyWrapper::GetValue()
{
  return this->metricval->GetValue();
}

void Metric3rdPartyWrapper::SetMetricDerivativeComponent(MetricDerivativeInterface* metricDerivativeComponent)
{
  this->metricderiv = metricDerivativeComponent;
}

int Metric3rdPartyWrapper::GetDerivative()
{
  return this->metricderiv->GetDerivative();
}

class GDOptimizer3rdPartyComponent : public ComponentBase, public InterfaceAcceptor<MetricValueInterface>, public InterfaceAcceptor<MetricDerivativeInterface>, public OptimizerUpdateInterface
{
public:
  GDOptimizer3rdPartyComponent();
  ~GDOptimizer3rdPartyComponent();
  GDOptimizer3rdParty* theImplementation;
  Metric3rdPartyWrapper* MetricObject;
  int Set(MetricValueInterface*);
  int Set(MetricDerivativeInterface*);
  int Update();
};

GDOptimizer3rdPartyComponent::GDOptimizer3rdPartyComponent()
{
  this->theImplementation = new GDOptimizer3rdParty();
  this->MetricObject = new Metric3rdPartyWrapper();
}
GDOptimizer3rdPartyComponent::~GDOptimizer3rdPartyComponent()
{
  delete this->theImplementation;
  delete this->MetricObject;
}

int GDOptimizer3rdPartyComponent::Set(MetricValueInterface* component)
{
  this->MetricObject->SetMetricValueComponent(component);
  return 0;
}
int GDOptimizer3rdPartyComponent::Set(MetricDerivativeInterface* component)
{
  this->MetricObject->SetMetricDerivativeComponent(component);
  return 0;
}
int GDOptimizer3rdPartyComponent::Update()
{
  this->theImplementation->SetMetric(this->MetricObject);
  return this->theImplementation->Optimize(); // 3rd party specific call
}


class SSDMetric4rdPartyComponent : public ComponentBase, public MetricValueInterface {
public:
  SSDMetric4rdPartyComponent();
  ~SSDMetric4rdPartyComponent();
  SSDMetric4rdParty* theImplementation;
  int GetValue();
};

SSDMetric4rdPartyComponent::SSDMetric4rdPartyComponent()
{
  this->theImplementation = new SSDMetric4rdParty();
}
SSDMetric4rdPartyComponent::~SSDMetric4rdPartyComponent()
{
  delete this->theImplementation;
}

int SSDMetric4rdPartyComponent::GetValue()
{
  return this->theImplementation->GetCost(); // translate method name
};

class Metric4rdPartyWrapper : public Metric4rdPartyBase  {
public:
  void SetMetricValueComponent(MetricValueInterface*);
  virtual int GetCost();
private:
  MetricValueInterface* metricval;
};

void Metric4rdPartyWrapper::SetMetricValueComponent(MetricValueInterface* metricValueComponent)
{
  this->metricval = metricValueComponent;
}

int Metric4rdPartyWrapper::GetCost()
{
  return this->metricval->GetValue();
}

class GDOptimizer4rdPartyComponent : public ComponentBase, public InterfaceAcceptor<MetricValueInterface>, public OptimizerUpdateInterface
{
public:
  GDOptimizer4rdPartyComponent();
  ~GDOptimizer4rdPartyComponent();
  GDOptimizer4rdParty* theImplementation;
  Metric4rdPartyWrapper* MetricObject;
  int Set(MetricValueInterface*);
  int Update();
};

GDOptimizer4rdPartyComponent::GDOptimizer4rdPartyComponent()
{
  this->theImplementation = new GDOptimizer4rdParty();
  this->MetricObject = new Metric4rdPartyWrapper();
}
GDOptimizer4rdPartyComponent::~GDOptimizer4rdPartyComponent()
{
  delete this->theImplementation;
  delete this->MetricObject;
}

int GDOptimizer4rdPartyComponent::Set(MetricValueInterface* component)
{
  this->MetricObject->SetMetricValueComponent(component);
  return 0;
}
int GDOptimizer4rdPartyComponent::Update()
{
  this->theImplementation->SetMetric(this->MetricObject);
  return this->theImplementation->DoOptimization(); // 4rd party specific call
}

int main () {
  {
    /************ testing interface casts ***********
    * expected: ok
    */
    SSDMetric3rdPartyComponent* tempmetric3p = new SSDMetric3rdPartyComponent();
    ComponentBase* metric3p = tempmetric3p;

    MetricValueInterface* valIF = dynamic_cast<MetricValueInterface*> (metric3p);
    std::cout << valIF->GetValue() << std::endl;

    MetricDerivativeInterface* derIF = dynamic_cast<MetricDerivativeInterface*> (metric3p);
    std::cout << derIF->GetDerivative() << std::endl;
  }
  /************ Connect metric4p to optimizer4p ***********
  * expected: ok
  */
  {
    SSDMetric4rdPartyComponent* tempmetric4p = new SSDMetric4rdPartyComponent();
    ComponentBase* metric4p = tempmetric4p; // type returned by our component factory

    GDOptimizer4rdPartyComponent* tempOptimizer4p = new GDOptimizer4rdPartyComponent();
    ComponentBase* optimizer4p = tempOptimizer4p; // type returned by our component factory

    InterfaceAcceptor<MetricValueInterface>* opValIF = dynamic_cast<InterfaceAcceptor<MetricValueInterface>*> (optimizer4p);
    if (!opValIF)
    {
      std::cout << "optimizer4p has no OptimizerValueInterface" << std::endl;
    }

    // connect value interfaces
    opValIF->Connect(metric4p);

    OptimizerUpdateInterface* opUpdIF = dynamic_cast<OptimizerUpdateInterface*> (optimizer4p);
    if (!opValIF)
    {
      std::cout << "optimizer4p has no OptimizerUpdateInterface" << std::endl;
    }
    
    // Update the optimizer component 
    opUpdIF->Update();
  }
  /************ Connect metric3p to optimizer4p ***********
  * expected: ok
  * optimizer4p will only use/have access to the GetValue interface of metric3p 
  */
  {
    SSDMetric3rdPartyComponent* tempmetric3p = new SSDMetric3rdPartyComponent();
    ComponentBase* metric3p = tempmetric3p; // type returned by our component factory

    GDOptimizer4rdPartyComponent* tempOptimizer4p = new GDOptimizer4rdPartyComponent();
    ComponentBase* optimizer4p = tempOptimizer4p; // type returned by our component factory

    InterfaceAcceptor<MetricValueInterface>* opValIF = dynamic_cast<InterfaceAcceptor<MetricValueInterface>*> (optimizer4p);
    if (!opValIF)
    {
      std::cout << "optimizer4p has no OptimizerValueInterface" << std::endl;
    }

    // connect value interfaces
    if (!opValIF->Connect(metric3p))
    {
      std::cout << "metric3p cannot connect to optimizer4p by ValueInterface" << std::endl;
    }
    
    OptimizerUpdateInterface* opUpdIF = dynamic_cast<OptimizerUpdateInterface*> (optimizer4p);
    if (!opValIF)
    {
      std::cout << "optimizer4p has no OptimizerUpdateInterface" << std::endl;
    }

    // Update the optimizer component 
    opUpdIF->Update();
  }
  /************ Connect metric4p to optimizer3p ***********
  * expected: fail
  * optimizer3p needs a metric with GetDerivative which metric4p doesn't have
  */
  {
    SSDMetric4rdPartyComponent* tempmetric4p = new SSDMetric4rdPartyComponent();
    ComponentBase* metric4p = tempmetric4p; // type returned by our component factory

    GDOptimizer3rdPartyComponent* tempOptimizer3p = new GDOptimizer3rdPartyComponent();
    ComponentBase* optimizer3p = tempOptimizer3p; // type returned by our component factory

    InterfaceAcceptor<MetricValueInterface>* opValIF = dynamic_cast<InterfaceAcceptor<MetricValueInterface>*> (optimizer3p);
    if (!opValIF)
    {
      std::cout << "optimizer3p has no OptimizerValueInterface" << std::endl;
    }

    // connect value interfaces
    if (!opValIF->Connect(metric4p))
    {
      std::cout << "metric4p cannot connect to optimizer3p by ValueInterface" << std::endl;
    }

    //opValIF->Set(tempmetric4p);

    InterfaceAcceptor<MetricDerivativeInterface>* opDerivIF = dynamic_cast<InterfaceAcceptor<MetricDerivativeInterface>*> (optimizer3p);
    if (!opDerivIF)
    {
      std::cout << "optimizer3p has no OptimizerDerivativeInterface" << std::endl;
    }
    // connect derivative interfaces
    if (!opDerivIF->Connect(metric4p))
    {
      std::cout << "metric4p cannot connect to optimizer3p by DerivativeInterface" << std::endl;
    }
    

    OptimizerUpdateInterface* opUpdIF = dynamic_cast<OptimizerUpdateInterface*> (optimizer3p);
    if (!opValIF)
    {
      std::cout << "optimizer3p has no OptimizerUpdateInterface" << std::endl;
    }

    // Update the optimizer component 
    // opUpdIF->Update(); // will fail since the metric does'nt have GetDerivative()
  }
  return 0;
}