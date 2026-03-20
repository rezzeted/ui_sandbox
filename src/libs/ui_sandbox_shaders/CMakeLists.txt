# Embedded post-effect Slang sources (cmakerc / CMakeRC)
cmrc_add_resource_library(ui_sandbox_shaders_rc
    ALIAS ui_sandbox::shaders_rc
    NAMESPACE ui_sandbox_shaders
    WHENCE "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
    shaders/imgui_post_vertex.slang
    shaders/post/grayscale.slang
    shaders/post/sepia.slang
    shaders/post/pixelate.slang
    shaders/post/chromatic.slang
    shaders/post/invert.slang
    shaders/post/scanlines.slang
    shaders/post/figma_fill.slang
)

set_target_properties(ui_sandbox_shaders_rc PROPERTIES FOLDER "libs")
