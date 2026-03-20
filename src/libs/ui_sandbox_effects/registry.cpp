#include "ui_sandbox_effects/registry.hpp"
#include "ui_sandbox_effects/builtin_effect_specs.hpp"
#include "ui_sandbox_effects/hotreload.hpp"
#include "ui_sandbox_effects/shaders.hpp"

#if defined(UI_SANDBOX_SHADERS_LOAD_FROM_DISK)
#include "shader_sources_config.hpp"
#endif

#include "imgui_effect_helpers.h"

#include <cstdio>
#include <filesystem>
#include <string>

namespace ui_sandbox::effects {

void register_builtin_effects(ImGuiRenderUX::EffectSystem& effect_system)
{
#if defined(UI_SANDBOX_SHADERS_LOAD_FROM_DISK)
    refresh_post_effect_bundles_for_hotreload();
#endif

    for (const BuiltinEffectSpec& e : kBuiltinEffectSpecs) {
        std::string err;
        ImGuiRenderUX::EffectHandle h;

#if defined(UI_SANDBOX_SHADERS_LOAD_FROM_DISK)
        ImGuiRenderUX::EffectCreateDesc desc;
        desc.name = e.name;
        desc.shaderSource.clear();
        desc.vertexEntry = "vertexMain";
        desc.fragmentEntry = "fragmentMain";
        desc.blendMode = ImGuiRenderUX::BuiltinBlendMode::Alpha;
        namespace fs = std::filesystem;
        desc.shaderFile = (fs::path(UI_SANDBOX_SHADER_BUNDLE_DIR) / (std::string(e.name) + ".slang")).string();
        h = effect_system.CreateEffectFromFile(desc, &err);
#else
        ImGuiRenderUX::EffectCreateDesc desc;
        desc.name = e.name;
        desc.shaderSource = composite_builtin_post_shader(e);
        desc.vertexEntry = "vertexMain";
        desc.fragmentEntry = "fragmentMain";
        desc.blendMode = ImGuiRenderUX::BuiltinBlendMode::Alpha;
        h = effect_system.EnsureEffect(desc, &err);
#endif

        if (!h.IsValid())
            std::fprintf(stderr, "Effect '%s': %s\n", e.name, err.c_str());
        else
            effect_system.ExpectEffectUniformBytes(h, e.uniform_bytes);
    }
}

} // namespace ui_sandbox::effects
