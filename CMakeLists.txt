# ###########################
# Project Marklar Dataflow
#
# by Istvan "MaDog" Simon
# ###########################

#  CMake version 
cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

# Name of Dataflow Engine Library
project(MarklarDataflow VERSION 0.1.0 LANGUAGES CXX)

# CMake includes

# Set build information
# ###########################

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
# TODO :: replace with c++ fetures

# Set compiler options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe -pthread") # Threading
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror") # Warning
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -pedantic-errors") # Pedantic
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdollars-in-identifiers") # Sole $

# With switchs
# ###########################

# Dependencies
# ###########################

# System packages
find_package(Threads REQUIRED FATAL_ERROR)

# 3Rd party packages
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(3rdparty/googletest)

# Defines for CMake
# ###########################

# Setup export preprocessor definitions
# ###########################
#set(BUILD_SHARED_LIBS 1)
#if(BUILD_SHARED_LIBS)
#    target_compile_definitions(MarklarDataflow PRIVATE MARKLARDATAFLOW_EXPORTS)
#    target_compile_definitions(MarklarDataflow INTERFACE MARKLARDATAFLOW_IMPORTS)
#endif()

# Defines for files
# ###########################

# Library include dirs
set(MARKLARDATAFLOW_DIR_INCLUDE "include")

# Sources directory
set(MARKLARDATAFLOW_DIR_SOURCE "source")

# Test directory
set(MARKLARDATAFLOW_DIR_TEST  "test")

# Example directory
set(MARKLARDATAFLOW_DIR_EXAMPLE  "example")

# Include directories
# ###########################

# System
# 3Rd Party
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(3rdparty/spdlog/include) # header only library
include_directories(3rdparty/wigwag/include) # header only library
#include_directories(3rdparty/stduuid/include) # header only library
include_directories(3rdparty/sole) # header only library
# Dataflow Engine
include_directories(${MARKLARDATAFLOW_DIR_INCLUDE})

# Project files
# ###########################

# Headers files for Dataflow Engine
set(MARKLARDATAFLOW_HEADERS
    ${MARKLARDATAFLOW_DIR_INCLUDE}/attribute.hpp

    ${MARKLARDATAFLOW_DIR_INCLUDE}/command/command.hpp
    ${MARKLARDATAFLOW_DIR_INCLUDE}/command/command_passthrough.hpp
    ${MARKLARDATAFLOW_DIR_INCLUDE}/command/command_pause.hpp
    ${MARKLARDATAFLOW_DIR_INCLUDE}/command/command_start.hpp
    ${MARKLARDATAFLOW_DIR_INCLUDE}/command/command_stop.hpp
    ${MARKLARDATAFLOW_DIR_INCLUDE}/command/controller.hpp
    ${MARKLARDATAFLOW_DIR_INCLUDE}/command/dashboard.hpp

    ${MARKLARDATAFLOW_DIR_INCLUDE}/connection/connection.hpp
    ${MARKLARDATAFLOW_DIR_INCLUDE}/connection/connection_pool.hpp

    ${MARKLARDATAFLOW_DIR_INCLUDE}/data/properties.hpp

    ${MARKLARDATAFLOW_DIR_INCLUDE}/error/exception.hpp

    ${MARKLARDATAFLOW_DIR_INCLUDE}/node/node.hpp

    ${MARKLARDATAFLOW_DIR_INCLUDE}/pin/input_pin.hpp
    ${MARKLARDATAFLOW_DIR_INCLUDE}/pin/output_pin.hpp
    ${MARKLARDATAFLOW_DIR_INCLUDE}/pin/pin.hpp
#    ${MARKLARDATAFLOW_DIR_INCLUDE}/pin/signal_pin.hpp
#    ${MARKLARDATAFLOW_DIR_INCLUDE}/pin/slot_pin.hpp

    ${MARKLARDATAFLOW_DIR_INCLUDE}/policy/identifier/uuid.hpp
    ${MARKLARDATAFLOW_DIR_INCLUDE}/policy/identifier/name.hpp

    ${MARKLARDATAFLOW_DIR_INCLUDE}/policy/extender/pin.hpp

    ${MARKLARDATAFLOW_DIR_INCLUDE}/policy/scheduler/checklist.hpp
    ${MARKLARDATAFLOW_DIR_INCLUDE}/policy/scheduler/timed.hpp
    ${MARKLARDATAFLOW_DIR_INCLUDE}/policy/scheduler/trigger.hpp

    ${MARKLARDATAFLOW_DIR_INCLUDE}/thread/thread_pool.hpp
    ${MARKLARDATAFLOW_DIR_INCLUDE}/thread/thread_task.hpp

    ${MARKLARDATAFLOW_DIR_INCLUDE}/utils/string/makestring.hpp
)

