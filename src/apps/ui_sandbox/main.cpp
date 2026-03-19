#define NOMINMAX
#include <drui/drui.h>
#include "editor_layout.h"
#include "panels.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3_slang.h"
#include "imgui_effect_helpers.h"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <cstdio>
#include <cmath>

static void glfw_error_cb(int err, const char* desc) {
    std::fprintf(stderr, "GLFW error %d: %s\n", err, desc);
}

// Slang source: same vertex layout as imgui default, fragment outputs grayscale
static const char* g_grayscaleShaderSource = R"slang(
struct VSOutput {
    float2 Frag_UV : TEXCOORD0;
    float4 Frag_Color : COLOR0;
    float4 Position : SV_Position;
};

uniform float4x4 ProjMtx;

[shader("vertex")]
VSOutput vertexMain(
    float2 Position : POSITION0,
    float2 UV : TEXCOORD0,
    float4 Color : COLOR0)
{
    VSOutput output;
    output.Frag_UV = UV;
    output.Frag_Color = Color;
    output.Position = mul(ProjMtx, float4(Position, 0.0, 1.0));
    return output;
}

[[vk::binding(0)]] Sampler2D Texture_0;

[shader("fragment")]
float4 fragmentMain(
    float2 Frag_UV : TEXCOORD0,
    float4 Frag_Color : COLOR0,
    float4 Position : SV_Position) : SV_Target
{
    float4 texColor = Texture_0.Sample(Frag_UV);
    float4 finalColor = texColor * Frag_Color;
    float gray = dot(finalColor.rgb, float3(0.299, 0.587, 0.114));
    return float4(gray, gray, gray, finalColor.a);
}
)slang";

int main() {
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

    GLFWwindow* window = glfwCreateWindow(1600, 1000, "UI Sandbox", nullptr, nullptr);
    if (!window) { glfwTerminate(); return 1; }
    glfwMakeContextCurrent(window);
    if (gl3wInit() != 0) {
        std::fprintf(stderr, "Failed to initialize OpenGL loader (gl3w)\n");
        glfwTerminate();
        return 1;
    }
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
    ImGui_ImplOpenGL3Slang_Init(glsl_version);

    ImGuiRenderUX::EffectSystem effectSystem;
    if (!effectSystem.Initialize()) {
        std::fprintf(stderr, "EffectSystem init failed\n");
    }
    ImGuiRenderUX::EffectHandle grayscaleEffect = ImGuiRenderUX::kInvalidEffectHandle;
    {
        ImGuiRenderUX::EffectCreateDesc desc;
        desc.name = "grayscale";
        desc.shaderSource = g_grayscaleShaderSource;
        desc.vertexEntry = "vertexMain";
        desc.fragmentEntry = "fragmentMain";
        desc.blendMode = ImGuiRenderUX::BuiltinBlendMode::Alpha;
        std::string err;
        grayscaleEffect = effectSystem.CreateEffect(desc, &err);
        if (grayscaleEffect == ImGuiRenderUX::kInvalidEffectHandle)
            std::fprintf(stderr, "Grayscale effect: %s\n", err.c_str());
    }

    // 1x1 white texture for effect-window quads (font atlas is skipped by the effect)
    GLuint whiteTexId = 0;
    {
        unsigned char white[] = { 255, 255, 255, 255 };
        glGenTextures(1, &whiteTexId);
        glBindTexture(GL_TEXTURE_2D, whiteTexId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    ImTextureID whiteTexImId = (ImTextureID)(intptr_t)whiteTexId;

    EditorSplitters splitters;
    PanelVisibility panels;
    panels.dpi_scale = dpi_scale;
    bool show_grayscale_window = true;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3Slang_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGuiViewport* vp = ImGui::GetMainViewport();

        DrawMainMenuBar(panels);

        float status_h = panels.status_bar ? 22.0f * dpi_scale : 0.0f;

        auto layout = CalculateLayout(vp, gap, 0.0f, status_h, splitters);
        HandleSplitters(layout, gap, vp, 0.0f, status_h, splitters);

        DrawLeftPanel(layout.left, dpi_scale, panels);
        DrawCanvasPanel(layout.canvas, dpi_scale);
        DrawSplitterIndicators(layout, gap, splitters);

        if (grayscaleEffect != ImGuiRenderUX::kInvalidEffectHandle && show_grayscale_window) {
            if (effectSystem.BeginEffectWindow("Grayscale", grayscaleEffect, &show_grayscale_window)) {
                ImGui::TextUnformatted("This window uses a custom grayscale shader.");
                ImGui::Separator();
                // Draw with a non-font texture so the effect is applied (font atlas is skipped).
                ImVec2 p0 = ImGui::GetCursorScreenPos();
                float sz = 60.f * dpi_scale;
                ImVec2 p1(p0.x + sz, p0.y + sz);
                ImDrawList* dl = ImGui::GetWindowDrawList();
                dl->AddImage(whiteTexImId, p0, p1, ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 80, 80, 255));
                dl->AddImage(whiteTexImId, ImVec2(p0.x + sz + 8.f, p0.y), ImVec2(p1.x + sz + 8.f, p1.y), ImVec2(0, 0), ImVec2(1, 1), IM_COL32(80, 255, 80, 255));
                dl->AddImage(whiteTexImId, ImVec2(p0.x + 2.f * (sz + 8.f), p0.y), ImVec2(p1.x + 2.f * (sz + 8.f), p1.y), ImVec2(0, 0), ImVec2(1, 1), IM_COL32(80, 80, 255, 255));
                ImGui::Dummy(ImVec2(3.f * sz + 16.f, sz));
                ImGui::TextUnformatted("Colored quads above are drawn in grayscale by the shader.");
                effectSystem.EndEffectWindow();
            }
        }

        effectSystem.SubmitQueuedEffects();

        DrUI::ToastAnchor anchor{layout.canvas.pos, layout.canvas.size};
        DrUI::DrawToasts(anchor);

        if (panels.status_bar) DrawStatusBar(dpi_scale);

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(DrUI::Colors::BackgroundPrimary.x, DrUI::Colors::BackgroundPrimary.y,
                     DrUI::Colors::BackgroundPrimary.z, DrUI::Colors::BackgroundPrimary.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3Slang_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    if (whiteTexId != 0)
        glDeleteTextures(1, &whiteTexId);
    ImGui_ImplOpenGL3Slang_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
