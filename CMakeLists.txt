cmake_minimum_required(VERSION 3.21)

project(UiSandbox VERSION 0.1.0 LANGUAGES CXX)

option(UI_SANDBOX_BUILD_APPS "Build applications." ON)

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

# --- Dependencies (vcpkg) ---------------------------------------------------
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Catch2 CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)

# MicroPather (no vcpkg port — fetch from GitHub)
include(FetchContent)
FetchContent_Declare(micropather
    GIT_REPOSITORY https://github.com/leethomason/MicroPather.git
    GIT_TAG        master
    SOURCE_SUBDIR  _unused)
FetchContent_MakeAvailable(micropather)
add_library(micropather STATIC ${micropather_SOURCE_DIR}/micropather.cpp)
target_include_directories(micropather PUBLIC ${micropather_SOURCE_DIR})
target_compile_features(micropather PUBLIC cxx_std_11)
set_target_properties(micropather PROPERTIES FOLDER "thirdparty")

# --- Project structure -------------------------------------------------------
add_subdirectory(src/libs)
if (UI_SANDBOX_BUILD_APPS)
    add_subdirectory(src/apps)
endif()
