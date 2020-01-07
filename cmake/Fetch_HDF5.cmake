include(${PROJECT_SOURCE_DIR}/cmake/FindPackageHDF5.cmake)

if(NOT TARGET hdf5::hdf5)
    find_package_hdf5()
    if(TARGET hdf5::hdf5)
        set(HDF5_FOUND TRUE)
    elseif (DOWNLOAD_METHOD MATCHES "native")
        message(STATUS "HDF5 will be installed into ${CMAKE_INSTALL_PREFIX}")
        include(${PROJECT_SOURCE_DIR}/cmake/BuildDependency.cmake)
        build_dependency(hdf5 "${hdf5_install_prefix}" "" "")
        set(HDF5_ROOT ${hdf5_install_prefix})
        find_package_hdf5()
        if(TARGET hdf5::hdf5)
            message(STATUS "hdf5 installed successfully: ${HDF5_BUILD_DIR} ${HDF5_CXX_INCLUDE_DIRS} ${HDF5_hdf5_LIBRARY}")
        else()
            message(FATAL_ERROR "hdf5 could not be downloaded.")
        endif()
    else()
        message("WARNING: Dependency HDF5 not found in your system. Set DOWNLOAD_METHOD to one of 'conan|native'")
    endif()

endif()
