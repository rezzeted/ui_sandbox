#include "ui_sandbox_effects/registry.hpp"
#include "ui_sandbox_effects/figma.hpp"
#include "ui_sandbox_effects/shaders.hpp"

#include "imgui_effect_helpers.h"

#include <cstdio>
#include <string>

namespace ui_sandbox::effects {

void register_builtin_effects(ImGuiRenderUX::EffectSystem& effect_system)
{
    auto reg = [&](const char* name, const std::string& src) {
        ImGuiRenderUX::EffectCreateDesc desc;
        desc.name = name;
        desc.shaderSource = src;
        desc.vertexEntry = "vertexMain";
        desc.fragmentEntry = "fragmentMain";
        desc.blendMode = ImGuiRenderUX::BuiltinBlendMode::Alpha;
        std::string err;
        ImGuiRenderUX::EffectHandle h = effect_system.EnsureEffect(desc, &err);
        if (!h.IsValid())
            std::fprintf(stderr, "Effect '%s': %s\n", name, err.c_str());
        return h;
    };

    ImGuiRenderUX::EffectHandle g = reg("grayscale", grayscale_effect_slang());
    if (g.IsValid())
        effect_system.ExpectEffectUniformBytes(g, sizeof(float) * 4);

    ImGuiRenderUX::EffectHandle s = reg("sepia", sepia_effect_slang());
    if (s.IsValid())
        effect_system.ExpectEffectUniformBytes(s, sizeof(float) * 4);

    ImGuiRenderUX::EffectHandle p = reg("pixelate", pixelate_effect_slang());
    if (p.IsValid())
        effect_system.ExpectEffectUniformBytes(p, sizeof(float) * 4);

    ImGuiRenderUX::EffectHandle c = reg("chromatic", chromatic_effect_slang());
    if (c.IsValid())
        effect_system.ExpectEffectUniformBytes(c, sizeof(float) * 4);

    ImGuiRenderUX::EffectHandle i = reg("invert", invert_effect_slang());
    if (i.IsValid())
        effect_system.ExpectEffectUniformBytes(i, sizeof(float) * 4);

    ImGuiRenderUX::EffectHandle sc = reg("scanlines", scanlines_effect_slang());
    if (sc.IsValid())
        effect_system.ExpectEffectUniformBytes(sc, sizeof(float) * 4);

    ImGuiRenderUX::EffectHandle ff = reg("figma_fill", figma_fill_effect_slang());
    if (ff.IsValid())
        effect_system.ExpectEffectUniformBytes(ff, sizeof(FigmaUnifiedGradientUBO));
}

} // namespace ui_sandbox::effects
