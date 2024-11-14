# If you want to use CMake, then you need to reorganize your project structure.
#  - scr/       : Source files (*.cpp)
#  - include/   : Header files (*.h)
#  - CMakeLists.txt
# Then, to build you do 
#   mkdir build && cd build 
#   cmake .. 
#   make
cmake_minimum_required(VERSION 3.10)
project(serial-rt)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define build type if not defined
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Set compile options
add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    $<$<CONFIG:DEBUG>:-g>
    $<$<CONFIG:RELEASE>:-O3>
)

# Find required packages
find_package(Boost REQUIRED COMPONENTS system)
find_package(Threads REQUIRED)

# Find readline manually
find_path(READLINE_INCLUDE_DIR
    NAMES readline/readline.h
    PATHS /usr/include
          /usr/local/include
)

find_library(READLINE_LIBRARY
    NAMES readline
    PATHS /usr/lib
          /usr/local/lib
          /usr/lib/x86_64-linux-gnu
)

if(NOT READLINE_LIBRARY OR NOT READLINE_INCLUDE_DIR)
    message(FATAL_ERROR "readline library not found")
endif()

# Collect source files
file(GLOB SOURCES "src/*.cpp")

# Create executable
add_executable(serial-rt ${SOURCES})

# Set include directories
target_include_directories(serial-rt
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${Boost_INCLUDE_DIRS}
        ${READLINE_INCLUDE_DIR}
)

# Link libraries
target_link_libraries(serial-rt
    PRIVATE
        ${Boost_LIBRARIES}
        ${READLINE_LIBRARY}
        Threads::Threads
)

# Install rules (optional)
install(TARGETS serial-rt
    RUNTIME DESTINATION bin
)