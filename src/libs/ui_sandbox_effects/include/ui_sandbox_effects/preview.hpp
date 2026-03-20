#pragma once

struct ImDrawList;
struct ImVec2;
typedef unsigned long long ImTextureID;

namespace ui_sandbox::effects {

/// Three tinted quads with `tex` for effect preview (use inside EffectDrawRegionScope).
void draw_effect_preview_quads(ImDrawList* dl, ImTextureID tex, ImVec2 p0, float sz, float gap);

} // namespace ui_sandbox::effects
