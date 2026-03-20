#include "ui_sandbox_effects/hotreload.hpp"

#include "ui_sandbox_effects/builtin_effect_specs.hpp"
#include "shader_disk_io.hpp"

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>

#if defined(UI_SANDBOX_SHADERS_LOAD_FROM_DISK)
#include "shader_sources_config.hpp"
#endif

namespace ui_sandbox::effects {

bool post_shaders_disk_mode_enabled()
{
#if defined(UI_SANDBOX_SHADERS_LOAD_FROM_DISK)
    return true;
#else
    return false;
#endif
}

#if defined(UI_SANDBOX_SHADERS_LOAD_FROM_DISK)
namespace {

[[nodiscard]] long long file_mtime_ticks(const std::filesystem::path& p)
{
    namespace fs = std::filesystem;
    std::error_code ec;
    if (!fs::exists(p, ec))
        return 0;
    const auto ft = fs::last_write_time(p, ec);
    if (ec)
        return 0;
    return static_cast<long long>(ft.time_since_epoch().count());
}

[[nodiscard]] bool write_bundle_if_stale(const BuiltinEffectSpec& spec)
{
    namespace fs = std::filesystem;

    const fs::path root(UI_SANDBOX_SHADER_SOURCE_ROOT);
    const fs::path bundle_dir(UI_SANDBOX_SHADER_BUNDLE_DIR);
    const fs::path bundle_path = bundle_dir / (std::string(spec.name) + ".slang");

    const fs::path vertex_path = root / "imgui_post_vertex.slang";
    const fs::path frag_path = root / "post" / (std::string(spec.frag_stem) + ".slang");
    const fs::path common_path = root / "post" / "_post_common.slang";

    long long dep_tick = file_mtime_ticks(vertex_path);
    dep_tick = (std::max)(dep_tick, file_mtime_ticks(frag_path));
    if (spec.use_post_common)
        dep_tick = (std::max)(dep_tick, file_mtime_ticks(common_path));
    if (dep_tick == 0) {
        std::fprintf(stderr, "ui_sandbox_effects: hotreload: missing shader source under %s\n",
                     root.string().c_str());
        return false;
    }

    if (fs::exists(bundle_path)) {
        const long long bt = file_mtime_ticks(bundle_path);
        if (bt >= dep_tick)
            return true;
    }

    std::error_code ec;
    fs::create_directories(bundle_dir, ec);

    std::string v;
    std::string c;
    std::string f;
    if (!read_shader_tree_file("imgui_post_vertex.slang", v))
        return false;
    const std::string frag_rel = std::string("post/") + spec.frag_stem + ".slang";
    if (!read_shader_tree_file(frag_rel, f))
        return false;
    std::string text;
    if (spec.use_post_common) {
        if (!read_shader_tree_file("post/_post_common.slang", c))
            return false;
        text = v + "\n" + c + "\n" + f + "\n";
    } else {
        text = v + "\n" + f + "\n";
    }

    std::ofstream out(bundle_path, std::ios::binary | std::ios::trunc);
    if (!out) {
        std::fprintf(stderr, "ui_sandbox_effects: hotreload: could not write %s\n",
                     bundle_path.string().c_str());
        return false;
    }
    out << text;
    return static_cast<bool>(out);
}

} // namespace

void refresh_post_effect_bundles_for_hotreload()
{
    for (const BuiltinEffectSpec& spec : kBuiltinEffectSpecs) {
        (void)write_bundle_if_stale(spec);
    }
}

#else

void refresh_post_effect_bundles_for_hotreload() {}

#endif

} // namespace ui_sandbox::effects
