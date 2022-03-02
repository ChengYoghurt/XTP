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

# list and compile executable
list(APPEND target_apps "main")

if ("main" IN_LIST target_apps)
    set(target_name "main")
    add_executable(${target_name})
    target_sources(
        ${target_name}
    PUBLIC
        ${target_name}.cpp
    )
    target_include_directories(
        ${target_name}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
    target_link_libraries(
        ${target_name}
    PUBLIC
        LogConfig::LogConfig
        Threads::Threads
        Kuafu::Kuafu
    )
    install(TARGETS ${target_name} DESTINATION bin)
endif()
