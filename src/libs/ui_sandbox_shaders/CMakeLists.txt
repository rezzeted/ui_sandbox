include("${CMAKE_CURRENT_LIST_DIR}/shader_manifest.cmake")

# CMRC inputs: vertex + common + all post fragments
set(_cmrc_shader_inputs
    shaders/imgui_post_vertex.slang
    shaders/post/_post_common.slang
)
foreach(_n IN LISTS UI_SANDBOX_SHADER_POST_EFFECTS_WITH_COMMON)
    list(APPEND _cmrc_shader_inputs "shaders/post/${_n}.slang")
endforeach()
foreach(_n IN LISTS UI_SANDBOX_SHADER_POST_EFFECTS_STANDALONE)
    list(APPEND _cmrc_shader_inputs "shaders/post/${_n}.slang")
endforeach()

cmrc_add_resource_library(ui_sandbox_shaders_rc
    ALIAS ui_sandbox::shaders_rc
    NAMESPACE ui_sandbox_shaders
    WHENCE "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
    ${_cmrc_shader_inputs}
)

set_target_properties(ui_sandbox_shaders_rc PROPERTIES FOLDER "libs")

# --- Optional: compile-check bundles with slangc (same concat as runtime)
if(UI_SANDBOX_VERIFY_SLANG_SHADERS AND SLANGC_EXECUTABLE)
    set(_v "${CMAKE_CURRENT_SOURCE_DIR}/shaders/imgui_post_vertex.slang")
    set(_c "${CMAKE_CURRENT_SOURCE_DIR}/shaders/post/_post_common.slang")
    set(_pfx "${CMAKE_CURRENT_BINARY_DIR}/_slang_verify")
    file(MAKE_DIRECTORY "${_pfx}")
    set(_verify_stamps "")

    macro(_ui_sandbox_stamp name use_common frag_relpath)
        set(_frag "${CMAKE_CURRENT_SOURCE_DIR}/shaders/${frag_relpath}")
        set(_out "${_pfx}/${name}.bundle.slang")
        set(_s "${_pfx}/${name}.stamp")
        set(_d "${_v};${_frag};${CMAKE_SOURCE_DIR}/cmake/SlangVerifyCombo.cmake")
        if("${use_common}" STREQUAL "1")
            list(APPEND _d "${_c}")
        endif()
        add_custom_command(
            OUTPUT "${_s}"
            COMMAND ${CMAKE_COMMAND}
                "-DOUT=${_out}"
                "-DVERTEX=${_v}"
                "-DCOMMON=${_c}"
                "-DFRAG=${_frag}"
                "-DSLANGC=${SLANGC_EXECUTABLE}"
                "-DUSE_COMMON=${use_common}"
                -P "${CMAKE_SOURCE_DIR}/cmake/SlangVerifyCombo.cmake"
            COMMAND ${CMAKE_COMMAND} -E touch "${_s}"
            DEPENDS ${_d}
            COMMENT "Slang verify: ${name}"
            VERBATIM
        )
        list(APPEND _verify_stamps "${_s}")
    endmacro()

    foreach(_n IN LISTS UI_SANDBOX_SHADER_POST_EFFECTS_WITH_COMMON)
        _ui_sandbox_stamp("${_n}" 1 "post/${_n}.slang")
    endforeach()
    foreach(_n IN LISTS UI_SANDBOX_SHADER_POST_EFFECTS_STANDALONE)
        _ui_sandbox_stamp("${_n}" 0 "post/${_n}.slang")
    endforeach()

    add_custom_target(ui_sandbox_verify_shaders ALL DEPENDS ${_verify_stamps})
    set_target_properties(ui_sandbox_verify_shaders PROPERTIES FOLDER "libs")
elseif(UI_SANDBOX_VERIFY_SLANG_SHADERS)
    message(WARNING "UI_SANDBOX_VERIFY_SLANG_SHADERS is ON but slangc not found (SLANGC_EXECUTABLE empty).")
endif()
