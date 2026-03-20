#include "ui_sandbox_effects/shaders.hpp"

#include "shader_disk_io.hpp"

#include <cmrc/cmrc.hpp>

#include <string>
#include <string_view>

CMRC_DECLARE(ui_sandbox_shaders);

namespace ui_sandbox::effects {
namespace {

[[nodiscard]] std::string load_shader_file_embedded(std::string_view cmrc_relative_path)
{
    auto fs = cmrc::ui_sandbox_shaders::get_filesystem();
    auto f = fs.open(std::string(cmrc_relative_path));
    return std::string(f.begin(), f.end());
}

[[nodiscard]] const std::string& post_vertex_cached_embedded()
{
    static const std::string k = load_shader_file_embedded("imgui_post_vertex.slang");
    return k;
}

[[nodiscard]] const std::string& post_common_cached_embedded()
{
    static const std::string k = load_shader_file_embedded("post/_post_common.slang");
    return k;
}

[[nodiscard]] std::string load_post_effect_shader_composite_impl(std::string_view frag_stem, bool use_post_common)
{
    const std::string frag_rel = std::string("post/") + std::string(frag_stem) + ".slang";

#if defined(UI_SANDBOX_SHADERS_LOAD_FROM_DISK)
    std::string v;
    std::string f;
    if (!read_shader_tree_file("imgui_post_vertex.slang", v))
        return {};
    if (!read_shader_tree_file(frag_rel, f))
        return {};
    if (use_post_common) {
        std::string c;
        if (!read_shader_tree_file("post/_post_common.slang", c))
            return {};
        return v + "\n" + c + "\n" + f;
    }
    return v + "\n" + f;
#else
    const std::string& v = post_vertex_cached_embedded();
    std::string f = load_shader_file_embedded(frag_rel);
    if (use_post_common) {
        const std::string& c = post_common_cached_embedded();
        return v + "\n" + c + "\n" + f;
    }
    return v + "\n" + f;
#endif
}

} // namespace

std::string imgui_effect_vertex_slang()
{
#if defined(UI_SANDBOX_SHADERS_LOAD_FROM_DISK)
    std::string v;
    if (!read_shader_tree_file("imgui_post_vertex.slang", v))
        return {};
    return v;
#else
    return post_vertex_cached_embedded();
#endif
}

std::string make_post_effect_slang(const char* fragment_tail)
{
#if defined(UI_SANDBOX_SHADERS_LOAD_FROM_DISK)
    std::string v;
    if (!read_shader_tree_file("imgui_post_vertex.slang", v))
        return {};
    return v + "\n" + std::string(fragment_tail);
#else
    return post_vertex_cached_embedded() + "\n" + std::string(fragment_tail);
#endif
}

std::string load_post_effect_shader_composite(std::string_view frag_stem, bool use_post_common)
{
    return load_post_effect_shader_composite_impl(frag_stem, use_post_common);
}

std::string composite_builtin_post_shader(const BuiltinEffectSpec& spec)
{
    return load_post_effect_shader_composite(spec.frag_stem, spec.use_post_common);
}

std::string grayscale_effect_slang()
{
    return load_post_effect_shader_composite("grayscale", true);
}

std::string sepia_effect_slang()
{
    return load_post_effect_shader_composite("sepia", true);
}

std::string pixelate_effect_slang()
{
    return load_post_effect_shader_composite("pixelate", true);
}

std::string chromatic_effect_slang()
{
    return load_post_effect_shader_composite("chromatic", true);
}

std::string invert_effect_slang()
{
    return load_post_effect_shader_composite("invert", true);
}

std::string scanlines_effect_slang()
{
    return load_post_effect_shader_composite("scanlines", true);
}

std::string figma_fill_effect_slang()
{
    return load_post_effect_shader_composite("figma_fill", false);
}

} // namespace ui_sandbox::effects
