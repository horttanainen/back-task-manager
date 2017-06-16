cmake_minimum_required(VERSION 3.6.2)

project(CppBack)

if(dev_build)
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")

    include(cmake/DownloadProject.cmake)
    download_project(PROJ                googletest
        GIT_REPOSITORY      https://github.com/google/googletest.git
        GIT_TAG             master
        ${UPDATE_DISCONNECTED_IF_AVAILABLE}
    )

    # Prevent GoogleTest from overriding our compiler/linker options
    # when building with Visual Studio
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    add_subdirectory(
        ${googletest_SOURCE_DIR}
        ${googletest_BINARY_DIR}
    )
endif(dev_build)

add_subdirectory(
    cppback
)

if(dev_build)
    add_subdirectory(
        test
    )
endif()
