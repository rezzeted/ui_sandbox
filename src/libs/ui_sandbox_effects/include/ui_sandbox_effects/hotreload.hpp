#pragma once

namespace ui_sandbox::effects {

/// True when the library was built with `UI_SANDBOX_SHADERS_LOAD_FROM_DISK` (disk trees + bundle hot-reload).
[[nodiscard]] bool post_shaders_disk_mode_enabled();

/// When disk mode is on: rewrite concat bundles under `UI_SANDBOX_SHADER_BUNDLE_DIR` if any source part is newer
/// (so `EffectSystem::TickAutoReload` sees mtime changes). No-op in CMRC builds.
void refresh_post_effect_bundles_for_hotreload();

} // namespace ui_sandbox::effects
