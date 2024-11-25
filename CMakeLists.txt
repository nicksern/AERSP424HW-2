cmake_minimum_required(VERSION 3.23)
project(AERSP424_HW1)
set(CMAKE_CXX_STANDARD 20)

# No need for a library because we duplicated problems within each other allowing files to be run individually. 
# This, for the purposes of the assignment, isolates any potential compilation errors 
# .hpp files are automatically linked regardless, using include commands
# add_library()

# Including header files directory
include_directories(include)
include_directories(src)

# Executables for each of the standalone main functions
add_executable(Header Include/Sensor.hpp Include/SensorFactory.hpp Include/ControlSysten.hpp)
add_executable(Source src/Main.cpp)


# Since no libraries, no target links
# target_link_libraries(Q )