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

#ifndef selxStreamToString_h
#define selxStreamToString_h

#endif // selxStreamToString_h

class StreamToString
{
public:

  // Stream T to string
  template < typename T >
  std::string operator<<( const T& value ) {
    std::ostringstream out;
    out << value;
    return out.str();
  }

  // Stream std:vector to string
  // TODO: Use std::copy_n to print [n1, n2, ... , n-1, n] if vector is long
  template < typename T >
  std::string operator<<( const std::vector< T >& v ) {
    std::ostringstream out;
    if( !v.empty() ) {
      if( v.size() > 1 ) out << '[';
      std::copy( v.begin(), v.end(), std::ostream_iterator< T >( out, ", " ) );
      out << "\b\b";
      if( v.size() > 1 ) out << "]";
    }
    return out.str();
  }

  // Stream std::map< T, T > to string
  template < typename T >
  std::string operator<<( const std::map< T, T >& m ) {
    std::ostringstream out;
    if( !m.empty() ) {
      out << "{";
      for( const auto& item : m )
      {
        out << item.first << ": " << item.second << ", ";
      }
      out << "\b\b}";
    }
    return out.str();
  }

  // Stream std::map< T, std::vector< T > > to string
  template < typename T >
  std::string operator<<( const std::map< T, std::vector< T > >& m ) {
    std::ostringstream out;
    if( !m.empty() ) {
      out << "{";
      for( const auto& item : m )
      {
        out << item.first << ": " << this << item.second << ", ";
      }
      out << "\b\b}";
    }
    return out.str();
  }
};
