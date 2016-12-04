include_directories(. ../src)

add_library(abcd SHARED abcd.cpp)
set_target_properties(abcd PROPERTIES
    PREFIX lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
target_link_libraries(abcd ${PROJECT_NAME})

add_executable(test_suite test.cpp)
target_link_libraries(test_suite ${PROJECT_NAME})

add_test(NAME test COMMAND test_suite)