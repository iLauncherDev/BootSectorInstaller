##
## PROJECT:     BootSectorInstaller
## LICENSE:     MIT (https://spdx.org/licenses/MIT)
## PURPOSE:     The central configuration file for a CMake project
## COPYRIGHT:   Copyright 2024 Daniel Victor <ilauncherdeveloper@gmail.com>
##

cmake_minimum_required(VERSION 3.20)

project(bootsector-installer C)

include_directories(include)

set(C_FLAGS "-Wall -Werror")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${C_FLAGS}")

file(GLOB_RECURSE C_FILES "src/*.c")

add_executable(${PROJECT_NAME} ${C_FILES})

foreach(SOURCE ${C_FILES})
    get_filename_component(ABS_PATH "${SOURCE}" ABSOLUTE)
    get_filename_component(PARENT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" DIRECTORY)

    file(RELATIVE_PATH REL_PATH "${PARENT_DIR}" "${ABS_PATH}")

    set_source_files_properties(
        ${SOURCE} PROPERTIES
        COMPILE_DEFINITIONS "__RELATIVE_FILE__=\"${REL_PATH}\""
    )
endforeach()
