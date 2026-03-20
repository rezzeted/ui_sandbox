add_subdirectory(drui)
add_subdirectory(ui_sandbox_shaders)
add_subdirectory(ui_sandbox_effects)

set_target_properties(drui ui_sandbox_shaders_rc ui_sandbox_effects PROPERTIES FOLDER "libs")
