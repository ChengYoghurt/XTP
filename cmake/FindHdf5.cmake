find_package(HDF5 REQUIRED)

if(HDF5_FOUND)
    message(STATUS "Find Hdf5: create HDF5 target interface")
    add_library(ihdf5 INTERFACE)
    target_include_directories(
        ihdf5
    INTERFACE
        ${HDF5_INCLUDE_DIRS}
    )

    target_link_libraries(
        ihdf5
    INTERFACE
        ${HDF5_LIBRARIES}
    )

    add_library(Hdf5::Hdf5 ALIAS ihdf5)
endif()