cmake_minimum_required(VERSION 3.16)
project(draw)

# Set output directory for built objects
# Makes sure the dynamic library goes into the build directory automatically
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

add_executable(draw cells.c main.c)

# Link to SDL3 library
target_link_libraries(draw PRIVATE SDL3::SDL3)

