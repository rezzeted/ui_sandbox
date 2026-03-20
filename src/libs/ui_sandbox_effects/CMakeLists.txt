add_library(ui_sandbox_effects
    shaders.cpp
    shader_disk_io.cpp
    hotreload.cpp
    figma_gradient.cpp
    registry.cpp
    preview.cpp
)

get_filename_component(_ui_sandbox_shader_tree "${CMAKE_CURRENT_SOURCE_DIR}/../ui_sandbox_shaders/shaders" ABSOLUTE)
if(UI_SANDBOX_SHADERS_LOAD_FROM_DISK)
    # Forward slashes — std::filesystem accepts them on Windows
    file(TO_CMAKE_PATH "${_ui_sandbox_shader_tree}" UI_SANDBOX_SHADER_ROOT)
    file(TO_CMAKE_PATH "${CMAKE_BINARY_DIR}/ui_sandbox_shader_bundles" UI_SANDBOX_SHADER_BUNDLE_DIR)
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/ui_sandbox_shader_bundles")
    configure_file(
        shader_sources_config.hpp.in
        "${CMAKE_CURRENT_BINARY_DIR}/shader_sources_config.hpp"
        @ONLY
    )
    target_include_directories(ui_sandbox_effects PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
    target_compile_definitions(ui_sandbox_effects PRIVATE UI_SANDBOX_SHADERS_LOAD_FROM_DISK=1)
endif()

target_include_directories(ui_sandbox_effects PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(ui_sandbox_effects PUBLIC cxx_std_20)

target_link_libraries(ui_sandbox_effects PUBLIC
    imgui::imgui
    unofficial::gl3w::gl3w
    ui_sandbox::shaders_rc
)

if(WIN32)
    target_link_libraries(ui_sandbox_effects PUBLIC opengl32)
elseif(APPLE)
    target_compile_definitions(ui_sandbox_effects PUBLIC GL_SILENCE_DEPRECATION)
    find_library(OPENGL_LIBRARY OpenGL)
    target_link_libraries(ui_sandbox_effects PUBLIC ${OPENGL_LIBRARY})
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(ui_sandbox_effects PUBLIC OpenGL::GL)
endif()

set_target_properties(ui_sandbox_effects PROPERTIES FOLDER "libs")
