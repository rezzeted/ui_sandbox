# Overlay: build shader-slang from GitHub at a pinned commit (SLANG_LIB_TYPE=STATIC, x64-windows-static /MT).
#
# vcpkg_from_git uses `git archive`, which does not include git submodules — Slang's external/* would be empty.
# We shallow-fetch the commit and run `submodule update` instead.

# slang-glslang / slang-glsl-module may still produce DLLs next to static compiler.lib
set(VCPKG_POLICY_DLLS_IN_STATIC_LIBRARY enabled)

# Pin bump: change SHA to move to another upstream revision.
set(SLANG_GIT_URL "https://github.com/shader-slang/slang.git")
set(SLANG_GIT_REF "80c30664c1250bd1ae94ea2c24e301968f1e09bf")

set(SOURCE_PATH "${CURRENT_BUILDTREES_DIR}/src/shader-slang-git")

vcpkg_find_acquire_program(GIT)

# Marker file: miniz is always a submodule under external/
set(SLANG_SUBMODULE_STAMP "${SOURCE_PATH}/external/miniz/CMakeLists.txt")
if(NOT EXISTS "${SLANG_SUBMODULE_STAMP}")
    file(REMOVE_RECURSE "${SOURCE_PATH}")
    file(MAKE_DIRECTORY "${SOURCE_PATH}")
    message(STATUS "Fetching ${SLANG_GIT_URL} @ ${SLANG_GIT_REF} (shallow, with submodules)...")
    vcpkg_execute_required_process(
        ALLOW_IN_DOWNLOAD_MODE
        COMMAND "${GIT}" -c core.autocrlf=false init
        WORKING_DIRECTORY "${SOURCE_PATH}"
        LOGNAME "shader-slang-git-init"
    )
    vcpkg_execute_required_process(
        ALLOW_IN_DOWNLOAD_MODE
        COMMAND "${GIT}" -c core.autocrlf=false remote add origin "${SLANG_GIT_URL}"
        WORKING_DIRECTORY "${SOURCE_PATH}"
        LOGNAME "shader-slang-git-remote"
    )
    vcpkg_execute_required_process(
        ALLOW_IN_DOWNLOAD_MODE
        COMMAND "${GIT}" -c core.autocrlf=false fetch --depth 1 origin "${SLANG_GIT_REF}"
        WORKING_DIRECTORY "${SOURCE_PATH}"
        LOGNAME "shader-slang-git-fetch"
    )
    vcpkg_execute_required_process(
        ALLOW_IN_DOWNLOAD_MODE
        COMMAND "${GIT}" -c core.autocrlf=false -c advice.detachedHead=false checkout FETCH_HEAD
        WORKING_DIRECTORY "${SOURCE_PATH}"
        LOGNAME "shader-slang-git-checkout"
    )
    vcpkg_execute_required_process(
        ALLOW_IN_DOWNLOAD_MODE
        COMMAND "${GIT}" -c core.autocrlf=false submodule update --init --recursive
        WORKING_DIRECTORY "${SOURCE_PATH}"
        LOGNAME "shader-slang-git-submodule"
    )
    vcpkg_apply_patches(
        SOURCE_PATH "${SOURCE_PATH}"
        PATCHES
            0002-slangc-no-memleak-assert.patch
    )
endif()

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        "-DGIT_EXECUTABLE=${GIT}"
        # If only SLANG_VERSION_FULL is set, get_git_version() skips work and leaves
        # SLANG_VERSION_NUMERIC unset → VERSION "0." breaks set_target_properties.
        -DSLANG_VERSION_NUMERIC=0.0.0
        -DSLANG_VERSION_FULL=${SLANG_GIT_REF}
        -DSLANG_LIB_TYPE=STATIC
        -DSLANG_ENABLE_TESTS=OFF
        -DSLANG_ENABLE_EXAMPLES=OFF
        -DSLANG_ENABLE_SLANG_RHI=OFF
        -DSLANG_ENABLE_GFX=OFF
        -DSLANG_ENABLE_SLANGD=OFF
        -DSLANG_ENABLE_REPLAYER=OFF
        -DSLANG_ENABLE_PREBUILT_BINARIES=OFF
        -DSLANG_SLANG_LLVM_FLAVOR=DISABLE
        -DSLANG_ENABLE_CUDA=OFF
        -DSLANG_ENABLE_OPTIX=OFF
)

vcpkg_cmake_install(ADD_BIN_TO_PATH)

# Upstream install rules only publish slang-compiler + slang-rt; static consumers need the full .lib set.
function(z_shader_slang_install_libs cfg_suffix cmake_cfg dest_lib_dir)
    set(_libdir "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-${cfg_suffix}/${cmake_cfg}/lib")
    if(EXISTS "${_libdir}")
        file(GLOB _libs "${_libdir}/*.lib")
        foreach(_lib IN LISTS _libs)
            get_filename_component(_name "${_lib}" NAME)
            # Skip generators-only helper; everything else is linked for a full static graph.
            if(NOT _name STREQUAL "slang-without-embedded-core-module.lib")
                file(INSTALL "${_lib}" DESTINATION "${dest_lib_dir}")
            endif()
        endforeach()
    endif()
endfunction()

z_shader_slang_install_libs(rel Release "${CURRENT_PACKAGES_DIR}/lib")
z_shader_slang_install_libs(dbg Debug "${CURRENT_PACKAGES_DIR}/debug/lib")

function(z_shader_slang_install_external_libs cfg_suffix cmake_cfg dest_lib_dir)
    set(_bt "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-${cfg_suffix}")
    foreach(
        _relpath
        IN
        ITEMS
        external/cmark/src/cmark-gfm.lib
        external/miniz/miniz.lib
        external/lz4/build/cmake/lz4.lib
    )
        set(_extlib "${_bt}/${_relpath}")
        if(EXISTS "${_extlib}")
            file(INSTALL "${_extlib}" DESTINATION "${dest_lib_dir}")
        endif()
    endforeach()
endfunction()

z_shader_slang_install_external_libs(rel Release "${CURRENT_PACKAGES_DIR}/lib")
z_shader_slang_install_external_libs(dbg Debug "${CURRENT_PACKAGES_DIR}/debug/lib")

# Slang installs slangc/slangi to bin/; export files expect tools/shader-slang/ (as in curated port).
file(MAKE_DIRECTORY "${CURRENT_PACKAGES_DIR}/tools/shader-slang")
foreach(sl_tool IN ITEMS slangc slangi)
    if(EXISTS "${CURRENT_PACKAGES_DIR}/bin/${sl_tool}.exe")
        file(COPY "${CURRENT_PACKAGES_DIR}/bin/${sl_tool}.exe"
            DESTINATION "${CURRENT_PACKAGES_DIR}/tools/shader-slang")
    endif()
endforeach()

vcpkg_copy_pdbs()

if(VCPKG_TARGET_IS_WINDOWS)
    vcpkg_cmake_config_fixup(CONFIG_PATH cmake PACKAGE_NAME slang)
else()
    vcpkg_cmake_config_fixup(PACKAGE_NAME slang)
endif()

# STATIC build: no install(EXPORT), but slangConfig.cmake includes slangTargets.cmake.
file(
    INSTALL "${CMAKE_CURRENT_LIST_DIR}/slangTargets.cmake"
    DESTINATION "${CURRENT_PACKAGES_DIR}/share/slang"
)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")

if(EXISTS "${SOURCE_PATH}/LICENSE")
    vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
endif()
