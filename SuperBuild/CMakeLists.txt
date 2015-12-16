cmake_minimum_required( VERSION 3.0.2 )

# ---------------------------------------------------------------------
project( SuperElastixSuperBuild )

find_package( Git REQUIRED )

include( ExternalProject )

set( CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../CMake
  ${CMAKE_MODULE_PATH}
)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH "SuperBuild install directory" FORCE)

# ---------------------------------------------------------------------
# Elastix SuperBuild configuration

# Build release by default
if( NOT DEFINED CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE "Release" )
endif()

# Build examples by default
# Examples will be build as an external project to verify the installation of elastix
option( SUPERELASTIX_BUILD_EXAMPLES "Enable building examples." ON )

# Build tests by default
option( SUPERELASTIX_BUILD_TESTING "Enable building tests." ON )
if( SUPERELASTIX_BUILD_TESTING )
  option( SUPERELASTIX_BUILD_BENCHMARKING "Enable building benchmarks." ON )
  option( SUPERELASTIX_BUILD_DASHBOARD "Enable building benchmarks." ON )
endif()

# Do not build dashboard by default
option( SUPERELASTIX_BUILD_DASHBOARD "Enable building dashboard." OFF )


# ---------------------------------------------------------------------
# Build ITK

set( ITK_VERSION_MAJOR "4" )
set( ITK_VERSION_MINOR "8" )
set( ITK_VERSION_PATCH "1" )
set( ITK_VERSION_STRING "${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR}.${ITK_VERSION_PATCH}" )

mark_as_advanced( USE_SYSTEM_ITK )
option( USE_SYSTEM_ITK "Use an installed version of ITK" OFF )

if( USE_SYSTEM_ITK )
  find_package( ITK ${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR} REQUIRED )
  include( elxRequiredITKModules )
else()
  include( ExternalITK )
endif()

# ---------------------------------------------------------------------
# Build Old Elastix

set( ELASTIX_VERSION_MAJOR "4" )
set( ELASTIX_VERSION_MINOR "8" )
set( ELASTIX_VERSION_STRING "${ELASTIX_VERSION_MAJOR}.${ELASTIX_VERSION_MINOR}" )

mark_as_advanced( SUPERELASTIX_BUILD_ELASTIX )
option( SUPERELASTIX_BUILD_ELASTIX "Build SuperElastix support Elastix ${ELASTIX_VERSION_STRING}." ON )
if( SUPERELASTIX_BUILD_ELASTIX )
  if( USE_SYSTEM_ELASTIX )
    if( NOT EXISTS ${ELASTIX_USE_FILE} )
      # Expose CMake variable to user
      set( ELASTIX_USE_FILE )
      # Stop the build
      message( FATAL_ERROR "Please point ELASTIX_USE_FILE to your systems UseElastix.cmake file." )
    endif()
  else()
    include( ExternalElastix )
  endif()
endif()

# ---------------------------------------------------------------------
# Boost Graph Library

mark_as_advanced( USE_SYSTEM_BOOST )
option( USE_SYSTEM_BOOST "Use an installed version of BOOST" OFF )
if( USE_SYSTEM_BOOST )
  find_package( BOOST REQUIRED graph )
else()
  include( ExternalBoost )
endif()

# ---------------------------------------------------------------------
# Build GoogleTest

mark_as_advanced( USE_SYSTEM_GOOGLETEST )
option( USE_SYSTEM_GOOGLETEST "Use a pre-compiled version of GoogleTest. " OFF ) 
mark_as_advanced( USE_SYSTEM_GOOGLETEST ) 
if ( SUPERELASTIX_BUILD_TESTING ) 
  if ( USE_SYSTEM_GOOGLETEST ) 
    find_package( GTest REQUIRED ) 
  else() 
    include( ExternalGoogleTest ) 
  endif() 
endif() 

# ---------------------------------------------------------------------
# Build SuperElastix

include( SuperElastix )



