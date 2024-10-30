cmake_minimum_required(VERSION 3.10)

# Set the project name
project(WordleSolver VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(APPLICATION_LIBRARY lib)

# Add the executable
add_executable(WordleSolver main.cpp)

# Include directories
add_subdirectory(${APPLICATION_LIBRARY})
include_directories(${APPLICATION_LIBRARY})

# Link libraries
target_link_libraries(WordleSolver ${APPLICATION_LIBRARY})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res/
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/res/)

# Add tests
enable_testing()
add_subdirectory(tests)
