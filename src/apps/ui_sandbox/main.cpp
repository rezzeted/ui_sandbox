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
#include <array>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <numeric>
#include <string>
#include <vector>

#include <ui_sandbox_effects/ui_sandbox_effects.hpp>

namespace fx = ui_sandbox::effects;

static void glfw_error_cb(int err, const char* desc) {
    std::fprintf(stderr, "GLFW error %d: %s\n", err, desc);
}

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
    const bool figma_palette_16f_ok = fx::figma_probe_rgba16f_texture();
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
    // Floatable ImGui windows become real GLFW/OS windows when dragged outside the main viewport
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImFont* default_font = DrUI::SetupFonts(io, font_scale);
    if (default_font) io.FontDefault = default_font;
    ImGui::GetStyle().FontScaleMain = 1.0f / fb_scale;

    DrUI::ApplyTheme(DrUI::ThemeId::Dark, dpi_scale);

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3Slang_Init(glsl_version);

    ImGuiRenderUX::EffectSystem effectSystem;
    if (!effectSystem.Initialize()) {
        std::fprintf(stderr, "EffectSystem init failed\n");
    }
    ImGuiRenderUX::BuiltinGpuTextures builtinTextures;
    builtinTextures.EnsureWhite1x1();
    ImTextureID whiteTexImId = builtinTextures.White1x1();

    fx::register_builtin_effects(effectSystem);

    GLuint figma_palette_gl = 0;
    glGenTextures(1, &figma_palette_gl);

    EditorSplitters splitters;
    PanelVisibility panels;
    panels.dpi_scale = dpi_scale;
    bool show_grayscale_window = true;
    bool show_region_demo_window = true;
    bool show_effect_gallery = true;
    bool show_effect_debug = false;
    static float s_grayscaleMix = 1.0f;
    static float s_sepiaMix = 0.85f;
    static float s_pixelateBlocks = 18.f;
    static float s_chromaStrength = 0.006f;
    static float s_invertMix = 0.65f;
    static float s_scanlineIntensity = 0.35f;
    static float s_scanlineLines = 180.f;
    // Figma-style fills: Linear | Radial | Angular | Diamond (UV 0..1); stops baked into GL palette texture.
    static int   s_figmaGtype = 0;
    static int   s_figmaStopCount = 4;
    static float s_figmaReplace = 1.f;
    static float s_figmaLinA[2] = { 0.05f, 0.15f };
    static float s_figmaLinB[2] = { 0.95f, 0.85f };
    static float s_figmaCenter[2] = { 0.5f, 0.5f };
    static float s_figmaEllipse[2] = { 0.5f, 0.5f };
    static float s_figmaAngleDeg = 0.f;
    static float s_figmaC[fx::kFigmaMaxStops][4] = {
        { 0.39f, 0.08f, 0.95f, 1.f },
        { 0.95f, 0.25f, 0.45f, 1.f },
        { 0.98f, 0.75f, 0.2f, 1.f },
        { 0.15f, 0.65f, 1.f, 1.f },
    };
    static float s_figmaS[fx::kFigmaMaxStops] = { 0.f, 0.33f, 0.66f, 1.f };
    static int   s_figmaSpread = 0;
    static float s_figmaDither = 0.35f;
    static float s_figmaFrameRotDeg = 0.f;
    static float s_figmaFrameSx = 1.f;
    static float s_figmaFrameSy = 1.f;
    static float s_figmaFrameTx = 0.f;
    static float s_figmaFrameTy = 0.f;
    static bool  s_figmaPalette16f = true;

    {
        ImGuiRenderUX::EffectHandle palFx = effectSystem.FindEffectByName("figma_fill");
        if (palFx.IsValid())
            effectSystem.SetEffectPaletteTexture(palFx, (ImTextureID)(intptr_t)figma_palette_gl);
        fx::rebuild_figma_palette_texture(figma_palette_gl, fx::kFigmaPaletteTexWidth, s_figmaStopCount, s_figmaC, s_figmaS,
                                   figma_palette_16f_ok && s_figmaPalette16f);
    }

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        effectSystem.AdvanceFrame();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED)) {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }
        ImGui_ImplOpenGL3Slang_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        effectSystem.NotifyAfterImGuiNewFrame();
        effectSystem.TickAutoReload(ImGui::GetIO().DeltaTime);

        ImGuiRenderUX::EffectHandle grayscaleEffect = effectSystem.FindEffectByName("grayscale");
        ImGuiRenderUX::EffectHandle sepiaEffect = effectSystem.FindEffectByName("sepia");
        ImGuiRenderUX::EffectHandle pixelateEffect = effectSystem.FindEffectByName("pixelate");
        ImGuiRenderUX::EffectHandle chromaticEffect = effectSystem.FindEffectByName("chromatic");
        ImGuiRenderUX::EffectHandle invertEffect = effectSystem.FindEffectByName("invert");
        ImGuiRenderUX::EffectHandle scanlinesEffect = effectSystem.FindEffectByName("scanlines");
        ImGuiRenderUX::EffectHandle figmaFillEffect = effectSystem.FindEffectByName("figma_fill");

        ImGuiViewport* vp = ImGui::GetMainViewport();

        DrawMainMenuBar(panels);

        float status_h = panels.status_bar ? 22.0f * dpi_scale : 0.0f;

        auto layout = CalculateLayout(vp, gap, 0.0f, status_h, splitters);
        HandleSplitters(layout, gap, vp, 0.0f, status_h, splitters);

        DrawLeftPanel(layout.left, dpi_scale, panels);
        DrawCanvasPanel(layout.canvas, dpi_scale);
        DrawSplitterIndicators(layout, gap, splitters);

        if (grayscaleEffect.IsValid() && show_grayscale_window) {
            struct GrayscaleUBO { float uMix; float _pad[3]; };
            IMGUI_EFFECT_UNIFORM_STRUCT(GrayscaleUBO);
            GrayscaleUBO ubo{ s_grayscaleMix, { 0, 0, 0 } };
            effectSystem.SetEffectUniformStruct(grayscaleEffect, ubo);

            ImGuiRenderUX::EffectWindowScope scope(effectSystem, "Grayscale", grayscaleEffect, &show_grayscale_window);
            if (scope) {
                ImGui::TextUnformatted("EffectWindowScope + uniform uMix (std140 binding 2).");
                ImGui::SliderFloat("Grayscale mix", &s_grayscaleMix, 0.0f, 1.0f);
                ImGui::Checkbox("Effect system debug", &show_effect_debug);
                ImGui::Separator();
                ImVec2 p0 = ImGui::GetCursorScreenPos();
                float sz = 60.f * dpi_scale;
                float g = 8.f * dpi_scale;
                fx::draw_effect_preview_quads(ImGui::GetWindowDrawList(), whiteTexImId, p0, sz, g);
                ImGui::TextUnformatted("Quads: lerp(color, gray, mix). Text uses font (skipped by default).");
            }
        }

        // Separate window: effect applies only to BeginEffectDrawRegion / EndEffectDrawRegion span (not whole window).
        if (grayscaleEffect.IsValid() && show_region_demo_window) {
            struct GrayscaleUBO { float uMix; float _pad[3]; };
            IMGUI_EFFECT_UNIFORM_STRUCT(GrayscaleUBO);
            GrayscaleUBO ubo{ s_grayscaleMix, { 0, 0, 0 } };
            effectSystem.SetEffectUniformStruct(grayscaleEffect, ubo);
            ImGui::Begin("Grayscale region only", &show_region_demo_window);
            ImGui::TextUnformatted("Only the orange quad below is in the effect region.");
            {
                ImGuiRenderUX::EffectDrawRegionScope regionScope(effectSystem, grayscaleEffect);
                if (regionScope) {
                    ImVec2 p0 = ImGui::GetCursorScreenPos();
                    float sz = 48.f * dpi_scale;
                    ImGui::GetWindowDrawList()->AddImage(whiteTexImId, p0, ImVec2(p0.x + sz, p0.y + sz),
                                                         ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 180, 60, 255));
                    ImGui::Dummy(ImVec2(sz, sz));
                }
            }
            ImGui::TextUnformatted("This line is outside the region (no effect pass on font).");
            ImGui::End();
        }

        // Gallery: one scrollable window with a row per post-process (each row is its own EffectDrawRegionScope).
        if (show_effect_gallery) {
            ImGui::SetNextWindowSize(ImVec2(420.f * dpi_scale, 520.f * dpi_scale), ImGuiCond_FirstUseEver);
            if (ImGui::Begin("Effect gallery", &show_effect_gallery)) {
                ImGui::TextUnformatted("Each row uses a different Slang effect on the RGB quads.");
                ImGui::Separator();
                const float sz = 44.f * dpi_scale;
                const float gap = 6.f * dpi_scale;

                if (sepiaEffect.IsValid()) {
                    struct UBO { float uMix; float _pad[3]; };
                    IMGUI_EFFECT_UNIFORM_STRUCT(UBO);
                    effectSystem.SetEffectUniformStruct(sepiaEffect, UBO{ s_sepiaMix, {} });
                    ImGui::SliderFloat("Sepia mix", &s_sepiaMix, 0.f, 1.f);
                    ImGuiRenderUX::EffectDrawRegionScope row(effectSystem, sepiaEffect);
                    if (row)
                        fx::draw_effect_preview_quads(ImGui::GetWindowDrawList(), whiteTexImId, ImGui::GetCursorScreenPos(), sz, gap);
                    ImGui::Separator();
                }

                if (pixelateEffect.IsValid()) {
                    struct UBO { float uBlocks; float _pad[3]; };
                    IMGUI_EFFECT_UNIFORM_STRUCT(UBO);
                    effectSystem.SetEffectUniformStruct(pixelateEffect, UBO{ s_pixelateBlocks, {} });
                    ImGui::SliderFloat("Pixelate cells", &s_pixelateBlocks, 2.f, 64.f);
                    ImGuiRenderUX::EffectDrawRegionScope row(effectSystem, pixelateEffect);
                    if (row)
                        fx::draw_effect_preview_quads(ImGui::GetWindowDrawList(), whiteTexImId, ImGui::GetCursorScreenPos(), sz, gap);
                    ImGui::Separator();
                }

                if (chromaticEffect.IsValid()) {
                    struct UBO { float uStrength; float _pad[3]; };
                    IMGUI_EFFECT_UNIFORM_STRUCT(UBO);
                    effectSystem.SetEffectUniformStruct(chromaticEffect, UBO{ s_chromaStrength, {} });
                    ImGui::SliderFloat("Chromatic UV shift", &s_chromaStrength, 0.f, 0.025f, "%.4f");
                    ImGuiRenderUX::EffectDrawRegionScope row(effectSystem, chromaticEffect);
                    if (row)
                        fx::draw_effect_preview_quads(ImGui::GetWindowDrawList(), whiteTexImId, ImGui::GetCursorScreenPos(), sz, gap);
                    ImGui::Separator();
                }

                if (invertEffect.IsValid()) {
                    struct UBO { float uMix; float _pad[3]; };
                    IMGUI_EFFECT_UNIFORM_STRUCT(UBO);
                    effectSystem.SetEffectUniformStruct(invertEffect, UBO{ s_invertMix, {} });
                    ImGui::SliderFloat("Invert mix", &s_invertMix, 0.f, 1.f);
                    ImGuiRenderUX::EffectDrawRegionScope row(effectSystem, invertEffect);
                    if (row)
                        fx::draw_effect_preview_quads(ImGui::GetWindowDrawList(), whiteTexImId, ImGui::GetCursorScreenPos(), sz, gap);
                    ImGui::Separator();
                }

                if (scanlinesEffect.IsValid()) {
                    struct UBO { float uIntensity, uLines; float _pad[2]; };
                    IMGUI_EFFECT_UNIFORM_STRUCT(UBO);
                    effectSystem.SetEffectUniformStruct(scanlinesEffect, UBO{ s_scanlineIntensity, s_scanlineLines, {} });
                    ImGui::SliderFloat("Scanline darken", &s_scanlineIntensity, 0.f, 0.85f);
                    ImGui::SliderFloat("Scanline bands", &s_scanlineLines, 20.f, 400.f);
                    ImGuiRenderUX::EffectDrawRegionScope row(effectSystem, scanlinesEffect);
                    if (row)
                        fx::draw_effect_preview_quads(ImGui::GetWindowDrawList(), whiteTexImId, ImGui::GetCursorScreenPos(), sz, gap);
                }

                if (figmaFillEffect.IsValid()) {
                    ImGui::Separator();
                    ImGui::TextUnformatted("Figma fills (Linear, Radial, Angular, Diamond) — UV 0..1");
                    ImGui::Combo("Type", &s_figmaGtype, "Linear\0Radial\0Angular\0Diamond\0");
                    ImGui::Combo("Spread", &s_figmaSpread, "Pad (clamp)\0Repeat\0Reflect\0");
                    ImGui::SliderFloat("Dither (banding)", &s_figmaDither, 0.f, 1.f);
                    ImGui::SliderFloat("Frame rotate (deg)", &s_figmaFrameRotDeg, -180.f, 180.f);
                    ImGui::SliderFloat("Frame scale X", &s_figmaFrameSx, 0.2f, 3.f);
                    ImGui::SliderFloat("Frame scale Y", &s_figmaFrameSy, 0.2f, 3.f);
                    ImGui::SliderFloat("Frame translate X", &s_figmaFrameTx, -0.5f, 0.5f);
                    ImGui::SliderFloat("Frame translate Y", &s_figmaFrameTy, -0.5f, 0.5f);
                    if (figma_palette_16f_ok) {
                        ImGui::Checkbox("Palette RGBA16F (linear, less banding)", &s_figmaPalette16f);
                    } else {
                        ImGui::TextUnformatted("RGBA16F palette: GL rejected (using 8-bit sRGB texels).");
                        s_figmaPalette16f = false;
                    }
                    ImGui::SliderInt("Color stops", &s_figmaStopCount, 2, fx::kFigmaMaxStops);

                    IMGUI_EFFECT_UNIFORM_STRUCT(fx::FigmaUnifiedGradientUBO);
                    for (int si = 0; si < s_figmaStopCount; ++si) {
                        ImGui::PushID(si);
                        ImGui::ColorEdit4("Color", s_figmaC[si], ImGuiColorEditFlags_Float);
                        ImGui::DragFloat("Pos", &s_figmaS[si], 0.01f, 0.f, 1.f);
                        ImGui::PopID();
                    }
                    ImGui::SliderFloat("Blend fill over pixmap", &s_figmaReplace, 0.f, 1.f);

                    if (s_figmaGtype == 0) {
                        ImGui::TextUnformatted("Linear: handles A/B (like Figma endpoints, normalized UV).");
                        ImGui::SliderFloat("Handle A X", &s_figmaLinA[0], 0.f, 1.f);
                        ImGui::SliderFloat("Handle A Y", &s_figmaLinA[1], 0.f, 1.f);
                        ImGui::SliderFloat("Handle B X", &s_figmaLinB[0], 0.f, 1.f);
                        ImGui::SliderFloat("Handle B Y", &s_figmaLinB[1], 0.f, 1.f);
                    } else {
                        ImGui::TextUnformatted("Center / ellipse half-axes in UV (radial ellipse, angular pivot, diamond scale).");
                        ImGui::SliderFloat("Center X", &s_figmaCenter[0], 0.f, 1.f);
                        ImGui::SliderFloat("Center Y", &s_figmaCenter[1], 0.f, 1.f);
                        ImGui::SliderFloat("Ellipse X", &s_figmaEllipse[0], 0.02f, 1.5f);
                        ImGui::SliderFloat("Ellipse Y", &s_figmaEllipse[1], 0.02f, 1.5f);
                        if (s_figmaGtype == 2)
                            ImGui::SliderFloat("Rotate gradient start (deg)", &s_figmaAngleDeg, -180.f, 180.f);
                    }

                    effectSystem.SetEffectPaletteTexture(figmaFillEffect, (ImTextureID)(intptr_t)figma_palette_gl);
                    const bool use16fPal = figma_palette_16f_ok && s_figmaPalette16f;
                    fx::rebuild_figma_palette_texture(figma_palette_gl, fx::kFigmaPaletteTexWidth, s_figmaStopCount, s_figmaC,
                                               s_figmaS, use16fPal);

                    fx::FigmaUnifiedGradientUBO gu{};
                    gu.linearAx = s_figmaLinA[0];
                    gu.linearAy = s_figmaLinA[1];
                    gu.linearBx = s_figmaLinB[0];
                    gu.linearBy = s_figmaLinB[1];
                    gu.centerX = s_figmaCenter[0];
                    gu.centerY = s_figmaCenter[1];
                    gu.ellipseX = s_figmaEllipse[0];
                    gu.ellipseY = s_figmaEllipse[1];
                    gu.gtype = static_cast<float>(s_figmaGtype);
                    gu.replaceMix = s_figmaReplace;
                    gu.angleStart = s_figmaAngleDeg * (3.14159265359f / 180.f);
                    gu.paletteWidth = static_cast<float>(fx::kFigmaPaletteTexWidth);
                    gu.spreadMode = static_cast<float>(s_figmaSpread);
                    gu.ditherStrength = s_figmaDither;
                    fx::figma_build_gradient_affine(s_figmaFrameRotDeg, s_figmaFrameSx, s_figmaFrameSy, s_figmaFrameTx,
                                            s_figmaFrameTy, &gu.gradM0);
                    gu.paletteLinear = use16fPal ? 1.f : 0.f;
                    gu._pad0 = gu._pad1 = gu._pad2 = 0.f;

                    effectSystem.SetEffectUniformStruct(figmaFillEffect, gu);

                    ImGuiRenderUX::EffectDrawRegionScope row(effectSystem, figmaFillEffect);
                    if (row) {
                        ImVec2 p0 = ImGui::GetCursorScreenPos();
                        float gw = 300.f * dpi_scale;
                        float gh = 140.f * dpi_scale;
                        ImGui::GetWindowDrawList()->AddImage(whiteTexImId, p0, ImVec2(p0.x + gw, p0.y + gh),
                                                             ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 255));
                        ImGui::Dummy(ImVec2(gw, gh));
                    }
                    ImGui::TextWrapped(
                        "Figma-like fills: linear / radial / angular / diamond in local UV, optional spread (pad|repeat|reflect), "
                        "UV frame affine (rotate/scale/translate), per-stop sRGB-linear interpolation, alpha in palette, dither, "
                        "optional RGBA16F palette. Not a full vector engine: no mesh warp, boolean fills, or plugin JSON import "
                        "(see docs/FIGMA_GRADIENT.md).");
                }
            }
            ImGui::End();
        }

        if (show_effect_debug)
            effectSystem.ShowDebugWindow(&show_effect_debug);

        {
            ImGuiRenderUX::EffectSubmitGuard submitGuard(effectSystem);
            (void)submitGuard;
        }

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

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_context);
        }

        glfwSwapBuffers(window);
    }

    if (figma_palette_gl != 0)
        glDeleteTextures(1, &figma_palette_gl);
    builtinTextures.Destroy();
    ImGui_ImplOpenGL3Slang_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
