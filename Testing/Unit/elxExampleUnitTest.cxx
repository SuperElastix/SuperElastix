#include <stdlib.h>
#include <time.h>

#include "gtest/gtest.h"

/**
 * This example demonstrates the elastix framework.
 * We run a couple of tests that outputs numbers for
 * use on the dashboard.
 */

TEST( GoogleTest, Example0 )
{
  srand( time( NULL ) );
  ASSERT_TRUE( true );
}

TEST( GoogleTest, Example1 )
{
  // Record random number between 0 and 9
  int MetricValue = rand() % 10;
  RecordProperty("MetricValue", MetricValue);
  ASSERT_TRUE( true );
}

TEST( GoogleTest, Example2 )
{
  // Record random number between 10 and 19
  int MetricValue = rand() % 10 + 10;
  RecordProperty("MetricValue", MetricValue);
  RecordProperty("OverlapCoefficient", 1);
  ASSERT_TRUE( true );
}

TEST( GoogleTest, Example3 )
{
  // Record random number between 20 and 29
  int MetricValue = rand() % 10 + 20;
  RecordProperty("MetricValue", MetricValue);
  ASSERT_TRUE( true );
}
