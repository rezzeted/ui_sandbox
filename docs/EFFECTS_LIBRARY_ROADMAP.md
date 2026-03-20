# Библиотека `ui_sandbox_effects` — границы и план наполнения

Цель: держать **демо-шейдеры**, **регистрацию в `EffectSystem`** и **утилиты Figma-градиента** отдельно от приложения `ui_sandbox`, чтобы другие приложения могли подключать один и тот же набор без копирования строк.

## Текущее состояние

| Компонент | Путь | Назначение |
|-----------|------|------------|
| Публичный вход | `src/libs/ui_sandbox_effects/include/ui_sandbox_effects/ui_sandbox_effects.hpp` | Единая точка `#include` |
| Slang-тексты | `shaders.hpp` / `shaders.cpp`, `builtin_effect_specs.hpp`, `shader_manifest.cmake` | Исходники в **`src/libs/ui_sandbox_shaders/shaders/`**; список фрагментов для CMRC/verify — **`shader_manifest.cmake`**; метаданные эффектов — **`kBuiltinEffectSpecs`**. Режим диска: bundle + **`TickAutoReload`** (см. `docs/EFFECTS.md`). |
| Реестр | `registry.hpp` / `registry.cpp` | `register_builtin_effects(EffectSystem&)` |
| Figma | `figma.hpp`, `figma_gradient.cpp` | UBO, палитра GL, affine UV |
| Превью | `preview.hpp` / `preview.cpp` | `draw_effect_preview_quads` |

Пространство имён: **`ui_sandbox::effects`** (в приложении удобен алиас `namespace fx = ui_sandbox::effects`).

## Границы зависимостей

- **Внутри библиотеки допустимо:** `imgui::imgui` (в т.ч. заголовки с `ImGuiRenderUX` / `imgui_effect_helpers.h`), `gl3w`, OpenGL.
- **Снаружи остаётся приложение:** GLFW, оркестрация кадра (`AdvanceFrame`, `NotifyAfterImGuiNewFrame`, `EffectSubmitGuard`), layout/UI (`drui`), загрузка шрифтов.
- **Форк ImGui + Slang-бэкенд** по-прежнему единственный источник контракта EffectSystem; изменения API — через обновление порта vcpkg, не через дублирование типов в библиотеке. См. **`docs/EFFECTS.md`**.

## Краткий бэклог (наполнение)

1. **Эффекты**
   - Классика UI: vignette, sharpen/unsharp, blur (отдельным пассом или аппроксимация в одном), CRT/curvature (лёгкий вариант), gamma/exposure как параметризуемый пост.
   - **Версионирование имён:** при несовместимом UBO добавлять суффикс (`grayscale_v2`) или отдельное имя; `register_builtin_effects` документировать как «полный набор демо», опционально разбить на `register_builtin_post_effects` / `register_figma_fill_only`.

2. **Исходники шейдеров**
   - Файлы `.slang` в репозитории + **CMakeRC (`cmakerc`)** → `ui_sandbox_shaders_rc`; общие биндинги простых эффектов — **`post/_post_common.slang`**. Опция **`UI_SANDBOX_SHADERS_LOAD_FROM_DISK`** — загрузка с диска без CMRC; **`UI_SANDBOX_VERIFY_SLANG_SHADERS`** — проверка компиляцией `slangc` при сборке. Пошаговый чеклист: **`docs/EFFECTS.md`** («Встроенные пост-шейдеры»).

3. **Figma / дизайн-токены**
   - Опциональный слой: разбор экспорта (JSON) → заполнение `FigmaUnifiedGradientUBO` + палитра (без обязательной зависимости от JSON в ядре библиотеки — отдельный модуль/опция CMake).
   - Документация контракта уже в **`docs/FIGMA_GRADIENT.md`**; при расширении UBO синхронизировать Slang, C++ struct и диаграмму полей.

4. **Тесты и регрессии**
   - Headless или скриншот-тесты сложны без GPU-пайплайна; минимальный полезный уровень: **юнит-тесты на чистую логику** (например сортировка стопов, интерполяция палитры) с выносом в `.cpp` без GL.
   - Опционально: CI job «компиляция Slang» для каждого вынесенного фрагмента (если появятся файлы на диске).

5. **Стабильность API**
   - Публичные функции снабжать краткими комментариями в заголовках (контракт GL: контекст должен быть текущим для функций с `gl*`).
   - Избегать макросов Windows (`min`/`max`): при необходимости `(std::min)` / `(std::max)` или `NOMINMAX` на уровне цели.

## Связанные документы

- **`docs/EFFECTS.md`** — контракт EffectSystem, порядок кадра, палитра, UBO.
- **`docs/FIGMA_GRADIENT.md`** — семантика `figma_fill`.
