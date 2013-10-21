do_test(MultiThreadWeb test-MultiThreadWeb.cpp)
do_test(RawFileHandler test-RawFileHandler.cpp)
append_test_resource(RawFileHandler ${CMAKE_SOURCE_DIR}/tests/test-RawFileHandler.testfile)
