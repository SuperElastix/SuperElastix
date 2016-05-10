#=========================================================================
#
#  Copyright Leiden University Medical Center, Erasmus University Medical 
#  Center and contributors
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0.txt
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
#=========================================================================

set( proj BOOST )

# Note: It IS important to download different files on different OS's:
# on Unix-like systems, we need the file persmissions (only available in the .tar.gz),
# while on Windows, we need CR/LF line feeds (only available in the .zip)

set( BOOST_CONFIGURE_COMMAND )
if( UNIX )
  set( BOOST_URL "http://sourceforge.net/projects/boost/files/boost/1.59.0/boost_1_59_0.tar.gz")
  set( BOOST_MD5 51528a0e3b33d9e10aaa311d9eb451e3 )
  set( BOOST_CONFIGURE_COMMAND ./bootstrap.sh )
  set( BOOST_BUILD_COMMAND ./b2 --with-program_options --with-filesystem address-model=64)
else()
  if( WIN32 )
    set( BOOST_URL "http://sourceforge.net/projects/boost/files/boost/1.59.0/boost_1_59_0.zip")
    set( BOOST_MD5 08d29a2d85db3ebc8c6fdfa3a1f2b83c )
    set( BOOST_CONFIGURE_COMMAND cmd /C bootstrap.bat msvc )
    set( BOOST_BUILD_COMMAND b2.exe --with-program_options --with-filesystem address-model=64)
  endif()
endif()

set( BOOST_BUILD_DIR "${CMAKE_INSTALL_PREFIX}/${proj}-build/" )

ExternalProject_Add( BOOST
  BUILD_IN_SOURCE 1
  URL "${BOOST_URL}"
  URL_MD5 ${BOOST_MD5}
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND ${BOOST_CONFIGURE_COMMAND} 
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)

set( BOOST_ROOT "${CMAKE_INSTALL_PREFIX}/${proj}-prefix/src/BOOST" )
set( BOOST_LIBRARYDIR "${BOOST_ROOT}/stage/lib/" )

list( APPEND SUPERELASTIX_DEPENDENCIES ${proj} )
