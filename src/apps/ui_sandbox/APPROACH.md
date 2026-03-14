# UI Sandbox — полноценный стенд для разработки UI

## Назначение

Самостоятельное приложение для отработки всех UI-компонентов (панели, меню,
табы, кнопки, поля ввода, слайдеры, чекбоксы, цвета, статус-бар).
Проверенные здесь решения переносятся в другие приложения.

## Архитектура

```
ui_sandbox/
├── main.cpp          — точка входа, главный цикл, раскладка, панели
├── sandbox_theme.h   — полная тема (Colors + ApplyModernStyle)
├── icons.h           — Font Awesome иконки (UTF-8 defines)
├── ui_helpers.h      — вспомогательные UI-функции (IconButton*, AccentBar, PanelBorder)
├── CMakeLists.txt    — сборка (linkage: imgui)
└── APPROACH.md       — этот файл
```

Зависимости: только `imgui` (который тянет GLFW + OpenGL3).
Никаких внешних зависимостей кроме `imgui` + GLFW + OpenGL3.

## Раскладка (3 панели)

Ручная пропорциональная раскладка без docking. Почему — см. историю ниже.

```
+================================================================+
|  MENU BAR (Файл, Правка, Вид, Отладка, Справка)               |
+================================================================+
|                                                                |
|  +--Left Panel--+  +--Canvas Panel--+                          |
|  | Tab: Обзор   |  |                |                          |
|  | Tab: Виджеты |  |  Grid +        |                          |
|  |              |  |  placeholder   |                          |
|  |  split_h     |  |                |                          |
|  |              |  +----------------+                          |
|  |              |       drag_v ↕                               |
|  |              |  +--Bottom Panel--+                          |
|  |              |  | Tab: Управление|                          |
|  |              |  | Tab: Лог       |                          |
|  |              |  | Tab: Свойства  |                          |
|  +--------------+  +----------------+                          |
|                                                                |
+================================================================+
|  STATUS BAR (app name, ImGui version, FPS)                     |
+================================================================+
```

### Формулы

```
menu_h   = ImGui::GetFrameHeight()
status_h = 22px * dpi_scale (или 0 если скрыт)
gap      = 8px * dpi_scale

usable_w = viewport.w - gap * 3
left.w   = usable_w * split_h          (0.12 .. 0.50)
right_w  = usable_w - left.w

usable_h = (viewport.h - menu_h - status_h) - gap * 3
canvas.h = usable_h * split_v          (0.30 .. 0.90)
bottom.h = usable_h - canvas.h
```

### Сплиттеры

Два сплиттера — горизонтальный (split_h, ↔) и вертикальный (split_v, ↕).
Захват = зазор между панелями ± 4px. Внешние края фиксированы.

## Компоненты UI

### Меню (MainMenuBar)
- Файл: Новый, Открыть, Сохранить, Выход
- Правка: Отменить, Повторить, Копировать, Вставить
- Вид: переключение видимости табов и статус-бара
- Отладка: Запуск, Шаг, Сброс
- Справка: О программе

### Левая панель (табы)
- **Обзор** — файловая структура (Selectable list)
- **Виджеты** — витрина всех виджетов:
  - Кнопки: обычные, IconButton, IconButtonCentered, цветные (Success/Warning/Error)
  - Поля ввода: InputText, InputTextMultiline, InputInt, InputFloat
  - Слайдеры: SliderFloat, SliderInt, ProgressBar
  - Checkbox, RadioButton, Combo
  - ColorEdit4

### Центральная панель (Canvas)
- Тёмный фон с сеткой 32px
- Текст-заглушка по центру
- InvisibleButton для будущей обработки мыши

### Нижняя панель (табы)
- **Управление** — кнопки Play/Pause/Step/Stop + параметры
- **Лог** — добавление/очистка записей, скроллинг
- **Свойства** — редактор узла (имя, тип, позиция, размер, цвет)

### Статус-бар
- Название приложения, версия ImGui, FPS

## Стиль

VS Code Dark Modern, полная палитра в `sandbox_theme.h`:

```
BackgroundPrimary   (0.06, 0.06, 0.08)  — glClearColor, зазоры
BackgroundSecondary (0.11, 0.11, 0.14)  — фон панелей
BackgroundTertiary  (0.14, 0.14, 0.18)  — попапы, active frame
AccentPrimary       (0.37, 0.55, 0.95)  — кнопки, overline, ссылки
AccentSecondary     (0.63, 0.45, 0.98)  — фиолетовый акцент
Border              (0.28, 0.28, 0.33)  — рамки
```

## Проблемы docking (историческая справка)

### 1. Сплиттер между dock-нодами — неотключаемый

ImGui рисует сплиттер в `DockNodeTreeUpdateSplitter()` (imgui.cpp:19122-19126).
Перед отрисовкой **принудительно подменяет** цвет:

```cpp
PushStyleColor(ImGuiCol_Separator, g.Style.Colors[ImGuiCol_Border]);
DockNodeTreeUpdateSplitter(node);
PopStyleColor(3);
```

Настройка `ImGuiCol_Separator` через стиль **не работает** — ImGui заменяет её на `ImGuiCol_Border`.
А `ImGuiCol_Border` нужен для рамок панелей, его нельзя обнулить.

### 2. Скругление углов убивается на внутренних гранях

`CalcRoundingFlagsForRectInRect()` решает какие углы скруглять. Угол скругляется
только если панель примыкает к краю host-окна. Внутренние углы **всегда прямые**.

### 3. Tab bar у одиночных панелей

`ImGuiDockNodeFlags_AutoHideTabBar` скрывает tab bar, но при наведении он мелькает.

### 4. `imgui.ini` кэширует раскладку

DockBuilder layout только если нет сохранённого в `imgui.ini`. Мешает итерациям.

**Решение:** ручная раскладка через `SetNextWindowPos/Size` + `Begin`.
