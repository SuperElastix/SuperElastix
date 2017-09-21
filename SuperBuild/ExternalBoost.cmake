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
set( B2_ARGS --with-program_options --with-filesystem --with-log --with-date_time --with-chrono --with-thread --with-regex address-model=64 )
if( UNIX )
  set( BOOST_URL "http://dl.bintray.com/boostorg/release/1.65.0/source/boost_1_65_0.tar.gz")
  set( BOOST_MD5 d75a27ad1a34cfc7c9b5cb92594410c3 )
  set( BOOST_CONFIGURE_COMMAND ./bootstrap.sh )
  set( BOOST_BUILD_COMMAND ./b2 ${B2_ARGS})
else()
  if( WIN32 )
    # If building on Windows: The target machine should be Windows 7 or higher.
    # Note that BOOST_USE_WINAPI_VERSION should match with _WIN32_WINNT. 
    set(B2_ARGS "define=BOOST_USE_WINAPI_VERSION=0x0601" ${B2_ARGS})
    if(MSVC11)
      set(B2_ARGS "--toolset=msvc-11.0" ${B2_ARGS})
    elseif(MSVC12)
      set(B2_ARGS "--toolset=msvc-12.0" ${B2_ARGS})
    elseif(MSVC14)
      # CMake defines MSVC14 for both VS2015 and VS2017!
      if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.10)
        set(B2_ARGS "--toolset=msvc-14.0" ${B2_ARGS})
      else()
        set(B2_ARGS "--toolset=msvc-14.1" ${B2_ARGS})
      endif()
    endif(MSVC11)
    set( BOOST_URL "http://dl.bintray.com/boostorg/release/1.65.0/source/boost_1_65_0.zip")
    set( BOOST_MD5 eb1e11262e0cfc6949d054f6d8d25dc6 )
    set( BOOST_CONFIGURE_COMMAND cmd /C bootstrap.bat msvc )
    set( BOOST_BUILD_COMMAND b2.exe ${B2_ARGS})
  endif()
endif()

ExternalProject_Add( BOOST
  BUILD_IN_SOURCE 1
  URL "${BOOST_URL}"
  URL_MD5 ${BOOST_MD5}
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND ${BOOST_CONFIGURE_COMMAND} 
  BUILD_COMMAND ${BOOST_BUILD_COMMAND}
  INSTALL_COMMAND ""
)

set( BOOST_ROOT "${PROJECT_BINARY_DIR}/${proj}-prefix/src/BOOST" )
set( BOOST_LIBRARYDIR "${BOOST_ROOT}/stage/lib/" )

list( APPEND SUPERELASTIX_DEPENDENCIES ${proj} )
