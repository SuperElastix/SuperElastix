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

#include <stdlib.h>
#include <time.h>

#include "gtest/gtest.h"

/**
 * This example demonstrates the SuperElastix framework.
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
