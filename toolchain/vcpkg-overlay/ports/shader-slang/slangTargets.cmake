# Static-install supplement: upstream Slang does not `install(EXPORT …)` when SLANG_LIB_TYPE=STATIC,
# but slangConfig.cmake still includes slangTargets.cmake. This file defines `slang::slang`.

get_filename_component(_slang_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)

if(NOT TARGET slang::slang)
    add_library(slang::slang STATIC IMPORTED GLOBAL)
endif()

set_property(TARGET slang::slang APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_property(TARGET slang::slang APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_property(TARGET slang::slang APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_property(TARGET slang::slang APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

set_target_properties(slang::slang PROPERTIES
    IMPORTED_LOCATION_DEBUG "${_slang_root}/debug/lib/slang-compiler.lib"
    IMPORTED_LOCATION_RELEASE "${_slang_root}/lib/slang-compiler.lib"
    IMPORTED_LOCATION_RELWITHDEBINFO "${_slang_root}/lib/slang-compiler.lib"
    IMPORTED_LOCATION_MINSIZEREL "${_slang_root}/lib/slang-compiler.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${_slang_root}/include"
    INTERFACE_COMPILE_DEFINITIONS SLANG_STATIC
)

include(CMakeFindDependencyMacro)
find_dependency(Threads)

# MSVC static link order: pull cpp-parser / compiler-core / core / glsl after slang-compiler.
target_link_libraries(slang::slang INTERFACE
    Threads::Threads
    "$<$<CONFIG:Debug>:${_slang_root}/debug/lib/slang-cpp-parser.lib>"
    "$<$<NOT:$<CONFIG:Debug>>:${_slang_root}/lib/slang-cpp-parser.lib>"
    "$<$<CONFIG:Debug>:${_slang_root}/debug/lib/compiler-core.lib>"
    "$<$<NOT:$<CONFIG:Debug>>:${_slang_root}/lib/compiler-core.lib>"
    "$<$<CONFIG:Debug>:${_slang_root}/debug/lib/core.lib>"
    "$<$<NOT:$<CONFIG:Debug>>:${_slang_root}/lib/core.lib>"
    "$<$<CONFIG:Debug>:${_slang_root}/debug/lib/cmark-gfm.lib>"
    "$<$<NOT:$<CONFIG:Debug>>:${_slang_root}/lib/cmark-gfm.lib>"
    "$<$<CONFIG:Debug>:${_slang_root}/debug/lib/miniz.lib>"
    "$<$<NOT:$<CONFIG:Debug>>:${_slang_root}/lib/miniz.lib>"
    "$<$<CONFIG:Debug>:${_slang_root}/debug/lib/lz4.lib>"
    "$<$<NOT:$<CONFIG:Debug>>:${_slang_root}/lib/lz4.lib>"
    "$<$<CONFIG:Debug>:${_slang_root}/debug/lib/slang-glsl-module.lib>"
    "$<$<NOT:$<CONFIG:Debug>>:${_slang_root}/lib/slang-glsl-module.lib>"
    "$<$<CONFIG:Debug>:${_slang_root}/debug/lib/slang-glslang.lib>"
    "$<$<NOT:$<CONFIG:Debug>>:${_slang_root}/lib/slang-glslang.lib>"
)
