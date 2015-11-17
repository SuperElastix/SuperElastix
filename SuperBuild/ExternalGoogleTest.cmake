set( proj GoogleTest )

set( GTEST_TARGET_VERSION 1.7.0 )
set( GTEST_DOWNLOAD_SOURCE_HASH "2d6ec8ccdf5c46b05ba54a9fd1d130d7" )

set( GTEST_binary_dir ${CMAKE_CURRENT_BINARY_DIR}/${proj}-prefix/src/${proj}-build )
set( GTEST_source_dir ${CMAKE_CURRENT_BINARY_DIR}/${proj}-prefix/src/${proj} )
set( GTEST_install_dir ${CMAKE_CURRENT_BINARY_DIR}/${proj} )

set( ${proj}_ARCHIVE_OUTPUT_DIRECTORY "<BINARY_DIR>/lib" )
if( MSVC )
  set( ${proj}_ARCHIVE_OUTPUT_DIRECTORY "<BINARY_DIR>/lib/$<CONFIGURATION>" )
endif()

set( MSVS_ARGS )
if(MSVC_VERSION EQUAL 1700)
  set(MSVS_ARGS ${MSVS_ARGS} -D CMAKE_CXX_FLAGS=-DGTEST_HAS_TR1_TUPLE=0 ${CMAKE_CXX_FLAGS})
endif()

if( MSVC)
  set( MSVS_ARGS ${MSVS_ARGS} -D gtest_force_shared_crt:BOOL=ON )
endif()

ExternalProject_Add(${proj}
  URL http://midas3.kitware.com/midas/api/rest?method=midas.bitstream.download&checksum=${GTEST_DOWNLOAD_SOURCE_HASH}&name=swig-${GTEST_TARGET_VERSION}.zip
  URL_MD5 ${GTEST_DOWNLOAD_SOURCE_HASH}
  INSTALL_DIR ${GTEST_install_dir}
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    --no-warn-unused-cli
    ${MSVS_ARGS}
    -D BUILD_SHARED_LIBS:BOOL=OFF
    -D CMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=<BINARY_DIR>/lib
  INSTALL_COMMAND
      ${CMAKE_COMMAND} -E copy_directory ${${proj}_ARCHIVE_OUTPUT_DIRECTORY} <INSTALL_DIR>/lib
    COMMAND
      ${CMAKE_COMMAND} -E copy_directory <SOURCE_DIR>/include <INSTALL_DIR>/include
)

set( GTEST_ROOT ${GTEST_install_dir} )
list( APPEND ELASTIX_DEPENDENCIES ${proj} )

