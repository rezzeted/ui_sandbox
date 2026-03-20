# UI Sandbox

Standalone ImGui application for prototyping and testing UI components:
panels, menus, tabs, buttons, inputs, sliders, checkboxes, color pickers,
status bar, canvas with pan/zoom, and more.

Built with **DrUI** — a small widget/theme library on top of Dear ImGui.

## Building (Windows, Visual Studio)

All scripts use the **`_build`** directory.

По умолчанию vcpkg:

- triplet **`x64-windows-static`** (готовый triplet vcpkg: статические `.lib`, CRT **`/MT`**).
- **`VCPKG_OVERLAY_PORTS`** = **`toolchain/vcpkg-overlay/ports`** — каталог должен заканчиваться на **`/ports`** (внутри лежит `shader-slang/`), иначе vcpkg подставит upstream-порт.
- зависимость **`shader-slang`** через overlay собирается **с GitHub** (`shader-slang/slang`, зафиксированный SHA в `toolchain/vcpkg-overlay/ports/shader-slang/portfile.cmake`), **`SLANG_LIB_TYPE=STATIC`**.

Чтобы сменить версию Slang, отредактируй **`SLANG_GIT_REF`** (и при необходимости **`SLANG_GIT_URL`**) в этом `portfile.cmake`, затем пересобери порт / удали кэш vcpkg для `shader-slang`.

Зависимости: `_build\vcpkg_installed\x64-windows-static\`. Первая сборка Slang может занять много времени.

```bat
:: Quick build (Debug)
build_vs.bat

:: Presets (CMake ≥ 3.21)
cmake --preset vs2026
cmake --build --preset debug
```

Вручную:

```bat
cmake -S . -B _build -G "Visual Studio 18 2026" -A x64 ^
  -DCMAKE_TOOLCHAIN_FILE=toolchain/vcpkg/scripts/buildsystems/vcpkg.cmake ^
  -DVCPKG_OVERLAY_PORTS=%CD%\toolchain\vcpkg-overlay\ports ^
  -DVCPKG_TARGET_TRIPLET=x64-windows-static
cmake --build _build --config Debug
```

После смены triplet лучше пересоздать `_build`, чтобы не смешивать `vcpkg_installed\…`.

Output: `_build\bin\Debug\ui_sandbox.exe`. Для **x86** придётся завести аналогичный triplet/overlay под свою архитектуру.

Без форка и без overlay по-прежнему можно использовать upstream-порт `shader-slang` (бинарники DLL) и triplet вроде **`x64-windows-static-md`**, задав `VCPKG_TARGET_TRIPLET` и убрав `VCPKG_OVERLAY_PORTS` из вызова CMake.

## Project structure

```
src/
├── apps/
│   └── ui_sandbox/      — main application (entry point, panels, layout)
└── libs/
    └── drui/            — DrUI widget/theme library
thirdparty/              — imgui, glfw, spdlog, catch2, nlohmann_json, immer, magic_enum
resources/
└── fonts/               — Font Awesome and other font assets (optional)
```

## Dependencies

- **ImGui** (with GLFW + OpenGL3 backends)
- **GLFW**
- C++20, CMake 3.20+
