add_executable(sample_editor main.cpp)

target_compile_features(sample_editor PRIVATE cxx_std_20)

target_include_directories(sample_editor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(sample_editor PRIVATE drui)

# Copy font resources next to the executable so relative paths work.
set(FONT_RESOURCES_DIR "${CMAKE_SOURCE_DIR}/resources/fonts")
if (EXISTS "${FONT_RESOURCES_DIR}")
    add_custom_command(TARGET sample_editor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:sample_editor>/resources/fonts"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${FONT_RESOURCES_DIR}" "$<TARGET_FILE_DIR:sample_editor>/resources/fonts"
        COMMENT "Copying font resources for sample_editor"
    )
endif()
