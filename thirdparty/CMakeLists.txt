# Third-party libraries CMake configuration

# ==============================================================================
# GLFW
# ==============================================================================
if (NOT TARGET glfw)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

    add_subdirectory(glfw)
    set_target_properties(glfw PROPERTIES FOLDER "thirdparty")
endif()

# ==============================================================================
# ImGui (static library with GLFW + OpenGL3 backends)
# ==============================================================================
if (NOT TARGET imgui)
    set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)

    add_library(imgui STATIC
        # Core ImGui sources
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui_demo.cpp

        # Backend implementations (GLFW + OpenGL3)
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    )

    target_include_directories(imgui PUBLIC
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
    )

    # ImGui uses its embedded OpenGL3 loader (imgui_impl_opengl3_loader.h)
    # No external gl3w/glad needed - just let ImGui autodetect

    # Link GLFW for the backend
    target_link_libraries(imgui PUBLIC
        glfw
    )

    # OpenGL is needed at link time by consumers
    if(WIN32)
        target_link_libraries(imgui PUBLIC opengl32)
    elseif(APPLE)
        target_compile_definitions(imgui PUBLIC GL_SILENCE_DEPRECATION)
        find_library(OPENGL_LIBRARY OpenGL)
        target_link_libraries(imgui PUBLIC ${OPENGL_LIBRARY})
    else()
        find_package(OpenGL REQUIRED)
        target_link_libraries(imgui PUBLIC OpenGL::GL)
    endif()
    set_target_properties(imgui PROPERTIES FOLDER "thirdparty")
endif()

# ==============================================================================
# spdlog (logging)
# ==============================================================================
if (NOT TARGET spdlog::spdlog)
    set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "" FORCE)
    set(SPDLOG_BUILD_PIC OFF CACHE BOOL "" FORCE)
    set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
    set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)

    add_subdirectory(spdlog)
    set_target_properties(spdlog spdlog_header_only PROPERTIES FOLDER "thirdparty")
endif()

# ==============================================================================
# Catch2 (unit testing)
# ==============================================================================
if (NOT TARGET Catch2::Catch2)
    set(CATCH_BUILD_TESTING OFF CACHE BOOL "" FORCE)
    set(CATCH_INSTALL_DOCS OFF CACHE BOOL "" FORCE)
    set(CATCH_INSTALL_EXTRAS OFF CACHE BOOL "" FORCE)
    set(CATCH_DEVELOPMENT_BUILD OFF CACHE BOOL "" FORCE)

    add_subdirectory(catch2)
    set_target_properties(Catch2 Catch2WithMain PROPERTIES FOLDER "thirdparty")
endif()

# ==============================================================================
# nlohmann::json (single-header)
# ==============================================================================
if (NOT TARGET nlohmann_json)
    add_library(nlohmann_json INTERFACE)
    target_include_directories(nlohmann_json INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/nlohmann_json/single_include
    )
    set_target_properties(nlohmann_json PROPERTIES FOLDER "thirdparty")
endif()


# ==============================================================================
# magic_enum (header-only, for enum_name in debug dump)
# - Prefer find_package (vcpkg / host) when available.
# - Fallback: vendored submodule at thirdparty/magic_enum.
# ==============================================================================
if (NOT TARGET magic_enum::magic_enum)
    find_package(magic_enum CONFIG QUIET)
    if (NOT magic_enum_FOUND)
        set(MAGIC_ENUM_OPT_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(MAGIC_ENUM_OPT_BUILD_TESTS OFF CACHE BOOL "" FORCE)
        add_subdirectory(magic_enum)
        set_target_properties(magic_enum PROPERTIES FOLDER "thirdparty")
    endif()
endif()

# ==============================================================================
# MicroPather (A* pathfinding)
# - No CMake upstream; fetch and build the single .cpp manually.
# ==============================================================================
if (NOT TARGET micropather)
    include(FetchContent)
    FetchContent_Declare(
        micropather
        GIT_REPOSITORY https://github.com/leethomason/MicroPather.git
        GIT_TAG        master
        SOURCE_SUBDIR  _unused
    )
    FetchContent_MakeAvailable(micropather)
    add_library(micropather STATIC ${micropather_SOURCE_DIR}/micropather.cpp)
    target_include_directories(micropather PUBLIC ${micropather_SOURCE_DIR})
    target_compile_features(micropather PUBLIC cxx_std_11)
    set_target_properties(micropather PROPERTIES FOLDER "thirdparty")
endif()
