#pragma once

#include <string>

namespace ui_sandbox::effects {

/// Shared ImGui-compatible vertex stage for post-effects (std140 UBO at binding 2).
[[nodiscard]] std::string imgui_effect_vertex_slang();

[[nodiscard]] std::string make_post_effect_slang(const char* fragment_tail);

[[nodiscard]] std::string grayscale_effect_slang();
[[nodiscard]] std::string sepia_effect_slang();
[[nodiscard]] std::string pixelate_effect_slang();
[[nodiscard]] std::string chromatic_effect_slang();
[[nodiscard]] std::string invert_effect_slang();
[[nodiscard]] std::string scanlines_effect_slang();
[[nodiscard]] std::string figma_fill_effect_slang();

} // namespace ui_sandbox::effects
