cmake_minimum_required(VERSION 3.21)

# Match vcpkg triplet x64-windows-static (/MT); linking /MT Slang into /MD exe breaks LNK2038.
if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project(UiSandbox VERSION 0.1.0 LANGUAGES CXX)

option(UI_SANDBOX_BUILD_APPS "Build applications." ON)
option(UI_SANDBOX_SHADERS_LOAD_FROM_DISK
    "Dev: load embedded post shaders from ui_sandbox_shaders/shaders on disk instead of CMRC (restart app to refresh)." OFF)
option(UI_SANDBOX_VERIFY_SLANG_SHADERS
    "Build: run slangc on each post-effect shader bundle (vertex + optional common + fragment)." ON)

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
# Required by imgui with opengl3-slang-binding
find_package(slang CONFIG REQUIRED)
find_package(gl3w CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Catch2 CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
find_package(cmakerc CONFIG REQUIRED)

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
