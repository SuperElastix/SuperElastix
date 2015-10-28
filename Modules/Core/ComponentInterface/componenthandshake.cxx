// multiple inheritance
#include <iostream>

class Metric3rdPartyBase{
public:
  virtual int GetValue() = 0;
  virtual int GetDerivative() = 0;
};

class Optimizer3rdPartyBase{
public:
  virtual int SetMetric(Metric3rdPartyBase*) = 0;
  virtual int Optimize() = 0;
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
  virtual int SetMetric(Metric3rdPartyBase*);
  virtual int Optimize();
  Metric3rdPartyBase* metric;
};

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

class OptimizerValueInterface {
public:
  virtual int SetMetricValueComponentInterface(MetricValueInterface*) = 0;
};

class OptimizerDerivativeInterface {
public:
  virtual int SetMeticDerivativeComponentInterface(MetricDerivativeInterface*) = 0;
};

class OptimizerUpdateInterface {
public:
  virtual int Update() = 0;
};

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

class GDOptimizer4rdPartyComponent : public ComponentBase, public OptimizerValueInterface, public OptimizerUpdateInterface
{
public:
  GDOptimizer4rdPartyComponent();
  ~GDOptimizer4rdPartyComponent();
  GDOptimizer4rdParty* theImplementation;
  Metric4rdPartyWrapper* MetricObject;
  MetricDerivativeInterface* metricDerivativeInterface;
  int SetMetricValueComponentInterface(MetricValueInterface*);
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

int GDOptimizer4rdPartyComponent::SetMetricValueComponentInterface(MetricValueInterface* component)
{
  this->MetricObject->SetMetricValueComponent(component);
  return 0;
}
int GDOptimizer4rdPartyComponent::Update()
{
  this->theImplementation->SetMetric(this->MetricObject);
  return this->theImplementation->DoOptimization();
}

int main () {
  {
    SSDMetric3rdPartyComponent* tempmetric3p = new SSDMetric3rdPartyComponent();
    ComponentBase* metric3p = tempmetric3p;

    MetricValueInterface* valIF = dynamic_cast<MetricValueInterface*> (metric3p);
    std::cout << valIF->GetValue() << std::endl;

    MetricDerivativeInterface* derIF = dynamic_cast<MetricDerivativeInterface*> (metric3p);
    std::cout << derIF->GetDerivative() << std::endl;
  }
  /************************/
  {
    SSDMetric4rdPartyComponent* tempmetric4p = new SSDMetric4rdPartyComponent();
    ComponentBase* metric4p = tempmetric4p;

    GDOptimizer4rdPartyComponent* tempOptimizer4p = new GDOptimizer4rdPartyComponent();
    ComponentBase* optimizer4p = tempOptimizer4p;

    MetricValueInterface* metvalIF = dynamic_cast<MetricValueInterface*> (metric4p);
    if (!metvalIF)
    {
      std::cout << "metric4p has no MetricValueInterface" << std::endl;
    }

    OptimizerValueInterface* opValIF = dynamic_cast<OptimizerValueInterface*> (optimizer4p);
    if (!opValIF)
    {
      std::cout << "optimizer4p has no OptimizerValueInterface" << std::endl;
    }

    // connect value interfaces
    opValIF->SetMetricValueComponentInterface(metvalIF); 
    OptimizerUpdateInterface* opUpdIF = dynamic_cast<OptimizerUpdateInterface*> (optimizer4p);
    if (!opValIF)
    {
      std::cout << "optimizer4p has no OptimizerUpdateInterface" << std::endl;
    }
    
    // Update the optimizer component 
    opUpdIF->Update();
  }

  {
    SSDMetric3rdPartyComponent* tempmetric3p = new SSDMetric3rdPartyComponent();
    ComponentBase* metric3p = tempmetric3p;

    GDOptimizer4rdPartyComponent* tempOptimizer4p = new GDOptimizer4rdPartyComponent();
    ComponentBase* optimizer4p = tempOptimizer4p;

    MetricValueInterface* metvalIF = dynamic_cast<MetricValueInterface*> (metric3p);
    if (!metvalIF)
    {
      std::cout << "metric3p has no MetricValueInterface" << std::endl;
    }

    OptimizerValueInterface* opValIF = dynamic_cast<OptimizerValueInterface*> (optimizer4p);
    if (!opValIF)
    {
      std::cout << "optimizer4p has no OptimizerValueInterface" << std::endl;
    }

    // connect value interfaces
    opValIF->SetMetricValueComponentInterface(metvalIF);
    OptimizerUpdateInterface* opUpdIF = dynamic_cast<OptimizerUpdateInterface*> (optimizer4p);
    if (!opValIF)
    {
      std::cout << "optimizer4p has no OptimizerUpdateInterface" << std::endl;
    }

    // Update the optimizer component 
    opUpdIF->Update();
  }

  return 0;
}