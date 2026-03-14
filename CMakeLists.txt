cmake_minimum_required(VERSION 3.20)

project(UiSandbox VERSION 0.1.0 LANGUAGES CXX)

option(UI_SANDBOX_BUILD_THIRDPARTY "Build vendored third-party targets when missing." ON)
option(UI_SANDBOX_BUILD_APPS       "Build applications."                              ON)

# Basic C++ settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# IDE solution folders (Visual Studio, Xcode, etc.)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Project structure
if (UI_SANDBOX_BUILD_THIRDPARTY)
    add_subdirectory(thirdparty)
endif()
add_subdirectory(src/libs)
if (UI_SANDBOX_BUILD_APPS)
    add_subdirectory(src/apps)
endif()
