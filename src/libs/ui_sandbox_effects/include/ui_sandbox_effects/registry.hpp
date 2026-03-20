#pragma once

namespace ImGuiRenderUX {
class EffectSystem;
}

namespace ui_sandbox::effects {

/// Registers built-in demo effects (grayscale, sepia, figma_fill, …) on `effectSystem`.
void register_builtin_effects(ImGuiRenderUX::EffectSystem& effect_system);

} // namespace ui_sandbox::effects
