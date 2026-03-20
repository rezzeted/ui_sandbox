#include "shader_disk_io.hpp"

#if defined(UI_SANDBOX_SHADERS_LOAD_FROM_DISK)

#include "shader_sources_config.hpp"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace ui_sandbox::effects {

bool read_shader_tree_file(std::string_view relative_path, std::string& out)
{
    out.clear();
    namespace fs = std::filesystem;
    fs::path p = fs::path(UI_SANDBOX_SHADER_SOURCE_ROOT) / fs::path(relative_path);
    std::ifstream in(p, std::ios::binary);
    if (!in) {
        std::fprintf(stderr, "ui_sandbox_effects: missing or unreadable shader file: %s\n", p.string().c_str());
        return false;
    }
    std::ostringstream oss;
    oss << in.rdbuf();
    out = std::move(oss).str();
    return true;
}

} // namespace ui_sandbox::effects

#else

namespace ui_sandbox::effects {

bool read_shader_tree_file(std::string_view, std::string& out)
{
    out.clear();
    return false;
}

} // namespace ui_sandbox::effects

#endif
