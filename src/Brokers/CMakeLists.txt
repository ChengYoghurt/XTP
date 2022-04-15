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

find_package(TradeX REQUIRED)
find_package(spdlog REQUIRED)

#list(APPEND broker_apis "Fake")
list(APPEND broker_apis "XTP")
set(broker_apis ${broker_apis} PARENT_SCOPE)

if ("XTP" IN_LIST broker_apis)
    set(BrokerApi "XTP")
    set(AdaptedApi Adapted${BrokerApi})
    add_library(${AdaptedApi})
    target_include_directories(
        ${AdaptedApi}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/${BrokerApi}>
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
