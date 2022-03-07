cmake_minimum_required(VERSION 3.12)

project(KuafuTest
        VERSION 0.1
        DESCRIPTION "Kuafu test cases"
        LANGUAGES C CXX
)

if(POLICY CMP0076)
  cmake_policy(SET CMP0076 NEW)
endif()

enable_testing()

# enable c++17 globally
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Catch2 REQUIRED)

# list and compile executable
list(APPEND target_tests "KuafuTest")
list(APPEND target_tests "NumericTimeTest")

if ("KuafuTest" IN_LIST target_tests)
    set(test_name "KuafuTest")
    add_executable(${test_name})
    target_sources(${test_name} PUBLIC ${test_name}.cpp)
    target_link_libraries(${test_name} PUBLIC Catch2::Catch2 Kuafu::Kuafu)
    add_test("${test_name}" ${test_name})
endif()

if ("NumericTimeTest" IN_LIST target_tests)
    set(test_name "NumericTimeTest")
    add_executable(${test_name})
    target_sources(${test_name} PUBLIC ${test_name}.cpp)
    target_link_libraries(${test_name} PUBLIC Catch2::Catch2 Kuafu::Kuafu)
    add_test("${test_name}" ${test_name})
endif()
