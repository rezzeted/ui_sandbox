#pragma once

#include <string>
#include <string_view>

namespace ui_sandbox::effects {

/// `relative_path` is under `UI_SANDBOX_SHADER_SOURCE_ROOT` (e.g. `imgui_post_vertex.slang`).
[[nodiscard]] bool read_shader_tree_file(std::string_view relative_path, std::string& out);

} // namespace ui_sandbox::effects
