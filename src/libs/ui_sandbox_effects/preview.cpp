#include "ui_sandbox_effects/preview.hpp"

#include <imgui.h>

namespace ui_sandbox::effects {

void draw_effect_preview_quads(ImDrawList* dl, ImTextureID tex, ImVec2 p0, float sz, float gap)
{
    const ImU32 cols[3] = { IM_COL32(255, 90, 90, 255), IM_COL32(90, 255, 120, 255), IM_COL32(120, 140, 255, 255) };
    for (int k = 0; k < 3; ++k) {
        float x0 = p0.x + k * (sz + gap);
        dl->AddImage(tex, ImVec2(x0, p0.y), ImVec2(x0 + sz, p0.y + sz), ImVec2(0, 0), ImVec2(1, 1), cols[k]);
    }
    ImGui::Dummy(ImVec2(3.f * sz + 2.f * gap, sz));
}

} // namespace ui_sandbox::effects
