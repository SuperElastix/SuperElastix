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

#include "selxSSDMetric3rdPartyComponent.h"
#include "selxGDOptimizer3rdPartyComponent.h"
#include "selxSSDMetric4thPartyComponent.h"
#include "selxGDOptimizer4thPartyComponent.h"

#include "gtest/gtest.h"

namespace selx
{
class InterfaceTest : public ::testing::Test
{
public:

  virtual void SetUp()
  {
    metric3p    = new SSDMetric3rdPartyComponent();
    optimizer3p = new GDOptimizer3rdPartyComponent();

    metric4p    = new SSDMetric4thPartyComponent();
    optimizer4p = new GDOptimizer4thPartyComponent();
  }


  virtual void TearDown()
  {
    //delete metric3p;
    //delete optimizer3p;
    //delete metric4p;
    //delete optimizer4p;
  }


  // types as if returned by our component factory
  ComponentBase * metric3p;
  ComponentBase * optimizer3p;
  ComponentBase * metric4p;
  ComponentBase * optimizer4p;
};

TEST_F( InterfaceTest, InterfaceNameTraits )
{
  EXPECT_STREQ( InterfaceName< MetricValueInterface >::Get(), "MetricValueInterface" );
  EXPECT_STREQ( InterfaceName< InterfaceAcceptor< MetricValueInterface >>::Get(), "MetricValueInterface" );
}

TEST_F( InterfaceTest, DynamicCast )
{
  int returnval;
  //metric3p should have a MetricValueInterface
  MetricValueInterface * valueIF = dynamic_cast< MetricValueInterface * >( metric3p );
  ASSERT_NE( valueIF, nullptr );
  EXPECT_NO_THROW( returnval = valueIF->GetValue() );

  //metric3p should have a MetricDerivativeInterface
  MetricDerivativeInterface * derivativeIF = dynamic_cast< MetricDerivativeInterface * >( metric3p );
  ASSERT_NE( derivativeIF, nullptr );
  EXPECT_NO_THROW( returnval = derivativeIF->GetDerivative() );

  //optimizer3p should have a OptimizerUpdateInterface
  OptimizerUpdateInterface * updateIF = dynamic_cast< OptimizerUpdateInterface * >( optimizer3p );
  ASSERT_NE( updateIF, nullptr );
  //EXPECT_NO_THROW(returnval = updateIF->Update()); // Update can only be called if metric and optimizer are connected

  //optimizer3p should have a InterfaceAcceptor<MetricValueInterface>
  InterfaceAcceptor< MetricValueInterface > * valueAcceptorIF = dynamic_cast< InterfaceAcceptor< MetricValueInterface > * >( optimizer3p );
  ASSERT_NE( valueAcceptorIF, nullptr );

  //optimizer3p should have a InterfaceAcceptor<MetricDerivativeInterface>
  InterfaceAcceptor< MetricDerivativeInterface > * derivativeAcceptorIF
    = dynamic_cast< InterfaceAcceptor< MetricDerivativeInterface > * >( optimizer3p );
  ASSERT_NE( derivativeAcceptorIF, nullptr );
}

TEST_F( InterfaceTest, ConnectByName )
{
  InterfaceStatus IFstatus;
  EXPECT_NO_THROW(IFstatus = optimizer3p->CanAcceptConnectionFrom( metric3p, { { "NameOfInterface", "MetricValueInterface" } }));
  EXPECT_EQ( IFstatus, InterfaceStatus::success );

  EXPECT_NO_THROW(IFstatus = optimizer3p->CanAcceptConnectionFrom( metric4p, { { "NameOfInterface", "MetricValueInterface" } }));
  EXPECT_EQ( IFstatus, InterfaceStatus::success );

  EXPECT_NO_THROW(IFstatus = optimizer4p->CanAcceptConnectionFrom( metric3p, { { "NameOfInterface", "MetricValueInterface" } }));
  EXPECT_EQ( IFstatus, InterfaceStatus::success );

  EXPECT_NO_THROW(IFstatus = optimizer4p->CanAcceptConnectionFrom( metric4p, { { "NameOfInterface", "MetricValueInterface" } }));
  EXPECT_EQ( IFstatus, InterfaceStatus::success );

  EXPECT_NO_THROW(IFstatus = optimizer3p->CanAcceptConnectionFrom( metric3p, { { "NameOfInterface", "MetricDerivativeInterface" } }));
  EXPECT_EQ( IFstatus, InterfaceStatus::success );

  EXPECT_NO_THROW(IFstatus = optimizer3p->CanAcceptConnectionFrom( metric4p, { { "NameOfInterface", "MetricDerivativeInterface" } }));
  EXPECT_EQ( IFstatus, InterfaceStatus::noprovider );

  EXPECT_NO_THROW(IFstatus = optimizer4p->CanAcceptConnectionFrom( metric3p, { { "NameOfInterface", "MetricDerivativeInterface" } }));
  EXPECT_EQ( IFstatus, InterfaceStatus::noaccepter );

  EXPECT_NO_THROW(IFstatus = optimizer3p->CanAcceptConnectionFrom(metric3p, { }));
  EXPECT_EQ(IFstatus, InterfaceStatus::multiple);
}

TEST_F( InterfaceTest, ConnectAll )
{
  int                        connectionCount = 0;
  int                        returnval;
  OptimizerUpdateInterface * updateIF;
  EXPECT_NO_THROW( connectionCount = optimizer3p->AcceptConnectionFrom( metric3p ) );
  EXPECT_EQ( connectionCount, 2 ); // both MetricValueInterface and MetricDerivativeInterface are connected

  //optimizer3p should have a OptimizerUpdateInterface
  updateIF = dynamic_cast< OptimizerUpdateInterface * >( optimizer3p );
  ASSERT_NE( updateIF, nullptr );
  EXPECT_NO_THROW( returnval = updateIF->Update() ); // Update can only be called if metric and optimizer are connected

  EXPECT_NO_THROW( connectionCount = optimizer3p->AcceptConnectionFrom( metric4p ) );
  EXPECT_EQ( connectionCount, 1 ); // only MetricValueInterface is connected

  //metric4p does not have MetricDerivativeInterface, so optimizer3p cannot run

  EXPECT_NO_THROW( connectionCount = optimizer4p->AcceptConnectionFrom( metric3p ) );
  EXPECT_EQ( connectionCount, 1 ); // only MetricValueInterface is connected

  updateIF = dynamic_cast< OptimizerUpdateInterface * >( optimizer4p );
  ASSERT_NE( updateIF, nullptr );
  EXPECT_NO_THROW( returnval = updateIF->Update() );

  EXPECT_NO_THROW( connectionCount = optimizer4p->AcceptConnectionFrom( metric4p ) );
  EXPECT_EQ( connectionCount, 1 ); // only MetricValueInterface is connected

  updateIF = dynamic_cast< OptimizerUpdateInterface * >( optimizer4p );
  ASSERT_NE( updateIF, nullptr );
  EXPECT_NO_THROW( returnval = updateIF->Update() );

  EXPECT_NO_THROW( connectionCount = metric4p->AcceptConnectionFrom( optimizer4p ) );
  EXPECT_EQ( connectionCount, 0 ); // cannot connect in this direction

  ConflictinUpdateInterface * update2IF = dynamic_cast< ConflictinUpdateInterface * >( optimizer4p );
  ASSERT_NE( update2IF, nullptr );
  EXPECT_NO_THROW( returnval = update2IF->Update( update2IF ) );
}
} // namespace selx
