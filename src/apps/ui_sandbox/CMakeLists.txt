add_executable(ui_sandbox main.cpp)

target_compile_features(ui_sandbox PRIVATE cxx_std_20)

target_include_directories(ui_sandbox PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(ui_sandbox PRIVATE drui)

# Copy font resources next to the executable so relative paths work.
set(FONT_RESOURCES_DIR "${CMAKE_SOURCE_DIR}/resources/fonts")
if (EXISTS "${FONT_RESOURCES_DIR}")
    add_custom_command(TARGET ui_sandbox POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:ui_sandbox>/resources/fonts"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${FONT_RESOURCES_DIR}" "$<TARGET_FILE_DIR:ui_sandbox>/resources/fonts"
        COMMENT "Copying font resources for ui_sandbox"
    )
endif()
