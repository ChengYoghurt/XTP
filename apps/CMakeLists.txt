cmake_minimum_required(VERSION 3.12)

project(KuafuApps
        VERSION 0.1
        DESCRIPTION "Kuafu apps"
        LANGUAGES C CXX
)

if(POLICY CMP0076)
  cmake_policy(SET CMP0076 NEW)
endif()

# enable c++17 globally
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# add directory of FindHdf5.cmake
include(${CMAKE_SOURCE_DIR}/cmake/FindHdf5.cmake)
#set(AdaptedApi::XTP_DIR "${CMAKE_SOURCE_DIR}/src/Brokers")
#find_package(AdaptedApi::XTP REQUIRED)
# list and compile executable
list(APPEND target_apps "main")

if ("main" IN_LIST target_apps)
    set(target_name "main")
    add_executable(${target_name})
    target_sources(
        ${target_name}
    PRIVATE
        ${target_name}.cpp
        ${CMAKE_SOURCE_DIR}/src/quote_spi.cpp
    )

    target_include_directories(
        ${target_name}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/${api}>
        $<INSTALL_INTERFACE:include/${api}>
    )

    target_link_directories(
        ${target_name}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/lib/WCTrader>
        $<INSTALL_INTERFACE:lib/WCTrader>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/Brokers>
        $<INSTALL_INTERFACE:src/Brokers>
    )

    target_link_libraries(
        ${target_name}
    PUBLIC
        LogConfig::LogConfig
        Threads::Threads
        Kuafu::Kuafu
        #XTP::XTP
        WCTrader
        AdaptedApi::XTP
        #AdaptedApi::${api}
    )
    install(TARGETS ${target_name} DESTINATION bin)
endif()

