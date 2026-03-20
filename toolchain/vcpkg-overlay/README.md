# vcpkg overlay (ports root)

Передавай в CMake / vcpkg **каталог `ports`**, а не родительский `vcpkg-overlay`:

- Правильно: `…/toolchain/vcpkg-overlay/ports` (в `CMakePresets.json` так и задано).
- Неправильно: `…/toolchain/vcpkg-overlay` — vcpkg не увидит оверлей и возьмёт upstream `shader-slang`.

Порты:

- **`ports/shader-slang`** — сборка Slang **с GitHub** (`shader-slang/slang`, ревизия задаётся в `portfile.cmake` как `SLANG_GIT_REF`), `SLANG_LIB_TYPE=STATIC`, совместимо с **`x64-windows-static`**.

Если vcpkg долго пишет `waiting to take filesystem lock on ... vcpkg-root`, закрой другие процессы, держащие этот каталог (второй CMake, IDE, другой `vcpkg`).