# Sources files for Dataflow Engine
set(MARKLARDATAFLOW_SOURCES
    ${MARKLARDATAFLOW_DIR_SOURCE}/command/command.cpp
    ${MARKLARDATAFLOW_DIR_SOURCE}/command/command_passthrough.cpp
    ${MARKLARDATAFLOW_DIR_SOURCE}/command/command_pause.cpp
    ${MARKLARDATAFLOW_DIR_SOURCE}/command/command_start.cpp
    ${MARKLARDATAFLOW_DIR_SOURCE}/command/command_stop.cpp
    ${MARKLARDATAFLOW_DIR_SOURCE}/command/controller.cpp
    ${MARKLARDATAFLOW_DIR_SOURCE}/command/dashboard.cpp

    ${MARKLARDATAFLOW_DIR_SOURCE}/connection/connection.cpp
    ${MARKLARDATAFLOW_DIR_SOURCE}/connection/connection_pool.cpp

    ${MARKLARDATAFLOW_DIR_SOURCE}/data/properties.cpp

#    ${MARKLARDATAFLOW_DIR_SOURCE}/error/exception.cpp

    ${MARKLARDATAFLOW_DIR_SOURCE}/pin/input_pin.cpp
    ${MARKLARDATAFLOW_DIR_SOURCE}/pin/output_pin.cpp
    ${MARKLARDATAFLOW_DIR_SOURCE}/pin/pin.cpp
#    ${MARKLARDATAFLOW_DIR_SOURCE}/pin/signal_pin.cpp
#    ${MARKLARDATAFLOW_DIR_SOURCE}/pin/slot_pin.cpp

    ${MARKLARDATAFLOW_DIR_SOURCE}/policy/identifier/uuid.cpp
    ${MARKLARDATAFLOW_DIR_SOURCE}/policy/identifier/name.cpp

    ${MARKLARDATAFLOW_DIR_SOURCE}/policy/extender/pin.cpp

    ${MARKLARDATAFLOW_DIR_SOURCE}/policy/scheduler/checklist.cpp
    ${MARKLARDATAFLOW_DIR_SOURCE}/policy/scheduler/timed.cpp
    ${MARKLARDATAFLOW_DIR_SOURCE}/policy/scheduler/trigger.cpp

    ${MARKLARDATAFLOW_DIR_SOURCE}/thread/thread_pool.cpp
    ${MARKLARDATAFLOW_DIR_SOURCE}/thread/thread_task.cpp

    #    ${MARKLARDATAFLOW_DIR_SOURCE}/utils/string/makestring.cpp
)

# Sources files for Dataflow Engine Testsuit
set(MARKLARDATAFLOW_TEST_FILES
    ${MARKLARDATAFLOW_DIR_TEST}/main.cpp

#    ${MARKLARDATAFLOW_DIR_TEST}/command/gtest_command.cpp
#    ${MARKLARDATAFLOW_DIR_TEST}/command/gtest_command_executer.cpp

    ${MARKLARDATAFLOW_DIR_TEST}/connection/gtest_connection.cpp
    ${MARKLARDATAFLOW_DIR_TEST}/connection/gtest_connection_pool.cpp

    ${MARKLARDATAFLOW_DIR_TEST}/data/gtest_properties.cpp

    ${MARKLARDATAFLOW_DIR_TEST}/dataflow/gtest_multi_input.cpp
    ${MARKLARDATAFLOW_DIR_TEST}/dataflow/gtest_multi_output.cpp

    ${MARKLARDATAFLOW_DIR_TEST}/error/gtest_exception.cpp

#    ${MARKLARDATAFLOW_DIR_TEST}/node/gtest_node.cpp

    ${MARKLARDATAFLOW_DIR_TEST}/pin/gtest_pin.cpp
    ${MARKLARDATAFLOW_DIR_TEST}/pin/gtest_input_pin.cpp
    ${MARKLARDATAFLOW_DIR_TEST}/pin/gtest_output_pin.cpp
#    ${MARKLARDATAFLOW_DIR_TEST}/pin/gtest_signal_pin.cpp
#    ${MARKLARDATAFLOW_DIR_TEST}/pin/gtest_slot_pin.cpp

#    ${MARKLARDATAFLOW_DIR_TEST}/policy/id/gtest_uuid.cpp
#    ${MARKLARDATAFLOW_DIR_TEST}/policy/id/gtest_name.cpp

#    ${MARKLARDATAFLOW_DIR_TEST}/policy/pin/gtest_pin.cpp

#    ${MARKLARDATAFLOW_DIR_TEST}/policy/wait/gtest_checklist.cpp
#    ${MARKLARDATAFLOW_DIR_TEST}/policy/wait/gtest_timed.cpp
#    ${MARKLARDATAFLOW_DIR_TEST}/policy/wait/gtest_trigger.cpp

    ${MARKLARDATAFLOW_DIR_TEST}/thread/gtest_thread_pool.cpp
    ${MARKLARDATAFLOW_DIR_TEST}/thread/gtest_thread_task.cpp

    ${MARKLARDATAFLOW_DIR_TEST}/utils/string/gtest_makestring.cpp
)

# Sources files for Dir listener example
#set(MARKLARDATAFLOW_EXAMPLE_DIR_LISTENER
#    ${MARKLARDATAFLOW_DIR_LISTENER_EXAMPLE}/dir_listener/main.cpp
#)

# Build information
# ###########################

# Create Dataflow Engine library
add_library(MarklarDataflow
    ${MARKLARDATAFLOW_HEADERS}
    ${MARKLARDATAFLOW_SOURCES}
)

# Set output and Detaflow Engine libraries
target_link_libraries(MarklarDataflow)

# Set output and Dataflow Engine Test libraries
add_executable(MarklarDataflowTest  ${MARKLARDATAFLOW_TEST_FILES})
target_link_libraries(MarklarDataflowTest MarklarDataflow gtest_main)
add_test(NAME MarklarDataflowTest COMMAND MarklarDataflowTest)

# Example directory listener
#add_executable(example_dirlistener ${MARKLARDATAFLOW_EXAMPLE_DIR_LISTENER})
#target_link_libraries(example_dirlistener MarklarDataflow stdc++fs)

# Additional
add_test(MarklarDataflowGTest MarklarDataflowGTest)

# Installing
