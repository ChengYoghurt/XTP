cmake_minimum_required(VERSION 3.12)

project(TradeXLib
        VERSION 0.1
        DESCRIPTION "TraderXTP"
        LANGUAGES C CXX
)

if(POLICY CMP0076)
  cmake_policy(SET CMP0076 NEW)
endif()

# enable c++17 globally
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(XTP_DIR "${CMAKE_SOURCE_DIR}/XTP/")
set(XTP_Algo_DIR "${CMAKE_SOURCE_DIR}/XTP_Algo/")
find_package(XTP REQUIRED)
find_package(XTP_Algo REQUIRED)
find_package(spdlog REQUIRED)

# start build AdaptedApi::broker_apis lib
list(APPEND broker_apis "XTP")
set(broker_apis ${broker_apis} PARENT_SCOPE)
message("Hi I'm AdaptedAPI::${broker_apis}")
if ("XTP" IN_LIST broker_apis)
    set(BrokerApi "XTP")
    set(AdaptedApi Adapted${BrokerApi})
    add_library(${AdaptedApi})
    target_include_directories(
        ${AdaptedApi}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/Brokers/${BrokerApi}>
    )
    target_sources(
        ${AdaptedApi}
    PRIVATE
        ${BrokerApi}/AdaptedApi.cpp
    )
    target_link_libraries(
        ${AdaptedApi}
    PUBLIC
        XTP::XTP
        spdlog::spdlog
    )
    add_library(AdaptedApi::${BrokerApi} ALIAS ${AdaptedApi})
endif()

list(APPEND broker_apis "XTP_Algo")
set(broker_apis ${broker_apis} PARENT_SCOPE)
if ("XTP_Algo" IN_LIST broker_apis)
    set(BrokerApi "XTP_Algo")
    message("Hi I'm AdaptedAPI::${BrokerApi}")
    set(AdaptedApi Adapted${BrokerApi})
    add_library(${AdaptedApi})
    target_include_directories(
        ${AdaptedApi}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/Brokers/${BrokerApi}>
    )
    target_sources(
        ${AdaptedApi}
    PRIVATE
        ${BrokerApi}/AdaptedApi.cpp
    )
    target_link_libraries(
        ${AdaptedApi}
    PUBLIC
        XTP_Algo::XTP_Algo
        spdlog::spdlog
    )
    add_library(AdaptedApi::${BrokerApi} ALIAS ${AdaptedApi})
endif()
