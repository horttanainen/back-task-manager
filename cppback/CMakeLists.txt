cmake_minimum_required(VERSION 3.6.2)

project(cppback)

file(
    GLOB SOURCE_FILES
    *.h
    *.cpp
    )

add_library(
    ${PROJECT_NAME}
    ${SOURCE_FILES}
    )

source_group("Src" FILES ${SOURCE_FILES})

set(
    ${PROJECT_NAME}_INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/..
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE
    )

