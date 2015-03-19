## This file should be placed in the root directory of your project.
## Then modify the CMakeLists.txt file in the root directory of your
## project to incorporate the testing dashboard.
## # The following are required to uses Dart and the Cdash dashboard
##   ENABLE_TESTING()
##   INCLUDE(CTest)
set(CTEST_PROJECT_NAME "Novastory")
set(CTEST_NIGHTLY_START_TIME "00:00:00 UTC")

set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE "cdash.draftup.org")
set(CTEST_DROP_LOCATION "/submit.php?project=Novastory")
set(CTEST_DROP_SITE_CDASH TRUE)

# should ctest wipe the binary tree before running
SET (CTEST_START_WITH_EMPTY_BINARY_DIRECTORY_ONCE TRUE)

# specify how long to run the continuous in minutes
SET (CTEST_CONTINUOUS_DURATION 30)
SET (CTEST_CONTINUOUS_MINIMUM_INTERVAL 10)
