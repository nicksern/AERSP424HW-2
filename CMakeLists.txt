cmake_minimum_required(VERSION 3.10)
project(MyProject)

# Set C++ standard (adjust as needed)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directory
include_directories(include)

# Find all source files in the 'src' folder
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Add the executable (change 'main' to your main source file's name if different)
add_executable(MyProject ${Problem1})
add_executable(MyProject ${Problem2})
add_executable(MyProject ${Problem3})
add_executable(MyProject ${Problem4})

# Find and link Matplot++ (assuming it's installed system-wide)
find_package(Matplot++ REQUIRED)
target_link_libraries(MyProject Matplot++::matplot)

# Optional: Compiler/linker flags (if needed)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")