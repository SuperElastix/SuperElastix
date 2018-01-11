## This file should be placed in the root directory of your project.
## Then modify the CMakeLists.txt file in the root directory of your
## project to incorporate the testing dashboard.
##
## # The following are required to submit to the CDash dashboard:
##   ENABLE_TESTING()
##   INCLUDE(CTest)

set(CTEST_PROJECT_NAME "SuperElastix")
set(CTEST_NIGHTLY_START_TIME "00:00:00 EST")

execute_process (
    COMMAND ${GIT_EXECUTABLE} name-rev --name-only HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE SELX_BRANCH_NAME 
)

set(CTEST_BUILD_NAME "${CTEST_BUILD_NAME}/${SELX_BRANCH_NAME}")

set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE "my.cdash.org")
set(CTEST_DROP_LOCATION "/submit.php?project=SuperElastix")
set(CTEST_DROP_SITE_CDASH TRUE)
