# Библиотека `ui_sandbox_effects` — границы и план наполнения

Цель: держать **демо-шейдеры**, **регистрацию в `EffectSystem`** и **утилиты Figma-градиента** отдельно от приложения `ui_sandbox`, чтобы другие приложения могли подключать один и тот же набор без копирования строк.

## Текущее состояние

| Компонент | Путь | Назначение |
|-----------|------|------------|
| Публичный вход | `src/libs/ui_sandbox_effects/include/ui_sandbox_effects/ui_sandbox_effects.hpp` | Единая точка `#include` |
| Slang-тексты | `include/.../shaders.hpp`, `shaders.cpp` | API строк Slang; исходники лежат в **`src/libs/ui_sandbox_shaders/shaders/`** (`imgui_post_vertex.slang`, `post/*.slang`), в бинарь встраиваются через **cmakerc** (таргет `ui_sandbox_shaders_rc` / `ui_sandbox::shaders_rc`) |
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
   - Файлы `.slang` в репозитории + **CMakeRC (порт `cmakerc`)**: отдельная цель `ui_sandbox_shaders_rc`. Для новых пост-эффектов — добавить `post/<имя>.slang`, зарегистрировать файл в `src/libs/ui_sandbox_shaders/CMakeLists.txt`, вернуть строку из `shaders.cpp` через `post_effect_from_frag_path` (или расширить общий хелпер).

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
