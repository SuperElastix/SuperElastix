#include "SSDMetric3rdPartyComponent.h"
#include "GDOptimizer3rdPartyComponent.h"
#include "SSDMetric4thPartyComponent.h"
#include "GDOptimizer4thPartyComponent.h"

#include "gtest/gtest.h"

namespace elx {

class InterfaceTest : public ::testing::Test {
public:

  virtual void SetUp() {
    metric3p = new SSDMetric3rdPartyComponent();
    optimizer3p = new GDOptimizer3rdPartyComponent();

    metric4p = new SSDMetric4thPartyComponent();
    optimizer4p = new GDOptimizer4thPartyComponent();
  }

  virtual void TearDown() {
    delete metric3p;
    delete optimizer3p;
    delete metric4p;
    delete optimizer4p;
  }
  // types as if returned by our component factory
  ComponentBase* metric3p;
  ComponentBase* optimizer3p;
  ComponentBase* metric4p;
  ComponentBase* optimizer4p;

};

TEST_F( InterfaceTest, InterfaceNameTraits )
{
  EXPECT_STREQ(InterfaceName<MetricValueInterface>::Get(), "MetricValueInterface");
  EXPECT_STREQ(InterfaceName<InterfaceAcceptor<MetricValueInterface> >::Get(), "MetricValueInterface");
}

TEST_F( InterfaceTest, DynamicCast )
{
  int returnval;
  //metric3p should have a MetricValueInterface
  MetricValueInterface* valueIF = dynamic_cast<MetricValueInterface*> (metric3p);
  ASSERT_NE(valueIF, nullptr);
  EXPECT_NO_THROW(returnval = valueIF->GetValue());

  //metric3p should have a MetricDerivativeInterface
  MetricDerivativeInterface* derivativeIF = dynamic_cast<MetricDerivativeInterface*> (metric3p);
  ASSERT_NE(derivativeIF, nullptr);
  EXPECT_NO_THROW(returnval = derivativeIF->GetDerivative());

  //optimizer3p should have a OptimizerUpdateInterface
  OptimizerUpdateInterface* updateIF = dynamic_cast<OptimizerUpdateInterface*> (optimizer3p);
  ASSERT_NE(updateIF, nullptr);
  EXPECT_NO_THROW(returnval = updateIF->Update());

  //optimizer3p should have a InterfaceAcceptor<MetricValueInterface> 
  InterfaceAcceptor<MetricValueInterface>* valueAcceptorIF = dynamic_cast<InterfaceAcceptor<MetricValueInterface>*> (optimizer3p);
  ASSERT_NE(valueAcceptorIF, nullptr);

  //optimizer3p should have a InterfaceAcceptor<MetricDerivativeInterface> 
  InterfaceAcceptor<MetricDerivativeInterface>* derivativeAcceptorIF = dynamic_cast<InterfaceAcceptor<MetricDerivativeInterface>*> (optimizer3p);
  ASSERT_NE(derivativeAcceptorIF, nullptr);
  
}

TEST_F( InterfaceTest, ComponentConnecting )
{
  interfaceStatus IFstatus;
  EXPECT_NO_THROW(IFstatus = optimizer3p->ConnectFrom("MetricValueInterface", metric3p));
  EXPECT_EQ(IFstatus, interfaceStatus::success);

  EXPECT_NO_THROW(IFstatus = optimizer3p->ConnectFrom("MetricValueInterface", metric4p));
  EXPECT_EQ(IFstatus, interfaceStatus::success);

  EXPECT_NO_THROW(IFstatus = optimizer4p->ConnectFrom("MetricValueInterface", metric3p));
  EXPECT_EQ(IFstatus, interfaceStatus::success);

  EXPECT_NO_THROW(IFstatus = optimizer4p->ConnectFrom("MetricValueInterface", metric4p));
  EXPECT_EQ(IFstatus, interfaceStatus::success);


  EXPECT_NO_THROW(IFstatus = optimizer3p->ConnectFrom("MetricDerivativeInterface", metric3p));
  EXPECT_EQ(IFstatus, interfaceStatus::success);

  EXPECT_NO_THROW(IFstatus = optimizer3p->ConnectFrom("MetricDerivativeInterface", metric4p));
  EXPECT_EQ(IFstatus, interfaceStatus::noprovider);

  EXPECT_NO_THROW(IFstatus = optimizer4p->ConnectFrom("MetricDerivativeInterface", metric3p));
  EXPECT_EQ(IFstatus, interfaceStatus::noaccepter);

}
} // namespace elx
