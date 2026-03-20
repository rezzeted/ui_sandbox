# ImGui Slang effects (EffectSystem)

Исходники в форке: `imgui_effect_helpers.h` / `.cpp`, бэкенд `imgui_impl_opengl3_slang.*`, `imgui_render_core.h` (ветка `backends/`).

## Поставка изменений: форк + vcpkg (без патчей в порту)

Изменения в EffectSystem / Slang-бэкенде делаются **только в Git-форке ImGui**, не патчами в `ports/imgui`.

1. **Форк** (например `https://github.com/nvoronetskiy/imgui`): правки в `backends/*`, коммит, **push** в удалённый репозиторий.
2. **Порт** `toolchain/vcpkg/ports/imgui/portfile.cmake`: обновить **`REF`** на полный SHA последнего коммита форка (`vcpkg_from_git` → `URL` + `REF`).
3. **`toolchain/vcpkg/ports/imgui/vcpkg.json`**: увеличить **`port-version`** на 1 (инвалидация кэша vcpkg при том же upstream version строки пакета).
4. **Патчи** в каталоге порта для этих исходников **не используются** (список `PATCHES` в `portfile.cmake` не добавлять).
5. Пересобрать зависимости: из корня проекта с манифестом — например  
   `vcpkg install --triplet x64-windows-static --x-manifest-root=. --x-install-root=_build/vcpkg_installed --overlay-ports=toolchain/vcpkg-overlay/ports` (как в `CMakePresets.json`)  
   или `cmake --build _build` (если toolchain подхватывает manifest install на configure).

Итог: один источник правды — **коммит в форке**; порт только **привязывает** ревизию и версию порта.

Демо **figma_fill**: параметры заливки, палитра и поля для интеграции с макетом описаны в **`docs/FIGMA_GRADIENT.md`**.

Встроенные демо-эффекты и хелперы вынесены в статическую библиотеку **`ui_sandbox_effects`** (`#include <ui_sandbox_effects/ui_sandbox_effects.hpp>`). План развития библиотеки: **`docs/EFFECTS_LIBRARY_ROADMAP.md`**.

## Контракт API: `EffectHandle`

- **`EffectHandle`** = `{ id, generation }`. **`id`** стабилен на всё время жизни эффекта; **`generation`** увеличивается после **успешного** `ReloadEffect` / авто-reload.
- Старый токен после reload **невалиден** для `FindEffect` внутри системы: вызывайте снова **`FindEffectByName` / `EnsureEffect`** или храните свежий токен из возврата `ReloadEffect`.
- **`IsEffectHandleLive(handle)`** — проверка, что токен совпал с текущим поколением.
- **`DestroyEffect`** вернёт `false`, если передан устаревший `generation`.

## Порядок вызовов (каждый кадр)

1. **`effectSystem.AdvanceFrame()`** — в начале итерации главного цикла (до `NewFrame`).
2. **`ImGui::NewFrame()`** …
3. **`effectSystem.NotifyAfterImGuiNewFrame()`** — сразу после `ImGui::NewFrame()` (включает проверки порядка; опционально лог в stderr при **`SetEffectFrameContractTracing(true)`**).
4. **`effectSystem.TickAutoReload(DeltaTime)`** — опционально, после п.3.
5. UI с **`EffectWindowScope` / `EffectDrawRegionScope`** (или Begin/End вручную).
6. Перед **`ImGui::Render()`** один раз: **`EffectSubmitGuard`** или **`SubmitQueuedEffects()`**.
7. **`ImGui::Render()`**.

Двойной `SubmitQueuedEffects` в одном кадре даёт assert, если не было второго `AdvanceFrame`.

## Перезагрузка шейдеров (надёжность)

- **`ReloadEffect`** больше **не удаляет** эффект до успешной компиляции: при ошибке возвращается **тот же** `{ id, generation }`, строка ошибки в `errorText`, GPU остаётся на предыдущей версии.
- В бэкенде **`RegisterShaderProgram`**: старый GL-программа удаляется **после** успешного link новой (защита от «пустого» кадра при ошибке линковки).

## Палитра (доп. сэмплер, unit **1**)

- В **`DrawPacket`** есть **`paletteTexture`**; **`EffectSystem::SetEffectPaletteTexture(handle, id)`** задаёт её для эффекта (например полоска **W×1**, фильтрация **LINEAR**).
- В Slang/GLSL объявите **`[[vk::binding(1)]] Sampler2D Texture_palette`**; бэкенд ищет uniform с именем **`Texture_palette`** и биндит **texture unit 1** (после кадра снова **unit 0**).
- Вершинный/глобальный UBO остаётся на своих binding’ах; смешивать с большим числом стопов градиента удобнее так, чем раздувать UBO.

## Uniform (binding **2**)

- Размер ≤ **`kEffectUniformBufferMaxBytes` (256)**.
- **`SetEffectUniformStruct(handle, …)`** — как раньше.
- **`ExpectEffectUniformBytes(handle, N)`** — в debug при несовпадении размера с **`SetEffectUniformData`** сработает assert.
- Макрос **`IMGUI_EFFECT_UNIFORM_STRUCT(MyCppStruct)`** рядом со struct: `static_assert` по размеру (документирует зеркалирование Slang `cbuffer`).

## Эргономика

- **`EffectWindowScope` / `EffectDrawRegionScope`** — RAII для окон и регионов (см. заголовок).
- Не вложенные сценарии без отдельного окна: см. раздел про вложенность.

## Вложенность и лимиты

- Максимум **`kMaxOpenEffectCaptureDepth`** (64) неснятых `Begin*` эффектов; превышение — **assert**.
- **Не** открывайте **`EffectDrawRegionScope` внутри того же `EffectWindowScope`**, если регион — подмножество того же window capture (двойная очередь). Отдельное окно или только region.

## Multi-viewport

- При **`ImGuiConfigFlags_ViewportsEnable`** в захвате сохраняется **`viewportId`**; кастомные пакеты уходят в нужное OS-окно.
- После **`NotifyAfterImGuiNewFrame()`** все захваты идут в фазе UI того же кадра; при отсутствии вызова можно включить tracing и увидеть предупреждение в stderr.

## Портирование GL (`BuiltinGpuTextures`)

- По умолчанию используются функции **gl3w** из этого TU.
- **`BuiltinGpuTextures::SetTextureProcs(&procs)`** — подставить свои `GenTextures` / `DeleteTextures` / `BindTexture` / `TexParameteri` / `TexImage2D` (аргументы как `unsigned` для GL enum), если не хотите тянуть gl3w в отдельной единице трансляции (**nullptr** = снова встроенный gl3w).

## Отладка

- Окно **`ShowDebugWindow`**: статистика, в т.ч. **`capturesSkippedStaleHandle`** (токен устарел до submit).
- **`SetEffectFrameContractTracing(true)`**: сообщения о нарушении порядка (например Begin до `NotifyAfterImGuiNewFrame`).
