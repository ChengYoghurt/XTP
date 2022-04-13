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

set(XTP_DIR "${CMAKE_SOURCE_DIR}/XTP/")
find_package(XTP REQUIRED)

# list and compile executable
list(APPEND target_apps "main")

if ("main" IN_LIST target_apps)
    foreach(api ${broker_apis})
    message(STATUS "Compile with AdaptedAPI::${api}")
    set(AdaptedTrader Adapted.${api})
    add_executable(${AdaptedTrader})
        target_sources(
            ${AdaptedTrader}
        PRIVATE
            # TODO 
            main.cpp
            ${CMAKE_SOURCE_DIR}/src/quote_spi.cpp
        )

    target_include_directories(
        ${AdaptedTrader}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/${api}>
        $<INSTALL_INTERFACE:include/${api}>
    )

    target_link_directories(
        ${AdaptedTrader}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/lib/WCTrader>
        $<INSTALL_INTERFACE:lib/WCTrader>
    )

    target_link_libraries(
        ${AdaptedTrader}
    PUBLIC
        LogConfig::LogConfig
        Threads::Threads
        Kuafu::Kuafu
        XTP::XTP
        WCTrader
        AdaptedApi::${api}
    )
    install(TARGETS ${AdaptedTrader} DESTINATION bin)
endif()

