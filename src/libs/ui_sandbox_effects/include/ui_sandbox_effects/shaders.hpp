#pragma once

#include "ui_sandbox_effects/builtin_effect_specs.hpp"

#include <string>

namespace ui_sandbox::effects {

/// Shared ImGui-compatible vertex stage for post-effects (std140 UBO at binding 2).
[[nodiscard]] std::string imgui_effect_vertex_slang();

[[nodiscard]] std::string make_post_effect_slang(const char* fragment_tail);

/// Vertex + optional `_post_common.slang` + `post/{frag_stem}.slang` (matches bundled layout).
[[nodiscard]] std::string load_post_effect_shader_composite(std::string_view frag_stem, bool use_post_common);

[[nodiscard]] std::string composite_builtin_post_shader(const BuiltinEffectSpec& spec);

[[nodiscard]] std::string grayscale_effect_slang();
[[nodiscard]] std::string sepia_effect_slang();
[[nodiscard]] std::string pixelate_effect_slang();
[[nodiscard]] std::string chromatic_effect_slang();
[[nodiscard]] std::string invert_effect_slang();
[[nodiscard]] std::string scanlines_effect_slang();
[[nodiscard]] std::string figma_fill_effect_slang();

} // namespace ui_sandbox::effects
