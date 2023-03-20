cmake_minimum_required(VERSION 3.15)
set_property(GLOBAL PROPERTY USE_FOLDERS OFF)
project(vecy CXX)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

add_library(project_options INTERFACE)
add_library(project_warnings INTERFACE)

# target_link_libraries(project_options INTERFACE edit_and_continue)

target_compile_features(project_options INTERFACE cxx_std_17)
# set_project_warnings(project_warnings)
# enable_sanitizers(project_options)

add_library(vecy::project_options ALIAS project_options)
add_library(vecy::project_warnings ALIAS project_warnings)

add_subdirectory(external)
add_subdirectory(src)
