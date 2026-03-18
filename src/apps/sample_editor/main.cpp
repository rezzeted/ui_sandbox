#include <drui/drui.h>
#include "editor_layout.h"
#include "panels.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include <algorithm>
#include <cstdio>
#include <cmath>
#include <filesystem>

static void glfw_error_cb(int err, const char* desc) {
    std::fprintf(stderr, "GLFW error %d: %s\n", err, desc);
}

static fs::path FindProjectRoot(const fs::path& start) {
    fs::path dir = start;
    for (int i = 0; i < 10 && !dir.empty(); ++i) {
        if (fs::exists(dir / "CMakeLists.txt")) return dir;
        auto parent = dir.parent_path();
        if (parent == dir) break;
        dir = parent;
    }
    return start;
}

int main(int argc, char* argv[]) {
    glfwSetErrorCallback(glfw_error_cb);
    if (!glfwInit()) return 1;

#ifdef __APPLE__
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(1600, 1000, "Sample Editor", nullptr, nullptr);
    if (!window) { glfwTerminate(); return 1; }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    float dpi_x = 1.0f, dpi_y = 1.0f;
    glfwGetWindowContentScale(window, &dpi_x, &dpi_y);
    const float content_scale = std::clamp(std::max(dpi_x, dpi_y), 1.0f, 3.0f);

    int fb_w = 0, win_w = 0;
    glfwGetFramebufferSize(window, &fb_w, nullptr);
    glfwGetWindowSize(window, &win_w, nullptr);
    const float fb_scale = (win_w > 0) ? static_cast<float>(fb_w) / static_cast<float>(win_w) : 1.0f;

    const float font_scale = content_scale;
    const float dpi_scale = content_scale / fb_scale;
    const float gap = 8.0f * dpi_scale;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImFont* default_font = DrUI::SetupFonts(io, font_scale);
    if (default_font) io.FontDefault = default_font;
    ImGui::GetStyle().FontScaleMain = 1.0f / fb_scale;

    DrUI::ApplyTheme(DrUI::ThemeId::Dark, dpi_scale);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    EditorSplitters splitters;
    PanelVisibility panels;
    panels.dpi_scale = dpi_scale;

    AppState app;
    fs::path exe_path = fs::weakly_canonical(fs::path(argv[0]));
    fs::path project_root = FindProjectRoot(exe_path.parent_path());
    app.root.name      = project_root.filename().string();
    app.root.full_path = project_root;
    app.root.is_dir    = true;
    BuildFileTree(app.root);
    app.log("Project opened");

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGuiViewport* vp = ImGui::GetMainViewport();

        HandleShortcuts(app);
        DrawMainMenuBar(panels, app);

        float status_h = panels.status_bar ? 22.0f * dpi_scale : 0.0f;

        auto layout = CalculateLayout(vp, gap, 0.0f, status_h, splitters, panels);
        HandleSplitters(layout, gap, vp, 0.0f, status_h, splitters, panels.bottom_collapsed);

        DrawLeftPanel(layout.left, dpi_scale, panels, app);
        DrawCenterPanel(layout.center, app);
        DrawBottomPanel(layout.bottom, dpi_scale, panels, app);
        DrawSplitterIndicators(layout, gap, splitters);

        DrUI::ToastAnchor anchor{layout.center.pos, layout.center.size};
        DrUI::DrawToasts(anchor);

        if (panels.status_bar) DrawStatusBar(dpi_scale, app);

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(DrUI::Colors::BackgroundPrimary.x, DrUI::Colors::BackgroundPrimary.y,
                     DrUI::Colors::BackgroundPrimary.z, DrUI::Colors::BackgroundPrimary.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
