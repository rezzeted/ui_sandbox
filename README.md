# UI Sandbox

Standalone ImGui application for prototyping and testing UI components:
panels, menus, tabs, buttons, inputs, sliders, checkboxes, color pickers,
status bar, canvas with pan/zoom, and more.

Built with **DrUI** — a small widget/theme library on top of Dear ImGui.

## Building (Windows, Visual Studio)

All scripts use the **`_build`** directory.

```bat
:: Quick build (Debug)
build_vs.bat

:: Manual CMake
cmake -S . -B _build -G "Visual Studio 17 2022" -A x64
cmake --build _build --config Debug
```

Output: `_build\bin\Debug\ui_sandbox.exe`.

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
