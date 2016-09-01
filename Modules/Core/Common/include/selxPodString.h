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
#ifndef selxPodString_h
#define selxPodString_h

#include "selxStaticErrorMessageRevealT.h"

namespace selx
{


	template< typename T >
	struct PodString
	{
		static_assert(StaticErrorMessageRevealT<T>::False, "Please Implement PodString<T> for this T");
	};

	template< >
	struct PodString< unsigned int >
	{
		static const char * Get()
		{
			return "unsigned int";
		}
	};

	template< >
	struct PodString< int >
	{
		static const char * Get()
		{
			return "int";
		}
	};

	template< >
	struct PodString< float >
	{
		static const char * Get()
		{
			return "float";
		}
	};


	template< >
	struct PodString< double >
	{
		static const char * Get()
		{
			return "double";
		}
	};
}

#endif //selxPodString_h