# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.11)

# (Not part of the boilerplate)
# This example uses an extra component 
#set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/sensor/components)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

set(BUILD_DIR ${CMAKE_BINARY_DIR})
set(SDKCONFIG "${BUILD_DIR}/sdkconfig")
set(SDKCONFIG_DEFAULTS "sdkconfig.defaults")

project(sensor)

include (${project_dir}/html/CMakeLists.txt)

# uncommet for release builds
#add_compile_definitions(CONFIG_LOG_DISABLED)