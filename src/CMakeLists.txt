cmake_minimum_required(VERSION 3.12)

project(KuafuLib
        VERSION 0.1
        DESCRIPTION "Kuafu"
        LANGUAGES C CXX
)

if(POLICY CMP0076)
  cmake_policy(SET CMP0076 NEW)
endif()

# enable c++17 globally
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(Kuafu)
target_sources(
    Kuafu
PRIVATE
    Kuafu.cpp
)
target_include_directories(
    Kuafu
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(
    Kuafu
PUBLIC
    Threads::Threads
    Boost::boost
)
add_library(Kuafu::Kuafu ALIAS Kuafu)

install(TARGETS Kuafu DESTINATION lib)
install(FILES ${CMAKE_SOURCE_DIR}/include/Kuafu.h DESTINATION include)
