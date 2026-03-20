add_library(ui_sandbox_effects
    shaders.cpp
    figma_gradient.cpp
    registry.cpp
    preview.cpp
)

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
