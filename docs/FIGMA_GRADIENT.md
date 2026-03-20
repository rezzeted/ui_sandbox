# Figma-подобный градиент (`figma_fill`)

Соответствие макету — в рамках **пост-эффекта по ImGui UV 0…1** на кваде (как «плоский» векторный фрейм). Полный рендер векторов Figma здесь не воспроизводится.

## Поток данных

1. **Стопы** (позиция 0…1, цвет **sRGB** 0…1 в UI, альфа 0…1) → на CPU переводятся в **линейный RGB** для интерполяции, затем либо кодируются обратно в sRGB для **RGBA8**, либо остаются **линейными** для **RGBA16F** палитры (`paletteLinear` в UBO).
2. **Палитра** — текстура 1×`paletteWidth` (по умолчанию 1024), фильтр LINEAR, unit **1**; см. `docs/EFFECTS.md`.
3. **UBO** `FigmaUnifiedGradientUBO` (std140, 96 байт) зеркалит Slang `GradientParams` в `main.cpp`.

## Поля UBO / JSON для плагина или экспорта

Имена ниже — как в C++; в JSON можно использовать `snake_case` и маппить в шейдер при сборке UBO.

| Поле | Тип | Смысл |
|------|-----|--------|
| `linearAx`, `linearAy`, `linearBx`, `linearBy` | float | Линейный градиент: концы в **локальном UV** после UV-аффина. |
| `centerX`, `centerY` | float | Центр радиального / углового / diamond. |
| `ellipseX`, `ellipseY` | float | Полуоси эллипса (масштаб нормалей). |
| `gtype` | float | 0 linear, 1 radial, 2 angular, 3 diamond. |
| `replaceMix` | float | 0…1 смесь заливки поверх `Texture_0`. |
| `angleStart` | float | Начало палитры для angular, **радианы**. |
| `paletteWidth` | float | Ширина палитры в текселях (float для шейдера). |
| `spreadMode` | float | 0 pad (clamp), 1 repeat, 2 reflect. |
| `ditherStrength` | float | Сила экранного шума против бэнда, 0…1. |
| `gradM0`…`gradM5` | float | Аффинное **UV**: `guv.x = m0·u + m1·v + m2`, `guv.y = m3·u + m4·v + m5` (как в коде `FigmaBuildGradientAffine`). |
| `paletteLinear` | float | 1 — палитра RGBA16F, линейный RGB; шейдер делает `linearToSrgb`. 0 — RGBA8, уже sRGB в сэмпле. |

Дополнительно в приложении: флаги `_pad0`…`_pad2` зарезервированы (std140).

## Пример JSON (фрагмент)

```json
{
  "kind": "figma_fill",
  "gtype": "linear",
  "spread": "reflect",
  "replace_mix": 1.0,
  "angle_start_deg": 0,
  "palette_width": 1024,
  "palette_storage": "rgba16f",
  "dither": 0.35,
  "frame": { "rotate_deg": 0, "scale": [1, 1], "translate": [0, 0] },
  "linear": { "a": [0.05, 0.15], "b": [0.95, 0.85] },
  "radial": { "center": [0.5, 0.5], "ellipse": [0.5, 0.5] },
  "stops": [
    { "position": 0.0, "color": [0.39, 0.08, 0.95, 1.0] },
    { "position": 1.0, "color": [0.15, 0.65, 1.0, 1.0] }
  ]
}
```

Цвета стопов в примере — **уже в пространстве UI** (как `ImGui::ColorEdit4` с флагом float): интерпретируются как **sRGB** для RGB-каналов.

## Ограничения относительно Figma

- Нет **mesh warp**, сложных контуров, **нескольких заливок** на одном пути.
- **Blend mode** заливки как в Figma — не моделируется отдельно (общий alpha-бленд ImGui).
- Интерполяция **OKLab/LAB** как в последних версиях Figma — не реализована (только sRGB ↔ linear по каналам).
- Для полного совпадения с экспортом нужен **плагин**, который отдаёт числа в тех же координатах и единицах, что и ваш UBO.
