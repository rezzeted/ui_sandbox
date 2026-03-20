#include "ui_sandbox_effects/shaders.hpp"

#include <cmrc/cmrc.hpp>

#include <string>
#include <string_view>

CMRC_DECLARE(ui_sandbox_shaders);

namespace ui_sandbox::effects {
namespace {

[[nodiscard]] std::string cmrc_slurp(std::string_view path)
{
    auto fs = cmrc::ui_sandbox_shaders::get_filesystem();
    auto f = fs.open(std::string(path));
    return std::string(f.begin(), f.end());
}

[[nodiscard]] std::string post_effect_from_frag_path(std::string_view frag_path)
{
    static const std::string vertex = cmrc_slurp("imgui_post_vertex.slang");
    return vertex + "\n" + cmrc_slurp(frag_path);
}

} // namespace

std::string imgui_effect_vertex_slang()
{
    return cmrc_slurp("imgui_post_vertex.slang");
}

std::string make_post_effect_slang(const char* fragment_tail)
{
    static const std::string vertex = cmrc_slurp("imgui_post_vertex.slang");
    return vertex + "\n" + std::string(fragment_tail);
}

std::string grayscale_effect_slang()
{
    return post_effect_from_frag_path("post/grayscale.slang");
}

std::string sepia_effect_slang()
{
    return post_effect_from_frag_path("post/sepia.slang");
}

std::string pixelate_effect_slang()
{
    return post_effect_from_frag_path("post/pixelate.slang");
}

std::string chromatic_effect_slang()
{
    return post_effect_from_frag_path("post/chromatic.slang");
}

std::string invert_effect_slang()
{
    return post_effect_from_frag_path("post/invert.slang");
}

std::string scanlines_effect_slang()
{
    return post_effect_from_frag_path("post/scanlines.slang");
}

std::string figma_fill_effect_slang()
{
    return post_effect_from_frag_path("post/figma_fill.slang");
}

} // namespace ui_sandbox::effects
