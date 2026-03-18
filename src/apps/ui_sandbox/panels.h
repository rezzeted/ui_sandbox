#pragma once

#include "editor_layout.h"
#include <drui/drui.h>

#include <imgui.h>
#include <cstdio>
#include <cmath>
#include <string>
#include <vector>

// ===================================================================
// Widget type selection (for canvas preview)
// ===================================================================

enum class WidgetType {
    None = 0,
    Button, IconButton, IconToggle, ColoredButton,
    ToggleSwitch, InputText, InputTextMultiline,
    InputInt, InputFloat, TagInput,
    SliderFloat, SliderInt, ProgressBar, Spinner,
    Checkbox, RadioButton, Combo, ColorEdit4,
    ShimmerText, GradientBorder, GlowGradientBorder,
    SkeuomorphCard,
    SkeuomorphSlider,
    SkeuoButton, SkeuoToggle, SkeuoCheckbox,
    SkeuoRadio, SkeuoInput, SkeuoProgress,
};

static WidgetType g_selected_widget = WidgetType::None;

// ===================================================================
// Main menu bar
// ===================================================================

static void DrawMainMenuBar(PanelVisibility& panels) {
    if (!ImGui::BeginMainMenuBar()) return;

    if (ImGui::BeginMenu(ICON_FA_FILE "  \xd0\xa4\xd0\xb0\xd0\xb9\xd0\xbb")) {
        ImGui::MenuItem(ICON_FA_FILE " \xd0\x9d\xd0\xbe\xd0\xb2\xd1\x8b\xd0\xb9", "Ctrl+N");
        ImGui::MenuItem(ICON_FA_FOLDER_OPEN " \xd0\x9e\xd1\x82\xd0\xba\xd1\x80\xd1\x8b\xd1\x82\xd1\x8c...", "Ctrl+O");
        ImGui::MenuItem(ICON_FA_SAVE " \xd0\xa1\xd0\xbe\xd1\x85\xd1\x80\xd0\xb0\xd0\xbd\xd0\xb8\xd1\x82\xd1\x8c", "Ctrl+S");
        ImGui::Separator();
        ImGui::MenuItem(ICON_FA_TIMES " \xd0\x92\xd1\x8b\xd1\x85\xd0\xbe\xd0\xb4");
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu(ICON_FA_EDIT "  \xd0\x9f\xd1\x80\xd0\xb0\xd0\xb2\xd0\xba\xd0\xb0")) {
        ImGui::MenuItem(ICON_FA_UNDO " \xd0\x9e\xd1\x82\xd0\xbc\xd0\xb5\xd0\xbd\xd0\xb8\xd1\x82\xd1\x8c", "Ctrl+Z");
        ImGui::MenuItem(ICON_FA_REDO " \xd0\x9f\xd0\xbe\xd0\xb2\xd1\x82\xd0\xbe\xd1\x80\xd0\xb8\xd1\x82\xd1\x8c", "Ctrl+Y");
        ImGui::Separator();
        ImGui::MenuItem(ICON_FA_COPY " \xd0\x9a\xd0\xbe\xd0\xbf\xd0\xb8\xd1\x80\xd0\xbe\xd0\xb2\xd0\xb0\xd1\x82\xd1\x8c", "Ctrl+C");
        ImGui::MenuItem(ICON_FA_EDIT " \xd0\x92\xd1\x81\xd1\x82\xd0\xb0\xd0\xb2\xd0\xb8\xd1\x82\xd1\x8c", "Ctrl+V");
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu(ICON_FA_EYE "  \xd0\x92\xd0\xb8\xd0\xb4")) {
        ImGui::MenuItem(ICON_FA_COG " \xd0\x92\xd0\xb8\xd0\xb4\xd0\xb6\xd0\xb5\xd1\x82\xd1\x8b", nullptr, &panels.tab_widgets);
        ImGui::MenuItem(ICON_FA_BOLT " Custom", nullptr, &panels.tab_custom);
        ImGui::Separator();
        ImGui::MenuItem(ICON_FA_BARS " \xd0\xa1\xd1\x82\xd1\x80\xd0\xbe\xd0\xba\xd0\xb0 \xd1\x81\xd0\xbe\xd1\x81\xd1\x82\xd0\xbe\xd1\x8f\xd0\xbd\xd0\xb8\xd1\x8f", nullptr, &panels.status_bar);
        ImGui::Separator();
        if (ImGui::BeginMenu(ICON_FA_EYE " \xd0\xa2\xd0\xb5\xd0\xbc\xd0\xb0")) {
            for (int i = 0; i < (int)DrUI::ThemeId::COUNT; ++i) {
                auto tid = static_cast<DrUI::ThemeId>(i);
                bool selected = (DrUI::g_current_theme == tid);
                if (ImGui::MenuItem(DrUI::ThemeName(tid), nullptr, selected)) {
                    DrUI::ApplyTheme(tid, panels.dpi_scale);
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu(ICON_FA_BUG "  \xd0\x9e\xd1\x82\xd0\xbb\xd0\xb0\xd0\xb4\xd0\xba\xd0\xb0")) {
        ImGui::MenuItem(ICON_FA_PLAY " \xd0\x97\xd0\xb0\xd0\xbf\xd1\x83\xd1\x81\xd0\xba");
        ImGui::MenuItem(ICON_FA_STEP_FORWARD " \xd0\xa8\xd0\xb0\xd0\xb3");
        ImGui::MenuItem(ICON_FA_STOP " \xd0\xa1\xd0\xb1\xd1\x80\xd0\xbe\xd1\x81");
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu(ICON_FA_INFO_CIRCLE "  \xd0\xa1\xd0\xbf\xd1\x80\xd0\xb0\xd0\xb2\xd0\xba\xd0\xb0")) {
        if (ImGui::MenuItem("\xd0\x9e \xd0\xbf\xd1\x80\xd0\xbe\xd0\xb3\xd1\x80\xd0\xb0\xd0\xbc\xd0\xbc\xd0\xb5")) {
            DrUI::ShowToast("UI Sandbox v1.0  \xc2\xb7  ImGui " IMGUI_VERSION,
                      DrUI::ToastType::Info, 4.0f);
        }
        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
}

// ===================================================================
// Left panel — tabs: Widgets showcase + Skeuomorph
// ===================================================================

static void DrawWidgetsTab(float dpi_scale) {
    ImGui::TextDisabled(ICON_FA_COG " \xd0\x92\xd0\xb8\xd1\x82\xd1\x80\xd0\xb8\xd0\xbd\xd0\xb0 \xd0\xb2\xd0\xb8\xd0\xb4\xd0\xb6\xd0\xb5\xd1\x82\xd0\xbe\xd0\xb2");
    DrUI::GradientSeparator();

    ImGui::BeginChild("##widgets_scroll", ImVec2(0, 0), false);

    #define W_SEL(t) if (ImGui::IsItemClicked()) g_selected_widget = WidgetType::t

    DrUI::CardBegin(ICON_FA_BOLT " \xd0\x9a\xd0\xbd\xd0\xbe\xd0\xbf\xd0\xba\xd0\xb8 \xd0\xb8 \xd0\xb2\xd0\xb2\xd0\xbe\xd0\xb4", false);
    {
        if (DrUI::Button(ICON_FA_PLUS " \xd0\x94\xd0\xbe\xd0\xb1\xd0\xb0\xd0\xb2\xd0\xb8\xd1\x82\xd1\x8c"))  {}
        W_SEL(Button);
        ImGui::SameLine();
        if (DrUI::Button(ICON_FA_TRASH " \xd0\xa3\xd0\xb4\xd0\xb0\xd0\xbb\xd0\xb8\xd1\x82\xd1\x8c"))  {}
        W_SEL(Button);
        ImGui::SameLine();
        if (DrUI::Button(ICON_FA_SAVE " \xd0\xa1\xd0\xbe\xd1\x85\xd1\x80\xd0\xb0\xd0\xbd\xd0\xb8\xd1\x82\xd1\x8c"))  {}
        W_SEL(Button);

        ImGui::Spacing();
        if (DrUI::Button(ICON_FA_PLAY, "\xd0\x97\xd0\xb0\xd0\xbf\xd1\x83\xd1\x81\xd0\xba"))  {}
        W_SEL(Button);
        ImGui::SameLine();
        if (DrUI::Button(ICON_FA_STOP, "\xd0\xa1\xd1\x82\xd0\xbe\xd0\xbf"))  {}
        W_SEL(Button);

        ImGui::Spacing();
        if (DrUI::IconButton(ICON_FA_UNDO))  {}
        W_SEL(IconButton);
        ImGui::SameLine();
        if (DrUI::IconButton(ICON_FA_REDO))  {}
        W_SEL(IconButton);
        DrUI::Badge(3);
        ImGui::SameLine();
        if (DrUI::IconButton(ICON_FA_SEARCH))  {}
        W_SEL(IconButton);
        ImGui::SameLine();
        if (DrUI::IconButton(ICON_FA_COG))  {}
        W_SEL(IconButton);
        DrUI::Badge(12);

        ImGui::Spacing();
        static bool tog_grid = true, tog_snap = false, tog_eye = true;
        DrUI::IconToggle(ICON_FA_DICE, &tog_grid);
        W_SEL(IconToggle);
        ImGui::SameLine();
        DrUI::IconToggle(ICON_FA_ROUTE, &tog_snap);
        W_SEL(IconToggle);
        ImGui::SameLine();
        DrUI::IconToggle(ICON_FA_EYE, &tog_eye);
        W_SEL(IconToggle);

        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Button, DrUI::Colors::Success);
        DrUI::Button(ICON_FA_CHECK " OK");
        W_SEL(ColoredButton);
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, DrUI::Colors::Warning);
        DrUI::Button(ICON_FA_WARNING " \xd0\x92\xd0\xbd\xd0\xb8\xd0\xbc\xd0\xb0\xd0\xbd\xd0\xb8\xd0\xb5");
        W_SEL(ColoredButton);
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, DrUI::Colors::Error);
        DrUI::Button(ICON_FA_TIMES " \xd0\x9e\xd1\x88\xd0\xb8\xd0\xb1\xd0\xba\xd0\xb0");
        W_SEL(ColoredButton);
        ImGui::PopStyleColor();

        DrUI::GradientSeparator();

        static bool dark_mode = true, autosave = false, notif = true;
        DrUI::ToggleSwitch("dark_mode", &dark_mode);
        W_SEL(ToggleSwitch);
        ImGui::SameLine();
        ImGui::Text(ICON_FA_EYE " \xd0\xa2\xd1\x91\xd0\xbc\xd0\xbd\xd0\xb0\xd1\x8f \xd1\x82\xd0\xb5\xd0\xbc\xd0\xb0");
        DrUI::ToggleSwitch("autosave", &autosave);
        W_SEL(ToggleSwitch);
        ImGui::SameLine();
        ImGui::Text(ICON_FA_SAVE " \xd0\x90\xd0\xb2\xd1\x82\xd0\xbe\xd1\x81\xd0\xbe\xd1\x85\xd1\x80\xd0\xb0\xd0\xbd\xd0\xb5\xd0\xbd\xd0\xb8\xd0\xb5");
        DrUI::ToggleSwitch("notif", &notif);
        W_SEL(ToggleSwitch);
        ImGui::SameLine();
        ImGui::Text(ICON_FA_BOLT " \xd0\xa3\xd0\xb2\xd0\xb5\xd0\xb4\xd0\xbe\xd0\xbc\xd0\xbb\xd0\xb5\xd0\xbd\xd0\xb8\xd1\x8f");

        DrUI::GradientSeparator();

        static char text_buf[256] = "Hello, UI Sandbox!";
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##text", text_buf, sizeof(text_buf));
        W_SEL(InputText);

        static char multi_buf[1024] =
            "\xd0\x9c\xd0\xbd\xd0\xbe\xd0\xb3\xd0\xbe\xd1\x81\xd1\x82\xd1\x80"
            "\xd0\xbe\xd1\x87\xd0\xbd\xd1\x8b\xd0\xb9\n\xd1\x82\xd0\xb5\xd0\xba"
            "\xd1\x81\xd1\x82";
        ImGui::InputTextMultiline("##multi", multi_buf, sizeof(multi_buf),
                                  ImVec2(-1, 60.0f * dpi_scale));
        W_SEL(InputTextMultiline);

        static int int_val = 42;
        DrUI::InputInt("Int", &int_val, 1, 10, 80.0f * dpi_scale);
        W_SEL(InputInt);

        static float float_val = 3.14f;
        DrUI::InputFloat("Float", &float_val, 0.1f, 1.0f, "%.2f",
                         80.0f * dpi_scale);
        W_SEL(InputFloat);

        DrUI::GradientSeparator();

        static std::vector<std::string> demo_tags = {"ui_sandbox", "imgui", "C++20"};
        static char tag_buf[64] = "";
        ImGui::TextDisabled(ICON_FA_BOLT " Tag Input");
        DrUI::TagInput("##tags", demo_tags, tag_buf, sizeof(tag_buf));
        W_SEL(TagInput);
    }
    DrUI::CardEnd();

    DrUI::CardBegin(ICON_FA_EYE " \xd0\xa1\xd0\xbb\xd0\xb0\xd0\xb9\xd0\xb4\xd0\xb5\xd1\x80\xd1\x8b \xd0\xb8 \xd0\xb2\xd1\x8b\xd0\xb1\xd0\xbe\xd1\x80", false);
    {
        static float slider1 = 0.5f;
        DrUI::SliderFloat("\xd0\x97\xd0\xbd\xd0\xb0\xd1\x87\xd0\xb5\xd0\xbd\xd0\xb8\xd0\xb5",
                          &slider1, 0.0f, 1.0f);
        W_SEL(SliderFloat);

        static int slider_int = 50;
        DrUI::SliderInt("\xd0\xa8\xd0\xb0\xd0\xb3\xd0\xb8",
                        &slider_int, 0, 100);
        W_SEL(SliderInt);

        static float progress = 0.65f;
        DrUI::ProgressBar(progress, ImVec2(-1, 0), "65%");
        W_SEL(ProgressBar);

        ImGui::Spacing();
        DrUI::Spinner("##spin1", 8.0f, 2.5f);
        W_SEL(Spinner);
        ImGui::SameLine();
        ImGui::TextDisabled("\xd0\x97\xd0\xb0\xd0\xb3\xd1\x80\xd1\x83\xd0\xb7\xd0\xba\xd0\xb0...");

        DrUI::GradientSeparator();

        static bool chk1 = true, chk2 = false, chk3 = true;
        ImGui::Checkbox("\xd0\x9e\xd0\xbf\xd1\x86\xd0\xb8\xd1\x8f A", &chk1);
        W_SEL(Checkbox);
        ImGui::Checkbox("\xd0\x9e\xd0\xbf\xd1\x86\xd0\xb8\xd1\x8f B", &chk2);
        W_SEL(Checkbox);
        ImGui::Checkbox("\xd0\x9e\xd0\xbf\xd1\x86\xd0\xb8\xd1\x8f C", &chk3);
        W_SEL(Checkbox);

        static int radio = 0;
        ImGui::RadioButton("\xd0\xa0\xd0\xb5\xd0\xb6\xd0\xb8\xd0\xbc 1", &radio, 0);
        W_SEL(RadioButton);
        ImGui::SameLine();
        ImGui::RadioButton("\xd0\xa0\xd0\xb5\xd0\xb6\xd0\xb8\xd0\xbc 2", &radio, 1);
        W_SEL(RadioButton);
        ImGui::SameLine();
        ImGui::RadioButton("\xd0\xa0\xd0\xb5\xd0\xb6\xd0\xb8\xd0\xbc 3", &radio, 2);
        W_SEL(RadioButton);

        static int combo_sel = 0;
        const char* combo_items[] = {"Sequence", "Silhouette", "Custom"};
        ImGui::SetNextItemWidth(150.0f * dpi_scale);
        ImGui::Combo("\xd0\xa2\xd0\xb8\xd0\xbf", &combo_sel, combo_items, 3);
        W_SEL(Combo);

        DrUI::GradientSeparator();

        static ImVec4 col1(0.37f, 0.55f, 0.95f, 1.0f);
        static ImVec4 col2(0.63f, 0.45f, 0.98f, 1.0f);
        ImGui::ColorEdit4("Accent 1", &col1.x);
        W_SEL(ColorEdit4);
        ImGui::ColorEdit4("Accent 2", &col2.x);
        W_SEL(ColorEdit4);
    }
    DrUI::CardEnd();

    #undef W_SEL

    ImGui::EndChild();
}

static void DrawCustomTab(float dpi_scale) {
    ImGui::TextDisabled(ICON_FA_BOLT " Custom \xd0\xb2\xd0\xb8\xd0\xb4\xd0\xb6\xd0\xb5\xd1\x82\xd1\x8b");
    DrUI::GradientSeparator();

    ImGui::BeginChild("##custom_scroll", ImVec2(0, 0), false);

    #define W_SEL(t) if (ImGui::IsItemClicked()) g_selected_widget = WidgetType::t

    DrUI::CardBegin(ICON_FA_BOLT " Shimmer Text", false);
    {
        DrUI::ShimmerText("Hello, Shimmer World!");
        W_SEL(ShimmerText);
        ImGui::Spacing();
        DrUI::ShimmerText("\xd0\x9f\xd0\xb5\xd1\x80\xd0\xb5\xd0\xbb\xd0\xb8\xd0\xb2\xd0\xb0\xd1\x8e\xd1\x89\xd0\xb8\xd0\xb9\xd1\x81\xd1\x8f \xd1\x82\xd0\xb5\xd0\xba\xd1\x81\xd1\x82 \xd0\xbd\xd0\xb0 \xd1\x80\xd1\x83\xd1\x81\xd1\x81\xd0\xba\xd0\xbe\xd0\xbc");
        W_SEL(ShimmerText);
        ImGui::Spacing();
        DrUI::ShimmerText(ICON_FA_COG " AI is thinking...", 1.2f, 0.25f);
        W_SEL(ShimmerText);
        ImGui::Spacing();
        DrUI::ShimmerText("Slow shimmer demo", 4.0f, 0.5f);
        W_SEL(ShimmerText);
    }
    DrUI::CardEnd();

    DrUI::CardBegin(ICON_FA_EYE " Gradient Border", false);
    {
        static const ImVec4 border_colors[] = {
            {0.90f, 0.30f, 0.50f, 1.0f},
            {0.30f, 0.80f, 0.95f, 1.0f},
            {0.95f, 0.75f, 0.20f, 1.0f},
            {0.40f, 0.90f, 0.50f, 1.0f},
        };
        float t = (float)ImGui::GetTime();
        ImDrawList* wdl = ImGui::GetWindowDrawList();
        float avail_w = ImGui::GetContentRegionAvail().x;

        ImGui::TextDisabled("GradientBorder");
        {
            ImVec2 rp = ImGui::GetCursorScreenPos();
            float rh = 60.0f * dpi_scale;
            ImGui::Dummy(ImVec2(avail_w, rh));
            W_SEL(GradientBorder);
            DrUI::GradientBorder(wdl, rp, ImVec2(avail_w, rh),
                                 border_colors, 4, t, 0.2f, 2.0f, 10.0f);
        }

        ImGui::Spacing();
        ImGui::TextDisabled("GlowGradientBorder");
        {
            float pad = 14.0f;
            ImVec2 rp = ImGui::GetCursorScreenPos();
            rp.x += pad;
            rp.y += pad;
            float rh = 60.0f * dpi_scale;
            ImGui::Dummy(ImVec2(avail_w, rh + pad * 2.0f));
            W_SEL(GlowGradientBorder);
            DrUI::GlowGradientBorder(wdl, rp, ImVec2(avail_w - pad * 2.0f, rh),
                                     border_colors, 4, t, 0.2f, 2.0f, 10.0f);
        }
    }
    DrUI::CardEnd();

    #undef W_SEL

    ImGui::EndChild();
}

static bool g_card_active = false;

static void DrawSkeuomorphCardImpl(ImDrawList* dl, ImVec2 cp0,
                                    float card_sz, bool active, float zm) {
    constexpr float PI = 3.14159265f;
    ImVec2 cp1(cp0.x + card_sz, cp0.y + card_sz);
    float rnd = 24.0f * zm;
    float pad = 22.0f * zm;

    if (active) {
        dl->AddRectFilled(cp0, cp1, IM_COL32(46, 113, 249, 255), rnd);

        {
            constexpr int passes = 6;
            constexpr float step = 1.2f;
            for (int i = 1; i <= passes; ++i) {
                float off = (float)i * step * zm;
                float a_frac = 1.0f - (float)(i - 1) / (float)passes;
                int d_alpha = (int)(45.0f * a_frac * a_frac);
                int l_alpha = (int)(30.0f * a_frac * a_frac);
                float lw = 1.5f * zm;
                float r = rnd - off;
                if (r < 1.0f) continue;

                dl->AddLine(ImVec2(cp0.x + rnd, cp0.y + off),
                            ImVec2(cp1.x - rnd, cp0.y + off),
                            IM_COL32(0, 0, 0, d_alpha), lw);
                dl->AddLine(ImVec2(cp0.x + off, cp0.y + rnd),
                            ImVec2(cp0.x + off, cp1.y - rnd),
                            IM_COL32(0, 0, 0, d_alpha), lw);
                dl->AddLine(ImVec2(cp0.x + rnd, cp1.y - off),
                            ImVec2(cp1.x - rnd, cp1.y - off),
                            IM_COL32(255, 255, 255, l_alpha), lw);
                dl->AddLine(ImVec2(cp1.x - off, cp0.y + rnd),
                            ImVec2(cp1.x - off, cp1.y - rnd),
                            IM_COL32(255, 255, 255, l_alpha), lw);

                dl->PathArcTo(ImVec2(cp0.x + rnd, cp0.y + rnd), r,
                              PI, PI * 1.5f, 12);
                dl->PathStroke(IM_COL32(0, 0, 0, d_alpha), 0, lw);
                dl->PathArcTo(ImVec2(cp1.x - rnd, cp1.y - rnd), r,
                              0, PI * 0.5f, 12);
                dl->PathStroke(IM_COL32(255, 255, 255, l_alpha), 0, lw);

                {
                    constexpr int segs = 12;
                    ImVec2 ctr(cp1.x - rnd, cp0.y + rnd);
                    float a0 = PI * 1.5f, a1 = PI * 2.0f;
                    for (int s = 0; s < segs; ++s) {
                        float t0 = (float)s / (float)segs;
                        float t1 = (float)(s + 1) / (float)segs;
                        float tmid = (t0 + t1) * 0.5f;
                        int sv = (int)(255.0f * tmid);
                        int sa = (int)(d_alpha + (l_alpha - d_alpha) * tmid);
                        float ang0 = a0 + (a1 - a0) * t0;
                        float ang1 = a0 + (a1 - a0) * t1;
                        dl->AddLine(
                            ImVec2(ctr.x + std::cos(ang0) * r, ctr.y + std::sin(ang0) * r),
                            ImVec2(ctr.x + std::cos(ang1) * r, ctr.y + std::sin(ang1) * r),
                            IM_COL32(sv, sv, sv, sa), lw);
                    }
                }
                {
                    constexpr int segs = 12;
                    ImVec2 ctr(cp0.x + rnd, cp1.y - rnd);
                    float a0 = PI, a1 = PI * 0.5f;
                    for (int s = 0; s < segs; ++s) {
                        float t0 = (float)s / (float)segs;
                        float t1 = (float)(s + 1) / (float)segs;
                        float tmid = (t0 + t1) * 0.5f;
                        int sv = (int)(255.0f * tmid);
                        int sa = (int)(d_alpha + (l_alpha - d_alpha) * tmid);
                        float ang0 = a0 + (a1 - a0) * t0;
                        float ang1 = a0 + (a1 - a0) * t1;
                        dl->AddLine(
                            ImVec2(ctr.x + std::cos(ang0) * r, ctr.y + std::sin(ang0) * r),
                            ImVec2(ctr.x + std::cos(ang1) * r, ctr.y + std::sin(ang1) * r),
                            IM_COL32(sv, sv, sv, sa), lw);
                    }
                }
            }
        }

        ImU32 wh = IM_COL32(255, 255, 255, 255);
        ImU32 wh70 = IM_COL32(255, 255, 255, 180);

        {
            float icx = cp0.x + pad + 14.0f * zm;
            float icy = cp0.y + pad + 14.0f * zm;
            float ri = 6.0f * zm, rri = 9.0f * zm, rro = 13.0f * zm;
            dl->AddCircle(ImVec2(icx, icy), ri, wh, 0, 2.0f * zm);
            for (int j = 0; j < 8; ++j) {
                float a = (float)j * (PI / 4.0f);
                dl->AddLine(ImVec2(icx + std::cos(a) * rri, icy + std::sin(a) * rri),
                            ImVec2(icx + std::cos(a) * rro, icy + std::sin(a) * rro),
                            wh, 2.0f * zm);
            }
        }
        {
            float dx = cp1.x - pad - 4.0f * zm;
            float dy = cp0.y + pad + 6.0f * zm;
            float dr = 2.2f * zm, dg = 8.0f * zm;
            dl->AddCircleFilled(ImVec2(dx, dy),      dr, wh70);
            dl->AddCircleFilled(ImVec2(dx, dy + dg), dr, wh70);
        }
        {
            ImFont* font = ImGui::GetFont();
            float fsz = ImGui::GetFontSize() * 1.5f * zm;
            ImVec2 tsz = font->CalcTextSizeA(fsz, FLT_MAX, 0, "Light");
            dl->AddText(font, fsz, ImVec2(cp0.x + pad, cp1.y - pad - 18.0f * zm - tsz.y), wh, "Light");
        }
        {
            ImFont* font = ImGui::GetFont();
            float fsz = ImGui::GetFontSize() * 0.95f * zm;
            ImVec2 tsz = font->CalcTextSizeA(fsz, FLT_MAX, 0, "Active");
            dl->AddText(font, fsz, ImVec2(cp0.x + pad, cp1.y - pad - tsz.y + 2.0f * zm), wh70, "Active");
        }
        {
            float tw = 44.0f * zm, th = 24.0f * zm, trnd = th * 0.5f;
            float ttx = cp1.x - pad - tw, tty = cp1.y - pad - th + 2.0f * zm;
            ImVec2 tp0(ttx, tty), tp1(ttx + tw, tty + th);
            dl->AddRectFilled(tp0, tp1, IM_COL32(42, 86, 196, 220), trnd);
            dl->AddRect(tp0, tp1, IM_COL32(100, 160, 240, 80), trnd, 0, 1.0f * zm);
            float kr = (th * 0.5f) - 3.0f * zm;
            dl->AddCircleFilled(ImVec2(tp1.x - th * 0.5f, tp0.y + th * 0.5f), kr, wh);
        }
    } else {
        ImU32 card_fill = IM_COL32(239, 241, 243, 255);

        {
            constexpr int passes = 12;
            constexpr float spread = 8.0f;
            float sdx = 3.0f * zm, sdy = 3.0f * zm;
            for (int i = passes; i >= 0; --i) {
                float frac = (float)i / (float)passes;
                float ex = spread * frac * zm;
                int alpha = (int)(18.0f * (1.0f - frac) * (1.0f - frac));
                dl->AddRectFilled(
                    ImVec2(cp0.x + sdx - ex, cp0.y + sdy - ex),
                    ImVec2(cp1.x + sdx + ex, cp1.y + sdy + ex),
                    IM_COL32(0, 0, 0, alpha), rnd + ex);
            }
        }
        {
            constexpr int passes = 12;
            constexpr float spread = 8.0f;
            float lox = -3.0f * zm, loy = -3.0f * zm;
            for (int i = passes; i >= 0; --i) {
                float frac = (float)i / (float)passes;
                float ex = spread * frac * zm;
                int alpha = (int)(40.0f * (1.0f - frac) * (1.0f - frac));
                dl->AddRectFilled(
                    ImVec2(cp0.x + lox - ex, cp0.y + loy - ex),
                    ImVec2(cp1.x + lox + ex, cp1.y + loy + ex),
                    IM_COL32(255, 255, 255, alpha), rnd + ex);
            }
        }

        dl->AddRectFilled(cp0, cp1, card_fill, rnd);

        ImU32 dk = IM_COL32(100, 105, 115, 255);
        ImU32 dk70 = IM_COL32(100, 105, 115, 140);

        {
            float icx = cp0.x + pad + 14.0f * zm;
            float icy = cp0.y + pad + 14.0f * zm;
            float ri = 6.0f * zm, rri = 9.0f * zm, rro = 13.0f * zm;
            dl->AddCircle(ImVec2(icx, icy), ri, dk, 0, 2.0f * zm);
            for (int j = 0; j < 8; ++j) {
                float a = (float)j * (PI / 4.0f);
                dl->AddLine(ImVec2(icx + std::cos(a) * rri, icy + std::sin(a) * rri),
                            ImVec2(icx + std::cos(a) * rro, icy + std::sin(a) * rro),
                            dk, 2.0f * zm);
            }
        }
        {
            float dx = cp1.x - pad - 4.0f * zm;
            float dy = cp0.y + pad + 6.0f * zm;
            float dr = 2.2f * zm, dg = 8.0f * zm;
            dl->AddCircleFilled(ImVec2(dx, dy),      dr, dk70);
            dl->AddCircleFilled(ImVec2(dx, dy + dg), dr, dk70);
        }
        {
            ImFont* font = ImGui::GetFont();
            float fsz = ImGui::GetFontSize() * 1.5f * zm;
            ImVec2 tsz = font->CalcTextSizeA(fsz, FLT_MAX, 0, "Light");
            dl->AddText(font, fsz, ImVec2(cp0.x + pad, cp1.y - pad - 18.0f * zm - tsz.y), dk, "Light");
        }
        {
            ImFont* font = ImGui::GetFont();
            float fsz = ImGui::GetFontSize() * 0.95f * zm;
            ImVec2 tsz = font->CalcTextSizeA(fsz, FLT_MAX, 0, "Inactive");
            dl->AddText(font, fsz, ImVec2(cp0.x + pad, cp1.y - pad - tsz.y + 2.0f * zm), dk70, "Inactive");
        }
        {
            float tw = 44.0f * zm, th = 24.0f * zm, trnd = th * 0.5f;
            float ttx = cp1.x - pad - tw, tty = cp1.y - pad - th + 2.0f * zm;
            ImVec2 tp0(ttx, tty), tp1(ttx + tw, tty + th);
            dl->AddRectFilled(tp0, tp1, IM_COL32(210, 212, 218, 220), trnd);
            dl->AddRect(tp0, tp1, IM_COL32(190, 192, 200, 100), trnd, 0, 1.0f * zm);
            float kr = (th * 0.5f) - 3.0f * zm;
            dl->AddCircleFilled(ImVec2(tp0.x + th * 0.5f, tp0.y + th * 0.5f), kr,
                                IM_COL32(255, 255, 255, 255));
        }
    }
}

static float g_skeuo_slider_val = 0.35f;

static void DrawSkeuomorphSliderImpl(ImDrawList* dl, ImVec2 pos,
                                      float width, float height,
                                      float* value, float zm) {
    float rnd = height * 0.5f;
    ImVec2 p0 = pos;
    ImVec2 p1(pos.x + width, pos.y + height);
    float cy = p0.y + height * 0.5f;

    float thumb_r = height * 0.58f;
    float thumb_margin = rnd;
    float thumb_range = width - thumb_margin * 2.0f;
    float thumb_x = p0.x + thumb_margin + thumb_range * (*value);

    // --- Outer shadows (skeuomorphic depth) ---
    {
        constexpr int passes = 10;
        constexpr float spread = 6.0f;
        float sdx = 2.0f * zm, sdy = 2.5f * zm;
        for (int i = passes; i >= 0; --i) {
            float frac = (float)i / (float)passes;
            float ex = spread * frac * zm;
            int alpha = (int)(14.0f * (1.0f - frac) * (1.0f - frac));
            dl->AddRectFilled(
                ImVec2(p0.x + sdx - ex, p0.y + sdy - ex),
                ImVec2(p1.x + sdx + ex, p1.y + sdy + ex),
                IM_COL32(0, 0, 0, alpha), rnd + ex);
        }
    }
    {
        constexpr int passes = 10;
        constexpr float spread = 6.0f;
        float lox = -2.0f * zm, loy = -2.0f * zm;
        for (int i = passes; i >= 0; --i) {
            float frac = (float)i / (float)passes;
            float ex = spread * frac * zm;
            int alpha = (int)(35.0f * (1.0f - frac) * (1.0f - frac));
            dl->AddRectFilled(
                ImVec2(p0.x + lox - ex, p0.y + loy - ex),
                ImVec2(p1.x + lox + ex, p1.y + loy + ex),
                IM_COL32(255, 255, 255, alpha), rnd + ex);
        }
    }

    // --- White track (full capsule) ---
    dl->AddRectFilled(p0, p1, IM_COL32(239, 241, 243, 255), rnd);

    // Inner shadow (top darker) / highlight (bottom lighter)
    {
        constexpr int passes = 4;
        for (int i = 1; i <= passes; ++i) {
            float off = (float)i * 1.0f * zm;
            float a = 1.0f - (float)(i - 1) / (float)passes;
            int da = (int)(18.0f * a * a);
            int la = (int)(35.0f * a * a);
            float lw = 1.0f * zm;
            dl->AddLine(ImVec2(p0.x + rnd, p0.y + off),
                        ImVec2(p1.x - rnd, p0.y + off),
                        IM_COL32(0, 0, 0, da), lw);
            dl->AddLine(ImVec2(p0.x + rnd, p1.y - off),
                        ImVec2(p1.x - rnd, p1.y - off),
                        IM_COL32(255, 255, 255, la), lw);
        }
    }

    // --- Blue fill (left portion up to thumb) ---
    {
        float blue_end = thumb_x;
        if (blue_end > p0.x + 1.0f) {
            dl->PushClipRect(p0, ImVec2(blue_end, p1.y), true);
            dl->AddRectFilled(p0, p1, IM_COL32(46, 113, 249, 255), rnd);
            for (int i = 1; i <= 3; ++i) {
                float off = (float)i * 1.0f * zm;
                float a = 1.0f - (float)(i - 1) / 3.0f;
                int alpha = (int)(25.0f * a * a);
                dl->AddLine(ImVec2(p0.x + rnd, p0.y + off),
                            ImVec2(p1.x - rnd, p0.y + off),
                            IM_COL32(0, 0, 0, alpha), 1.0f * zm);
            }
            dl->PopClipRect();
        }
    }

    // --- Tick marks ---
    {
        constexpr int num_ticks = 13;
        float tick_h = height * 0.30f;
        float tick_y0 = cy - tick_h * 0.5f;
        float tick_y1 = cy + tick_h * 0.5f;
        float margin = rnd + 16.0f * zm;
        float tick_start = p0.x + margin;
        float tick_end = p1.x - margin;
        float tick_spacing = (tick_end - tick_start) / (float)(num_ticks - 1);

        for (int i = 0; i < num_ticks; ++i) {
            float tx = tick_start + tick_spacing * (float)i;
            if (std::fabs(tx - thumb_x) < thumb_r * 0.8f) continue;
            bool on_blue = tx < thumb_x;
            ImU32 col = on_blue ? IM_COL32(255, 255, 255, 150)
                                : IM_COL32(190, 195, 205, 110);
            dl->AddLine(ImVec2(tx, tick_y0), ImVec2(tx, tick_y1),
                        col, 1.5f * zm);
        }
    }

    // --- "off" text ---
    {
        ImFont* font = ImGui::GetFont();
        float fsz = ImGui::GetFontSize() * 0.8f * zm;
        const char* label = "off";
        ImVec2 tsz = font->CalcTextSizeA(fsz, FLT_MAX, 0, label);
        float tx = p0.x + rnd * 0.55f;
        float ty = cy - tsz.y * 0.5f;
        bool off_on_blue = (tx + tsz.x) < thumb_x;
        ImU32 col = off_on_blue ? IM_COL32(255, 255, 255, 200)
                                : IM_COL32(170, 175, 185, 160);
        dl->AddText(font, fsz, ImVec2(tx, ty), col, label);
    }

    // --- Temperature text ---
    {
        int temp = (int)(16.0f + (*value) * 16.0f);
        char buf[16];
        std::snprintf(buf, sizeof(buf), "%d\xC2\xB0""C", temp);
        ImFont* font = ImGui::GetFont();
        float fsz = ImGui::GetFontSize() * 0.8f * zm;
        ImVec2 tsz = font->CalcTextSizeA(fsz, FLT_MAX, 0, buf);
        float tx = p1.x - rnd * 0.55f - tsz.x;
        float ty = cy - tsz.y * 0.5f;
        ImU32 col = IM_COL32(170, 175, 185, 170);
        dl->AddText(font, fsz, ImVec2(tx, ty), col, buf);
    }

    // --- Thumb shadow ---
    {
        constexpr int passes = 12;
        constexpr float spread = 5.0f;
        float sdx = 1.0f * zm, sdy = 2.0f * zm;
        for (int i = passes; i >= 0; --i) {
            float frac = (float)i / (float)passes;
            float ex = spread * frac * zm;
            int alpha = (int)(22.0f * (1.0f - frac) * (1.0f - frac));
            dl->AddCircleFilled(ImVec2(thumb_x + sdx, cy + sdy),
                                thumb_r + ex, IM_COL32(0, 0, 0, alpha));
        }
    }

    // --- Thumb body ---
    dl->AddCircleFilled(ImVec2(thumb_x, cy), thumb_r,
                        IM_COL32(255, 255, 255, 255));

    // --- Blue glow ring ---
    dl->AddCircle(ImVec2(thumb_x, cy), thumb_r + 1.0f * zm,
                  IM_COL32(46, 113, 249, 160), 0, 2.5f * zm);
    dl->AddCircle(ImVec2(thumb_x, cy), thumb_r + 3.0f * zm,
                  IM_COL32(46, 113, 249, 40), 0, 2.0f * zm);

    // --- Grip lines ---
    {
        float gh = thumb_r * 0.55f;
        float gy0 = cy - gh * 0.5f;
        float gy1 = cy + gh * 0.5f;
        float gs = 3.5f * zm;
        ImU32 gc = IM_COL32(185, 190, 200, 170);
        for (int i = -1; i <= 1; ++i) {
            float gx = thumb_x + (float)i * gs;
            dl->AddLine(ImVec2(gx, gy0), ImVec2(gx, gy1), gc, 1.5f * zm);
        }
    }

    // --- Interaction (drag) ---
    {
        static bool dragging = false;
        ImGuiIO& io = ImGui::GetIO();
        ImVec2 mp = io.MousePos;
        float dx = mp.x - thumb_x, dy = mp.y - cy;
        bool over_thumb = (dx * dx + dy * dy) <= (thumb_r + 4.0f * zm) * (thumb_r + 4.0f * zm);
        bool over_track = mp.x >= p0.x && mp.x <= p1.x &&
                          mp.y >= p0.y - 4.0f * zm && mp.y <= p1.y + 4.0f * zm;

        if (!dragging && ImGui::IsMouseClicked(0) && (over_thumb || over_track))
            dragging = true;

        if (dragging) {
            if (ImGui::IsMouseDown(0)) {
                float nx = std::clamp(mp.x, p0.x + thumb_margin, p0.x + thumb_margin + thumb_range);
                *value = (nx - (p0.x + thumb_margin)) / thumb_range;
            } else {
                dragging = false;
            }
        }
    }
}

// -------------------------------------------------------------------
// Skeuomorphic Button
// -------------------------------------------------------------------

static void DrawSkeuoButtonImpl(ImDrawList* dl, ImVec2 pos,
                                 float w, float h, const char* label,
                                 bool* pressed, float zm) {
    ImVec2 p0 = pos, p1(pos.x + w, pos.y + h);
    float rnd = 8.0f * zm;
    bool active = *pressed;

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 mp = io.MousePos;
    bool hovered = mp.x >= p0.x && mp.x <= p1.x &&
                   mp.y >= p0.y && mp.y <= p1.y;

    if (hovered && ImGui::IsMouseClicked(0))
        *pressed = !(*pressed);

    active = *pressed;

    if (!active) {
        // Raised outer shadows
        {
            constexpr int passes = 8;
            constexpr float spread = 5.0f;
            float sdx = 2.0f * zm, sdy = 2.0f * zm;
            for (int i = passes; i >= 0; --i) {
                float f = (float)i / (float)passes;
                float ex = spread * f * zm;
                int a = (int)(12.0f * (1.0f - f) * (1.0f - f));
                dl->AddRectFilled(ImVec2(p0.x + sdx - ex, p0.y + sdy - ex),
                                  ImVec2(p1.x + sdx + ex, p1.y + sdy + ex),
                                  IM_COL32(0, 0, 0, a), rnd + ex);
            }
        }
        {
            constexpr int passes = 8;
            constexpr float spread = 5.0f;
            float lx = -1.5f * zm, ly = -1.5f * zm;
            for (int i = passes; i >= 0; --i) {
                float f = (float)i / (float)passes;
                float ex = spread * f * zm;
                int a = (int)(28.0f * (1.0f - f) * (1.0f - f));
                dl->AddRectFilled(ImVec2(p0.x + lx - ex, p0.y + ly - ex),
                                  ImVec2(p1.x + lx + ex, p1.y + ly + ex),
                                  IM_COL32(255, 255, 255, a), rnd + ex);
            }
        }
        ImU32 fill = hovered ? IM_COL32(245, 247, 250, 255)
                             : IM_COL32(239, 241, 243, 255);
        dl->AddRectFilled(p0, p1, fill, rnd);

        // Inner highlight
        dl->AddLine(ImVec2(p0.x + rnd, p0.y + 1.0f * zm),
                    ImVec2(p1.x - rnd, p0.y + 1.0f * zm),
                    IM_COL32(255, 255, 255, 80), 1.0f * zm);
        dl->AddLine(ImVec2(p0.x + rnd, p1.y - 1.0f * zm),
                    ImVec2(p1.x - rnd, p1.y - 1.0f * zm),
                    IM_COL32(0, 0, 0, 15), 1.0f * zm);

        ImFont* font = ImGui::GetFont();
        float fsz = ImGui::GetFontSize() * 0.9f * zm;
        ImVec2 tsz = font->CalcTextSizeA(fsz, FLT_MAX, 0, label);
        float tx = p0.x + (w - tsz.x) * 0.5f;
        float ty = p0.y + (h - tsz.y) * 0.5f;
        dl->AddText(font, fsz, ImVec2(tx, ty),
                    IM_COL32(80, 85, 95, 255), label);
    } else {
        // Pressed: inset look
        {
            constexpr int passes = 6;
            constexpr float spread = 3.0f;
            float sdx = 1.0f * zm, sdy = 1.0f * zm;
            for (int i = passes; i >= 0; --i) {
                float f = (float)i / (float)passes;
                float ex = spread * f * zm;
                int a = (int)(10.0f * (1.0f - f) * (1.0f - f));
                dl->AddRectFilled(ImVec2(p0.x + sdx - ex, p0.y + sdy - ex),
                                  ImVec2(p1.x + sdx + ex, p1.y + sdy + ex),
                                  IM_COL32(0, 0, 0, a), rnd + ex);
            }
        }
        dl->AddRectFilled(p0, p1, IM_COL32(46, 113, 249, 255), rnd);

        // Inner shadow on blue
        for (int i = 1; i <= 3; ++i) {
            float off = (float)i * 1.0f * zm;
            float a = 1.0f - (float)(i - 1) / 3.0f;
            dl->AddLine(ImVec2(p0.x + rnd, p0.y + off),
                        ImVec2(p1.x - rnd, p0.y + off),
                        IM_COL32(0, 0, 0, (int)(20.0f * a * a)), 1.0f * zm);
        }
        dl->AddLine(ImVec2(p0.x + rnd, p1.y - 1.0f * zm),
                    ImVec2(p1.x - rnd, p1.y - 1.0f * zm),
                    IM_COL32(255, 255, 255, 30), 1.0f * zm);

        ImFont* font = ImGui::GetFont();
        float fsz = ImGui::GetFontSize() * 0.9f * zm;
        ImVec2 tsz = font->CalcTextSizeA(fsz, FLT_MAX, 0, label);
        float tx = p0.x + (w - tsz.x) * 0.5f;
        float ty = p0.y + (h - tsz.y) * 0.5f + 1.0f * zm;
        dl->AddText(font, fsz, ImVec2(tx, ty),
                    IM_COL32(255, 255, 255, 255), label);
    }
}

// -------------------------------------------------------------------
// Skeuomorphic Toggle Switch
// -------------------------------------------------------------------

static void DrawSkeuoToggleImpl(ImDrawList* dl, ImVec2 pos,
                                 float w, float h, bool* value, float zm) {
    ImVec2 p0 = pos, p1(pos.x + w, pos.y + h);
    float rnd = h * 0.5f;
    float cy = p0.y + h * 0.5f;

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 mp = io.MousePos;
    bool hovered = mp.x >= p0.x && mp.x <= p1.x &&
                   mp.y >= p0.y && mp.y <= p1.y;
    if (hovered && ImGui::IsMouseClicked(0))
        *value = !(*value);

    float knob_r = h * 0.40f;
    float knob_margin = h * 0.5f;
    float knob_x = *value ? (p1.x - knob_margin) : (p0.x + knob_margin);

    if (!(*value)) {
        // Off: inset gray track
        {
            constexpr int passes = 6;
            constexpr float spread = 3.0f;
            for (int i = passes; i >= 0; --i) {
                float f = (float)i / (float)passes;
                float ex = spread * f * zm;
                int a = (int)(12.0f * (1.0f - f) * (1.0f - f));
                dl->AddRectFilled(ImVec2(p0.x + 1.0f * zm - ex, p0.y + 1.0f * zm - ex),
                                  ImVec2(p1.x + 1.0f * zm + ex, p1.y + 1.0f * zm + ex),
                                  IM_COL32(0, 0, 0, a), rnd + ex);
            }
        }
        dl->AddRectFilled(p0, p1, IM_COL32(210, 212, 218, 255), rnd);
        dl->AddLine(ImVec2(p0.x + rnd, p0.y + 1.0f * zm),
                    ImVec2(p1.x - rnd, p0.y + 1.0f * zm),
                    IM_COL32(0, 0, 0, 18), 1.0f * zm);
        dl->AddLine(ImVec2(p0.x + rnd, p1.y - 1.0f * zm),
                    ImVec2(p1.x - rnd, p1.y - 1.0f * zm),
                    IM_COL32(255, 255, 255, 40), 1.0f * zm);
    } else {
        // On: blue track
        {
            constexpr int passes = 6;
            constexpr float spread = 3.0f;
            for (int i = passes; i >= 0; --i) {
                float f = (float)i / (float)passes;
                float ex = spread * f * zm;
                int a = (int)(10.0f * (1.0f - f) * (1.0f - f));
                dl->AddRectFilled(ImVec2(p0.x + 1.0f * zm - ex, p0.y + 1.0f * zm - ex),
                                  ImVec2(p1.x + 1.0f * zm + ex, p1.y + 1.0f * zm + ex),
                                  IM_COL32(0, 0, 0, a), rnd + ex);
            }
        }
        dl->AddRectFilled(p0, p1, IM_COL32(46, 113, 249, 255), rnd);
        for (int i = 1; i <= 2; ++i) {
            float off = (float)i * 1.0f * zm;
            float a = 1.0f - (float)(i - 1) / 2.0f;
            dl->AddLine(ImVec2(p0.x + rnd, p0.y + off),
                        ImVec2(p1.x - rnd, p0.y + off),
                        IM_COL32(0, 0, 0, (int)(20.0f * a * a)), 1.0f * zm);
        }
    }

    // Knob shadow
    {
        constexpr int passes = 8;
        constexpr float spread = 4.0f;
        for (int i = passes; i >= 0; --i) {
            float f = (float)i / (float)passes;
            float ex = spread * f * zm;
            int a = (int)(20.0f * (1.0f - f) * (1.0f - f));
            dl->AddCircleFilled(ImVec2(knob_x + 1.0f * zm, cy + 1.5f * zm),
                                knob_r + ex, IM_COL32(0, 0, 0, a));
        }
    }
    dl->AddCircleFilled(ImVec2(knob_x, cy), knob_r, IM_COL32(255, 255, 255, 255));
    dl->AddCircle(ImVec2(knob_x, cy), knob_r, IM_COL32(0, 0, 0, 15), 0, 1.0f * zm);
}

// -------------------------------------------------------------------
// Skeuomorphic Checkbox
// -------------------------------------------------------------------

static void DrawSkeuoCheckboxImpl(ImDrawList* dl, ImVec2 pos,
                                   float sz, bool* checked,
                                   const char* label, float zm) {
    ImVec2 p0 = pos, p1(pos.x + sz, pos.y + sz);
    float rnd = 4.0f * zm;

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 mp = io.MousePos;
    bool hovered = mp.x >= p0.x && mp.x <= p1.x &&
                   mp.y >= p0.y && mp.y <= p1.y;
    if (hovered && ImGui::IsMouseClicked(0))
        *checked = !(*checked);

    if (!(*checked)) {
        // Inset shadow (sunken)
        {
            constexpr int passes = 6;
            constexpr float spread = 3.0f;
            float ox = 1.0f * zm, oy = 1.0f * zm;
            for (int i = passes; i >= 0; --i) {
                float f = (float)i / (float)passes;
                float ex = spread * f * zm;
                int a = (int)(15.0f * (1.0f - f) * (1.0f - f));
                dl->AddRectFilled(ImVec2(p0.x + ox - ex, p0.y + oy - ex),
                                  ImVec2(p1.x + ox + ex, p1.y + oy + ex),
                                  IM_COL32(0, 0, 0, a), rnd + ex * 0.5f);
            }
        }
        dl->AddRectFilled(p0, p1, IM_COL32(220, 223, 228, 255), rnd);
        dl->AddLine(ImVec2(p0.x + rnd * 0.5f, p0.y + 1.0f * zm),
                    ImVec2(p1.x - rnd * 0.5f, p0.y + 1.0f * zm),
                    IM_COL32(0, 0, 0, 20), 1.0f * zm);
        dl->AddLine(ImVec2(p0.x + rnd * 0.5f, p1.y - 1.0f * zm),
                    ImVec2(p1.x - rnd * 0.5f, p1.y - 1.0f * zm),
                    IM_COL32(255, 255, 255, 50), 1.0f * zm);
    } else {
        // Checked: raised blue
        {
            constexpr int passes = 6;
            constexpr float spread = 4.0f;
            float sdx = 1.5f * zm, sdy = 1.5f * zm;
            for (int i = passes; i >= 0; --i) {
                float f = (float)i / (float)passes;
                float ex = spread * f * zm;
                int a = (int)(12.0f * (1.0f - f) * (1.0f - f));
                dl->AddRectFilled(ImVec2(p0.x + sdx - ex, p0.y + sdy - ex),
                                  ImVec2(p1.x + sdx + ex, p1.y + sdy + ex),
                                  IM_COL32(0, 0, 0, a), rnd + ex * 0.5f);
            }
        }
        {
            constexpr int passes = 6;
            constexpr float spread = 3.0f;
            float lx = -1.0f * zm, ly = -1.0f * zm;
            for (int i = passes; i >= 0; --i) {
                float f = (float)i / (float)passes;
                float ex = spread * f * zm;
                int a = (int)(20.0f * (1.0f - f) * (1.0f - f));
                dl->AddRectFilled(ImVec2(p0.x + lx - ex, p0.y + ly - ex),
                                  ImVec2(p1.x + lx + ex, p1.y + ly + ex),
                                  IM_COL32(255, 255, 255, a), rnd + ex * 0.5f);
            }
        }
        dl->AddRectFilled(p0, p1, IM_COL32(46, 113, 249, 255), rnd);

        // White checkmark
        float cx = p0.x + sz * 0.5f;
        float cy = p0.y + sz * 0.5f;
        float s = sz * 0.28f;
        float lw = 2.5f * zm;
        dl->AddLine(ImVec2(cx - s, cy), ImVec2(cx - s * 0.2f, cy + s * 0.8f),
                    IM_COL32(255, 255, 255, 255), lw);
        dl->AddLine(ImVec2(cx - s * 0.2f, cy + s * 0.8f), ImVec2(cx + s, cy - s * 0.6f),
                    IM_COL32(255, 255, 255, 255), lw);
    }

    if (label && *label) {
        ImFont* font = ImGui::GetFont();
        float fsz = ImGui::GetFontSize() * 0.85f * zm;
        float tx = p1.x + 6.0f * zm;
        float ty = p0.y + (sz - fsz) * 0.5f;
        dl->AddText(font, fsz, ImVec2(tx, ty),
                    IM_COL32(80, 85, 95, 255), label);
    }
}

// -------------------------------------------------------------------
// Skeuomorphic Radio Button
// -------------------------------------------------------------------

static void DrawSkeuoRadioImpl(ImDrawList* dl, ImVec2 pos,
                                float sz, bool selected,
                                const char* label, float zm) {
    float r = sz * 0.5f;
    float cx = pos.x + r, cy = pos.y + r;

    if (!selected) {
        // Inset circle
        {
            constexpr int passes = 6;
            constexpr float spread = 3.0f;
            for (int i = passes; i >= 0; --i) {
                float f = (float)i / (float)passes;
                float ex = spread * f * zm;
                int a = (int)(14.0f * (1.0f - f) * (1.0f - f));
                dl->AddCircleFilled(ImVec2(cx + 1.0f * zm, cy + 1.0f * zm),
                                    r + ex, IM_COL32(0, 0, 0, a));
            }
        }
        dl->AddCircleFilled(ImVec2(cx, cy), r, IM_COL32(220, 223, 228, 255));
    } else {
        // Raised blue circle
        {
            constexpr int passes = 6;
            constexpr float spread = 4.0f;
            for (int i = passes; i >= 0; --i) {
                float f = (float)i / (float)passes;
                float ex = spread * f * zm;
                int a = (int)(12.0f * (1.0f - f) * (1.0f - f));
                dl->AddCircleFilled(ImVec2(cx + 1.5f * zm, cy + 1.5f * zm),
                                    r + ex, IM_COL32(0, 0, 0, a));
            }
        }
        {
            constexpr int passes = 4;
            constexpr float spread = 3.0f;
            for (int i = passes; i >= 0; --i) {
                float f = (float)i / (float)passes;
                float ex = spread * f * zm;
                int a = (int)(18.0f * (1.0f - f) * (1.0f - f));
                dl->AddCircleFilled(ImVec2(cx - 1.0f * zm, cy - 1.0f * zm),
                                    r + ex, IM_COL32(255, 255, 255, a));
            }
        }
        dl->AddCircleFilled(ImVec2(cx, cy), r, IM_COL32(46, 113, 249, 255));
        dl->AddCircleFilled(ImVec2(cx, cy), r * 0.35f, IM_COL32(255, 255, 255, 255));
    }

    if (label && *label) {
        ImFont* font = ImGui::GetFont();
        float fsz = ImGui::GetFontSize() * 0.85f * zm;
        float tx = pos.x + sz + 6.0f * zm;
        float ty = pos.y + (sz - fsz) * 0.5f;
        dl->AddText(font, fsz, ImVec2(tx, ty),
                    IM_COL32(80, 85, 95, 255), label);
    }
}

// -------------------------------------------------------------------
// Skeuomorphic Input (display-only)
// -------------------------------------------------------------------

static void DrawSkeuoInputImpl(ImDrawList* dl, ImVec2 pos,
                                float w, float h,
                                const char* text, float zm) {
    ImVec2 p0 = pos, p1(pos.x + w, pos.y + h);
    float rnd = 6.0f * zm;

    // Inset shadow (dark top-left, light bottom-right)
    {
        constexpr int passes = 6;
        constexpr float spread = 3.0f;
        float ox = 1.0f * zm, oy = 1.0f * zm;
        for (int i = passes; i >= 0; --i) {
            float f = (float)i / (float)passes;
            float ex = spread * f * zm;
            int a = (int)(14.0f * (1.0f - f) * (1.0f - f));
            dl->AddRectFilled(ImVec2(p0.x + ox - ex, p0.y + oy - ex),
                              ImVec2(p1.x + ox + ex, p1.y + oy + ex),
                              IM_COL32(0, 0, 0, a), rnd + ex * 0.5f);
        }
    }

    dl->AddRectFilled(p0, p1, IM_COL32(228, 230, 234, 255), rnd);

    // Inner edge lines
    dl->AddLine(ImVec2(p0.x + rnd, p0.y + 1.0f * zm),
                ImVec2(p1.x - rnd, p0.y + 1.0f * zm),
                IM_COL32(0, 0, 0, 18), 1.0f * zm);
    dl->AddLine(ImVec2(p0.x + rnd, p1.y - 1.0f * zm),
                ImVec2(p1.x - rnd, p1.y - 1.0f * zm),
                IM_COL32(255, 255, 255, 45), 1.0f * zm);

    // Text (clipped)
    if (text && *text) {
        float pad = 8.0f * zm;
        ImFont* font = ImGui::GetFont();
        float fsz = ImGui::GetFontSize() * 0.85f * zm;
        dl->PushClipRect(ImVec2(p0.x + pad, p0.y), ImVec2(p1.x - pad, p1.y), true);
        float ty = p0.y + (h - fsz) * 0.5f;
        dl->AddText(font, fsz, ImVec2(p0.x + pad, ty),
                    IM_COL32(60, 65, 75, 255), text);
        dl->PopClipRect();
    }
}

// -------------------------------------------------------------------
// Skeuomorphic Progress Bar
// -------------------------------------------------------------------

static void DrawSkeuoProgressBarImpl(ImDrawList* dl, ImVec2 pos,
                                      float w, float h,
                                      float value, float zm) {
    ImVec2 p0 = pos, p1(pos.x + w, pos.y + h);
    float rnd = h * 0.5f;

    // Inset track shadow
    {
        constexpr int passes = 8;
        constexpr float spread = 4.0f;
        float ox = 1.0f * zm, oy = 1.0f * zm;
        for (int i = passes; i >= 0; --i) {
            float f = (float)i / (float)passes;
            float ex = spread * f * zm;
            int a = (int)(14.0f * (1.0f - f) * (1.0f - f));
            dl->AddRectFilled(ImVec2(p0.x + ox - ex, p0.y + oy - ex),
                              ImVec2(p1.x + ox + ex, p1.y + oy + ex),
                              IM_COL32(0, 0, 0, a), rnd + ex);
        }
    }

    // Gray background track
    dl->AddRectFilled(p0, p1, IM_COL32(218, 220, 226, 255), rnd);
    dl->AddLine(ImVec2(p0.x + rnd, p0.y + 1.0f * zm),
                ImVec2(p1.x - rnd, p0.y + 1.0f * zm),
                IM_COL32(0, 0, 0, 15), 1.0f * zm);
    dl->AddLine(ImVec2(p0.x + rnd, p1.y - 1.0f * zm),
                ImVec2(p1.x - rnd, p1.y - 1.0f * zm),
                IM_COL32(255, 255, 255, 35), 1.0f * zm);

    // Blue fill
    float fill_w = w * std::clamp(value, 0.0f, 1.0f);
    if (fill_w > rnd * 2.0f) {
        dl->PushClipRect(p0, ImVec2(p0.x + fill_w, p1.y), true);
        dl->AddRectFilled(p0, p1, IM_COL32(46, 113, 249, 255), rnd);
        // Highlight band near top
        dl->AddLine(ImVec2(p0.x + rnd, p0.y + 2.0f * zm),
                    ImVec2(p0.x + fill_w - rnd * 0.5f, p0.y + 2.0f * zm),
                    IM_COL32(100, 160, 255, 80), 1.5f * zm);
        dl->PopClipRect();
    }

    // Percentage text
    {
        char buf[16];
        std::snprintf(buf, sizeof(buf), "%d%%", (int)(value * 100.0f));
        ImFont* font = ImGui::GetFont();
        float fsz = ImGui::GetFontSize() * 0.75f * zm;
        ImVec2 tsz = font->CalcTextSizeA(fsz, FLT_MAX, 0, buf);
        float tx = p0.x + (w - tsz.x) * 0.5f;
        float ty = p0.y + (h - tsz.y) * 0.5f;
        bool over_fill = (tx + tsz.x * 0.5f) < (p0.x + fill_w);
        ImU32 col = over_fill ? IM_COL32(255, 255, 255, 220)
                              : IM_COL32(120, 125, 135, 180);
        dl->AddText(font, fsz, ImVec2(tx, ty), col, buf);
    }
}

static void DrawSkeuomorphTab(float dpi_scale) {
    ImVec2 cursor = ImGui::GetCursorScreenPos();
    ImVec2 avail = ImGui::GetContentRegionAvail();

    ImVec2 p0 = cursor;
    ImVec2 p1(cursor.x + avail.x, cursor.y + avail.y);

    ImDrawList* bg_dl = ImGui::GetWindowDrawList();
    ImU32 fill   = IM_COL32(237, 241, 244, 255);
    ImU32 border = IM_COL32(220, 222, 228, 180);
    float bg_rnd = 8.0f * dpi_scale;
    bg_dl->AddRectFilled(p0, p1, fill, bg_rnd);
    bg_dl->AddRect(p0, p1, border, bg_rnd, 0, 1.0f);

    ImGui::BeginChild("##skeuo_scroll", avail, false);

    ImDrawList* dl = ImGui::GetWindowDrawList();
    float pad = 16.0f;
    float cw = avail.x - pad * 2.0f;
    float cx = pad;
    float gap = 18.0f;

    #define SKEUO_SEL(t) { ImVec2 _mp = ImGui::GetIO().MousePos; \
        if (_mp.x >= _x0 && _mp.x <= _x1 && _mp.y >= _y0 && _mp.y <= _y1 \
            && ImGui::IsMouseClicked(0)) g_selected_widget = WidgetType::t; }

    ImGui::Spacing();
    ImGui::Indent(pad);
    ImGui::TextDisabled(ICON_FA_CUBE " \xd0\x9a\xd0\xb0\xd1\x80\xd1\x82\xd0\xbe\xd1\x87\xd0\xba\xd0\xb0 \xd0\xb8 \xd1\x81\xd0\xbb\xd0\xb0\xd0\xb9\xd0\xb4\xd0\xb5\xd1\x80");
    ImGui::Unindent(pad);
    DrUI::GradientSeparator();
    ImGui::Spacing();

    // --- Card ---
    {
        float card_sz = 220.0f;
        ImVec2 cur = ImGui::GetCursorScreenPos();
        float card_x = cur.x + (cw + pad * 2.0f - card_sz) * 0.5f;
        float card_y = cur.y;
        float _x0 = card_x, _y0 = card_y, _x1 = card_x + card_sz, _y1 = card_y + card_sz;

        if (ImGui::GetIO().MousePos.x >= _x0 && ImGui::GetIO().MousePos.x <= _x1 &&
            ImGui::GetIO().MousePos.y >= _y0 && ImGui::GetIO().MousePos.y <= _y1 &&
            ImGui::IsMouseClicked(0)) {
            g_card_active = !g_card_active;
            g_selected_widget = WidgetType::SkeuomorphCard;
        }

        DrawSkeuomorphCardImpl(dl, ImVec2(card_x, card_y), card_sz, g_card_active, 1.0f);
        ImGui::Dummy(ImVec2(cw, card_sz + gap));
    }

    // --- Slider ---
    {
        float slider_w = std::min(cw, 380.0f);
        float slider_h = 50.0f;
        ImVec2 cur = ImGui::GetCursorScreenPos();
        float sl_x = cur.x + (cw + pad * 2.0f - slider_w) * 0.5f;
        float sl_y = cur.y;
        float _x0 = sl_x, _y0 = sl_y - slider_h * 0.1f;
        float _x1 = sl_x + slider_w, _y1 = sl_y + slider_h * 1.2f;
        SKEUO_SEL(SkeuomorphSlider);
        DrawSkeuomorphSliderImpl(dl, ImVec2(sl_x, sl_y), slider_w, slider_h,
                                 &g_skeuo_slider_val, 1.0f);
        ImGui::Dummy(ImVec2(cw, slider_h + gap + 8.0f));
    }

    ImGui::Spacing();
    ImGui::Indent(pad);
    ImGui::TextDisabled(ICON_FA_BOLT " \xd0\x9a\xd0\xbd\xd0\xbe\xd0\xbf\xd0\xba\xd0\xb8 \xd0\xb8 \xd0\xbf\xd0\xb5\xd1\x80\xd0\xb5\xd0\xba\xd0\xbb\xd1\x8e\xd1\x87\xd0\xb0\xd1\x82\xd0\xb5\xd0\xbb\xd0\xb8");
    ImGui::Unindent(pad);
    DrUI::GradientSeparator();
    ImGui::Spacing();

    // --- Buttons ---
    {
        static bool btn1 = false, btn2 = true;
        float bw = 120.0f, bh = 36.0f;
        ImVec2 cur = ImGui::GetCursorScreenPos();
        float row_w = bw * 2.0f + 12.0f;
        float bx = cur.x + (cw + pad * 2.0f - row_w) * 0.5f;
        float _x0 = bx, _y0 = cur.y, _x1 = bx + row_w, _y1 = cur.y + bh;
        SKEUO_SEL(SkeuoButton);
        DrawSkeuoButtonImpl(dl, ImVec2(bx, cur.y), bw, bh, "Normal", &btn1, 1.0f);
        DrawSkeuoButtonImpl(dl, ImVec2(bx + bw + 12.0f, cur.y), bw, bh, "Active", &btn2, 1.0f);
        ImGui::Dummy(ImVec2(cw, bh + gap));
    }

    // --- Toggles ---
    {
        static bool tog1 = true, tog2 = false;
        float tw = 52.0f, th = 28.0f;
        ImVec2 cur = ImGui::GetCursorScreenPos();
        float row_w = tw * 2.0f + 16.0f;
        float bx = cur.x + (cw + pad * 2.0f - row_w) * 0.5f;
        float _x0 = bx, _y0 = cur.y, _x1 = bx + row_w, _y1 = cur.y + th;
        SKEUO_SEL(SkeuoToggle);
        DrawSkeuoToggleImpl(dl, ImVec2(bx, cur.y), tw, th, &tog1, 1.0f);
        DrawSkeuoToggleImpl(dl, ImVec2(bx + tw + 16.0f, cur.y), tw, th, &tog2, 1.0f);
        ImGui::Dummy(ImVec2(cw, th + gap));
    }

    ImGui::Spacing();
    ImGui::Indent(pad);
    ImGui::TextDisabled(ICON_FA_CHECK " \xd0\x92\xd1\x8b\xd0\xb1\xd0\xbe\xd1\x80 \xd0\xb8 \xd0\xb2\xd0\xb2\xd0\xbe\xd0\xb4");
    ImGui::Unindent(pad);
    DrUI::GradientSeparator();
    ImGui::Spacing();

    // --- Checkboxes ---
    {
        static bool chk1 = true, chk2 = false, chk3 = true;
        float sz = 20.0f;
        ImVec2 cur = ImGui::GetCursorScreenPos();
        float bx = cur.x + pad;
        float _x0 = bx, _y0 = cur.y, _x1 = bx + cw, _y1 = cur.y + sz * 3.0f + 8.0f * 2.0f;
        SKEUO_SEL(SkeuoCheckbox);
        DrawSkeuoCheckboxImpl(dl, ImVec2(bx, cur.y), sz, &chk1, "Option A", 1.0f);
        DrawSkeuoCheckboxImpl(dl, ImVec2(bx, cur.y + sz + 8.0f), sz, &chk2, "Option B", 1.0f);
        DrawSkeuoCheckboxImpl(dl, ImVec2(bx, cur.y + (sz + 8.0f) * 2.0f), sz, &chk3, "Option C", 1.0f);
        ImGui::Dummy(ImVec2(cw, sz * 3.0f + 8.0f * 2.0f + gap));
    }

    // --- Radio buttons ---
    {
        static int radio_sel = 0;
        float sz = 20.0f;
        ImVec2 cur = ImGui::GetCursorScreenPos();
        float bx = cur.x + pad;
        float _x0 = bx, _y0 = cur.y, _x1 = bx + cw, _y1 = cur.y + sz * 3.0f + 8.0f * 2.0f;
        SKEUO_SEL(SkeuoRadio);

        for (int i = 0; i < 3; ++i) {
            const char* labels[] = {"Mode 1", "Mode 2", "Mode 3"};
            bool sel = (radio_sel == i);
            float ry = cur.y + (float)i * (sz + 8.0f);
            ImVec2 rmp = ImGui::GetIO().MousePos;
            if (rmp.x >= bx && rmp.x <= bx + 100.0f &&
                rmp.y >= ry && rmp.y <= ry + sz && ImGui::IsMouseClicked(0))
                radio_sel = i;
            DrawSkeuoRadioImpl(dl, ImVec2(bx, ry), sz, sel, labels[i], 1.0f);
        }
        ImGui::Dummy(ImVec2(cw, sz * 3.0f + 8.0f * 2.0f + gap));
    }

    ImGui::Spacing();
    ImGui::Indent(pad);
    ImGui::TextDisabled(ICON_FA_EDIT " \xd0\x9f\xd0\xbe\xd0\xbb\xd0\xb5 \xd0\xb2\xd0\xb2\xd0\xbe\xd0\xb4\xd0\xb0 \xd0\xb8 \xd0\xbf\xd1\x80\xd0\xbe\xd0\xb3\xd1\x80\xd0\xb5\xd1\x81\xd1\x81");
    ImGui::Unindent(pad);
    DrUI::GradientSeparator();
    ImGui::Spacing();

    // --- Input ---
    {
        float iw = std::min(cw - 8.0f, 300.0f), ih = 32.0f;
        ImVec2 cur = ImGui::GetCursorScreenPos();
        float bx = cur.x + (cw + pad * 2.0f - iw) * 0.5f;
        float _x0 = bx, _y0 = cur.y, _x1 = bx + iw, _y1 = cur.y + ih;
        SKEUO_SEL(SkeuoInput);
        DrawSkeuoInputImpl(dl, ImVec2(bx, cur.y), iw, ih,
                           "Hello, Skeuomorph!", 1.0f);
        ImGui::Dummy(ImVec2(cw, ih + gap));
    }

    // --- Progress bar ---
    {
        float pw = std::min(cw - 8.0f, 300.0f), ph = 22.0f;
        ImVec2 cur = ImGui::GetCursorScreenPos();
        float bx = cur.x + (cw + pad * 2.0f - pw) * 0.5f;
        float _x0 = bx, _y0 = cur.y, _x1 = bx + pw, _y1 = cur.y + ph;
        SKEUO_SEL(SkeuoProgress);
        DrawSkeuoProgressBarImpl(dl, ImVec2(bx, cur.y), pw, ph, 0.65f, 1.0f);
        ImGui::Dummy(ImVec2(cw, ph + gap));
    }

    #undef SKEUO_SEL

    ImGui::EndChild();
}

static void DrawLeftPanel(const PanelLayout& zone, float dpi_scale,
                          PanelVisibility& panels) {
    ImGui::SetNextWindowPos(zone.pos);
    ImGui::SetNextWindowSize(zone.size);
    ImGui::Begin("##LeftPanel", nullptr, kPanelFlags);

    if (ImGui::BeginTabBar("##LeftTabs", ImGuiTabBarFlags_DrawSelectedOverline)) {
        if (panels.tab_widgets && ImGui::BeginTabItem(ICON_FA_COG " \xd0\x92\xd0\xb8\xd0\xb4\xd0\xb6\xd0\xb5\xd1\x82\xd1\x8b", &panels.tab_widgets)) {
            DrawWidgetsTab(dpi_scale);
            ImGui::EndTabItem();
        }
        if (panels.tab_custom && ImGui::BeginTabItem(ICON_FA_BOLT " Custom", &panels.tab_custom)) {
            DrawCustomTab(dpi_scale);
            ImGui::EndTabItem();
        }
        if (panels.tab_skeuomorph && ImGui::BeginTabItem(ICON_FA_CUBE " Skeuomorph", &panels.tab_skeuomorph)) {
            DrawSkeuomorphTab(dpi_scale);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}

// ===================================================================
// Widget preview (drawn on the canvas)
// ===================================================================

static constexpr float kPrevWorldPad = 16.0f;

static ImVec2 GetPreviewWorldSize(WidgetType type, float dpi_scale) {
    float cw, ch;
    switch (type) {
    case WidgetType::Button:              cw = 380; ch = 80;  break;
    case WidgetType::IconButton:          cw = 250; ch = 50;  break;
    case WidgetType::IconToggle:          cw = 250; ch = 50;  break;
    case WidgetType::ColoredButton:       cw = 380; ch = 50;  break;
    case WidgetType::ToggleSwitch:        cw = 300; ch = 90;  break;
    case WidgetType::InputText:           cw = 350; ch = 50;  break;
    case WidgetType::InputTextMultiline:  cw = 350; ch = 130; break;
    case WidgetType::InputInt:            cw = 300; ch = 50;  break;
    case WidgetType::InputFloat:          cw = 300; ch = 50;  break;
    case WidgetType::TagInput:            cw = 350; ch = 50;  break;
    case WidgetType::SliderFloat:         cw = 350; ch = 50;  break;
    case WidgetType::SliderInt:           cw = 350; ch = 50;  break;
    case WidgetType::ProgressBar:         cw = 350; ch = 40;  break;
    case WidgetType::Spinner:             cw = 300; ch = 40;  break;
    case WidgetType::Checkbox:            cw = 300; ch = 100; break;
    case WidgetType::RadioButton:         cw = 400; ch = 50;  break;
    case WidgetType::Combo:               cw = 350; ch = 50;  break;
    case WidgetType::ColorEdit4:          cw = 400; ch = 50;  break;
    case WidgetType::ShimmerText:         cw = 380; ch = 100; break;
    case WidgetType::GradientBorder:      cw = 380; ch = 100; break;
    case WidgetType::GlowGradientBorder:  cw = 380; ch = 130; break;
    case WidgetType::SkeuomorphCard:      cw = 280; ch = 280; break;
    case WidgetType::SkeuomorphSlider:   cw = 460; ch = 100; break;
    case WidgetType::SkeuoButton:        cw = 200; ch = 60;  break;
    case WidgetType::SkeuoToggle:        cw = 120; ch = 60;  break;
    case WidgetType::SkeuoCheckbox:      cw = 250; ch = 40;  break;
    case WidgetType::SkeuoRadio:         cw = 250; ch = 40;  break;
    case WidgetType::SkeuoInput:         cw = 350; ch = 60;  break;
    case WidgetType::SkeuoProgress:      cw = 350; ch = 50;  break;
    default:                              cw = 380; ch = 200; break;
    }
    float pad2 = kPrevWorldPad * 2.0f;
    return ImVec2(cw * dpi_scale + pad2, ch * dpi_scale + pad2);
}

static void DrawWidgetPreview(WidgetType type, float dpi_scale) {
    ImGui::PushID("cvs_preview");
    switch (type) {
    case WidgetType::None: break;

    case WidgetType::Button: {
        if (DrUI::Button(ICON_FA_PLUS " \xd0\x94\xd0\xbe\xd0\xb1\xd0\xb0\xd0\xb2\xd0\xb8\xd1\x82\xd1\x8c")) {}
        ImGui::SameLine();
        if (DrUI::Button(ICON_FA_TRASH " \xd0\xa3\xd0\xb4\xd0\xb0\xd0\xbb\xd0\xb8\xd1\x82\xd1\x8c")) {}
        ImGui::SameLine();
        if (DrUI::Button(ICON_FA_SAVE " \xd0\xa1\xd0\xbe\xd1\x85\xd1\x80\xd0\xb0\xd0\xbd\xd0\xb8\xd1\x82\xd1\x8c")) {}
        ImGui::Spacing();
        if (DrUI::Button(ICON_FA_PLAY, "\xd0\x97\xd0\xb0\xd0\xbf\xd1\x83\xd1\x81\xd0\xba")) {}
        ImGui::SameLine();
        if (DrUI::Button(ICON_FA_STOP, "\xd0\xa1\xd1\x82\xd0\xbe\xd0\xbf")) {}
        break;
    }
    case WidgetType::IconButton: {
        if (DrUI::IconButton(ICON_FA_UNDO)) {}
        ImGui::SameLine();
        if (DrUI::IconButton(ICON_FA_REDO)) {}
        ImGui::SameLine();
        if (DrUI::IconButton(ICON_FA_SEARCH)) {}
        ImGui::SameLine();
        if (DrUI::IconButton(ICON_FA_COG)) {}
        break;
    }
    case WidgetType::IconToggle: {
        static bool t1 = true, t2 = false, t3 = true;
        DrUI::IconToggle(ICON_FA_DICE, &t1);
        ImGui::SameLine();
        DrUI::IconToggle(ICON_FA_ROUTE, &t2);
        ImGui::SameLine();
        DrUI::IconToggle(ICON_FA_EYE, &t3);
        break;
    }
    case WidgetType::ColoredButton: {
        ImGui::PushStyleColor(ImGuiCol_Button, DrUI::Colors::Success);
        DrUI::Button(ICON_FA_CHECK " OK");
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, DrUI::Colors::Warning);
        DrUI::Button(ICON_FA_WARNING " \xd0\x92\xd0\xbd\xd0\xb8\xd0\xbc\xd0\xb0\xd0\xbd\xd0\xb8\xd0\xb5");
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, DrUI::Colors::Error);
        DrUI::Button(ICON_FA_TIMES " \xd0\x9e\xd1\x88\xd0\xb8\xd0\xb1\xd0\xba\xd0\xb0");
        ImGui::PopStyleColor();
        break;
    }
    case WidgetType::ToggleSwitch: {
        static bool s1 = true, s2 = false, s3 = true;
        DrUI::ToggleSwitch("##ps1", &s1);
        ImGui::SameLine();
        ImGui::Text(ICON_FA_EYE " \xd0\x9e\xd0\xbf\xd1\x86\xd0\xb8\xd1\x8f 1");
        DrUI::ToggleSwitch("##ps2", &s2);
        ImGui::SameLine();
        ImGui::Text(ICON_FA_SAVE " \xd0\x9e\xd0\xbf\xd1\x86\xd0\xb8\xd1\x8f 2");
        DrUI::ToggleSwitch("##ps3", &s3);
        ImGui::SameLine();
        ImGui::Text(ICON_FA_BOLT " \xd0\x9e\xd0\xbf\xd1\x86\xd0\xb8\xd1\x8f 3");
        break;
    }
    case WidgetType::InputText: {
        static char buf[256] = "Preview text";
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##ptext", buf, sizeof(buf));
        break;
    }
    case WidgetType::InputTextMultiline: {
        static char buf[1024] =
            "\xd0\x9c\xd0\xbd\xd0\xbe\xd0\xb3\xd0\xbe\xd1\x81\xd1\x82\xd1\x80"
            "\xd0\xbe\xd1\x87\xd0\xbd\xd1\x8b\xd0\xb9\n\xd1\x82\xd0\xb5\xd0\xba"
            "\xd1\x81\xd1\x82";
        ImGui::InputTextMultiline("##pmulti", buf, sizeof(buf),
                                  ImVec2(-1, 80.0f * dpi_scale));
        break;
    }
    case WidgetType::InputInt: {
        static int val = 42;
        DrUI::InputInt("Int", &val, 1, 10, 120.0f * dpi_scale);
        break;
    }
    case WidgetType::InputFloat: {
        static float val = 3.14f;
        DrUI::InputFloat("Float", &val, 0.1f, 1.0f, "%.2f",
                         120.0f * dpi_scale);
        break;
    }
    case WidgetType::TagInput: {
        static std::vector<std::string> tags = {"tag1", "tag2"};
        static char buf[64] = "";
        DrUI::TagInput("##ptags", tags, buf, sizeof(buf));
        break;
    }
    case WidgetType::SliderFloat: {
        static float val = 0.5f;
        DrUI::SliderFloat("\xd0\x97\xd0\xbd\xd0\xb0\xd1\x87\xd0\xb5\xd0\xbd\xd0\xb8\xd0\xb5",
                          &val, 0.0f, 1.0f);
        break;
    }
    case WidgetType::SliderInt: {
        static int val = 50;
        DrUI::SliderInt("\xd0\xa8\xd0\xb0\xd0\xb3\xd0\xb8", &val, 0, 100);
        break;
    }
    case WidgetType::ProgressBar: {
        static float val = 0.65f;
        DrUI::ProgressBar(val, ImVec2(-1, 0), "65%");
        break;
    }
    case WidgetType::Spinner: {
        DrUI::Spinner("##pspin", 8.0f, 2.5f);
        ImGui::SameLine();
        ImGui::TextDisabled("\xd0\x97\xd0\xb0\xd0\xb3\xd1\x80\xd1\x83\xd0\xb7\xd0\xba\xd0\xb0...");
        break;
    }
    case WidgetType::Checkbox: {
        static bool c1 = true, c2 = false, c3 = true;
        ImGui::Checkbox("\xd0\x9e\xd0\xbf\xd1\x86\xd0\xb8\xd1\x8f A", &c1);
        ImGui::Checkbox("\xd0\x9e\xd0\xbf\xd1\x86\xd0\xb8\xd1\x8f B", &c2);
        ImGui::Checkbox("\xd0\x9e\xd0\xbf\xd1\x86\xd0\xb8\xd1\x8f C", &c3);
        break;
    }
    case WidgetType::RadioButton: {
        static int r = 0;
        ImGui::RadioButton("\xd0\xa0\xd0\xb5\xd0\xb6\xd0\xb8\xd0\xbc 1", &r, 0);
        ImGui::SameLine();
        ImGui::RadioButton("\xd0\xa0\xd0\xb5\xd0\xb6\xd0\xb8\xd0\xbc 2", &r, 1);
        ImGui::SameLine();
        ImGui::RadioButton("\xd0\xa0\xd0\xb5\xd0\xb6\xd0\xb8\xd0\xbc 3", &r, 2);
        break;
    }
    case WidgetType::Combo: {
        static int sel = 0;
        const char* items[] = {"Sequence", "Silhouette", "Custom"};
        ImGui::SetNextItemWidth(200.0f * dpi_scale);
        ImGui::Combo("\xd0\xa2\xd0\xb8\xd0\xbf", &sel, items, 3);
        break;
    }
    case WidgetType::ColorEdit4: {
        static ImVec4 col(0.37f, 0.55f, 0.95f, 1.0f);
        ImGui::ColorEdit4("Color", &col.x);
        break;
    }
    case WidgetType::ShimmerText: {
        DrUI::ShimmerText("Hello, Shimmer World!");
        ImGui::Spacing();
        DrUI::ShimmerText("\xd0\x9f\xd0\xb5\xd1\x80\xd0\xb5\xd0\xbb\xd0\xb8\xd0\xb2\xd0\xb0\xd1\x8e\xd1\x89\xd0\xb8\xd0\xb9\xd1\x81\xd1\x8f \xd1\x82\xd0\xb5\xd0\xba\xd1\x81\xd1\x82");
        ImGui::Spacing();
        DrUI::ShimmerText(ICON_FA_COG " AI is thinking...", 1.2f, 0.25f);
        break;
    }
    case WidgetType::GradientBorder: {
        static const ImVec4 colors[] = {
            {0.90f, 0.30f, 0.50f, 1.0f}, {0.30f, 0.80f, 0.95f, 1.0f},
            {0.95f, 0.75f, 0.20f, 1.0f}, {0.40f, 0.90f, 0.50f, 1.0f},
        };
        float t = (float)ImGui::GetTime();
        ImDrawList* wdl = ImGui::GetWindowDrawList();
        float w = ImGui::GetContentRegionAvail().x;
        ImVec2 rp = ImGui::GetCursorScreenPos();
        float rh = 60.0f * dpi_scale;
        ImGui::Dummy(ImVec2(w, rh));
        DrUI::GradientBorder(wdl, rp, ImVec2(w, rh),
                             colors, 4, t, 0.2f, 2.0f, 10.0f);
        break;
    }
    case WidgetType::GlowGradientBorder: {
        static const ImVec4 colors[] = {
            {0.90f, 0.30f, 0.50f, 1.0f}, {0.30f, 0.80f, 0.95f, 1.0f},
            {0.95f, 0.75f, 0.20f, 1.0f}, {0.40f, 0.90f, 0.50f, 1.0f},
        };
        float t = (float)ImGui::GetTime();
        ImDrawList* wdl = ImGui::GetWindowDrawList();
        float w = ImGui::GetContentRegionAvail().x;
        float pad = 14.0f;
        ImVec2 rp = ImGui::GetCursorScreenPos();
        rp.x += pad; rp.y += pad;
        float rh = 60.0f * dpi_scale;
        ImGui::Dummy(ImVec2(w, rh + pad * 2.0f));
        DrUI::GlowGradientBorder(wdl, rp, ImVec2(w - pad * 2.0f, rh),
                                 colors, 4, t, 0.2f, 2.0f, 10.0f);
        break;
    }
    case WidgetType::SkeuomorphCard: {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        float card_sz = 220.0f * dpi_scale;
        float card_pad = 24.0f * dpi_scale;
        float total = card_sz + card_pad * 2.0f;
        float avail_w = ImGui::GetContentRegionAvail().x;
        ImVec2 cursor = ImGui::GetCursorScreenPos();
        float bx = cursor.x + (avail_w - total) * 0.5f;
        float by = cursor.y;
        float bg_rnd = 8.0f * dpi_scale;
        dl->AddRectFilled(ImVec2(bx, by), ImVec2(bx + total, by + total),
                          IM_COL32(237, 241, 244, 255), bg_rnd);
        dl->AddRect(ImVec2(bx, by), ImVec2(bx + total, by + total),
                    IM_COL32(220, 222, 228, 180), bg_rnd, 0, 1.0f);
        DrawSkeuomorphCardImpl(dl, ImVec2(bx + card_pad, by + card_pad),
                               card_sz, g_card_active, dpi_scale);
        ImGui::Dummy(ImVec2(avail_w, total));
        break;
    }
    case WidgetType::SkeuomorphSlider: {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        float sl_w = 420.0f * dpi_scale;
        float sl_h = 50.0f * dpi_scale;
        float sl_pad = 20.0f * dpi_scale;
        float thumb_ext = sl_h * 0.58f;
        float total_w = sl_w + sl_pad * 2.0f;
        float total_h = sl_h + sl_pad * 2.0f + (thumb_ext - sl_h * 0.5f) * 2.0f;
        float avail_w = ImGui::GetContentRegionAvail().x;
        ImVec2 cursor = ImGui::GetCursorScreenPos();
        float bx = cursor.x + (avail_w - total_w) * 0.5f;
        float by = cursor.y;
        float bg_rnd = 8.0f * dpi_scale;
        dl->AddRectFilled(ImVec2(bx, by), ImVec2(bx + total_w, by + total_h),
                          IM_COL32(237, 241, 244, 255), bg_rnd);
        dl->AddRect(ImVec2(bx, by), ImVec2(bx + total_w, by + total_h),
                    IM_COL32(220, 222, 228, 180), bg_rnd, 0, 1.0f);
        float sx = bx + sl_pad;
        float sy = by + (total_h - sl_h) * 0.5f;
        DrawSkeuomorphSliderImpl(dl, ImVec2(sx, sy), sl_w, sl_h,
                                 &g_skeuo_slider_val, dpi_scale);
        ImGui::Dummy(ImVec2(avail_w, total_h));
        break;
    }
    case WidgetType::SkeuoButton: {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        float bw = 160.0f * dpi_scale, bh = 38.0f * dpi_scale;
        float pad = 16.0f * dpi_scale;
        float tw = bw + pad * 2.0f, th = bh + pad * 2.0f;
        float aw = ImGui::GetContentRegionAvail().x;
        ImVec2 cur = ImGui::GetCursorScreenPos();
        float bx = cur.x + (aw - tw) * 0.5f, by = cur.y;
        dl->AddRectFilled(ImVec2(bx, by), ImVec2(bx + tw, by + th),
                          IM_COL32(237, 241, 244, 255), 8.0f * dpi_scale);
        dl->AddRect(ImVec2(bx, by), ImVec2(bx + tw, by + th),
                    IM_COL32(220, 222, 228, 180), 8.0f * dpi_scale, 0, 1.0f);
        static bool skeuo_btn_st = false;
        DrawSkeuoButtonImpl(dl, ImVec2(bx + pad, by + pad), bw, bh,
                            "Button", &skeuo_btn_st, dpi_scale);
        ImGui::Dummy(ImVec2(aw, th));
        break;
    }
    case WidgetType::SkeuoToggle: {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        float tw = 56.0f * dpi_scale, th = 28.0f * dpi_scale;
        float pad = 16.0f * dpi_scale;
        float totw = tw + pad * 2.0f, toth = th + pad * 2.0f;
        float aw = ImGui::GetContentRegionAvail().x;
        ImVec2 cur = ImGui::GetCursorScreenPos();
        float bx = cur.x + (aw - totw) * 0.5f, by = cur.y;
        dl->AddRectFilled(ImVec2(bx, by), ImVec2(bx + totw, by + toth),
                          IM_COL32(237, 241, 244, 255), 8.0f * dpi_scale);
        dl->AddRect(ImVec2(bx, by), ImVec2(bx + totw, by + toth),
                    IM_COL32(220, 222, 228, 180), 8.0f * dpi_scale, 0, 1.0f);
        static bool skeuo_tog_st = false;
        DrawSkeuoToggleImpl(dl, ImVec2(bx + pad, by + pad), tw, th,
                            &skeuo_tog_st, dpi_scale);
        ImGui::Dummy(ImVec2(aw, toth));
        break;
    }
    case WidgetType::SkeuoCheckbox: {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        float sz = 22.0f * dpi_scale;
        float pad = 16.0f * dpi_scale;
        float totw = 200.0f * dpi_scale + pad * 2.0f;
        float toth = sz + pad * 2.0f;
        float aw = ImGui::GetContentRegionAvail().x;
        ImVec2 cur = ImGui::GetCursorScreenPos();
        float bx = cur.x + (aw - totw) * 0.5f, by = cur.y;
        dl->AddRectFilled(ImVec2(bx, by), ImVec2(bx + totw, by + toth),
                          IM_COL32(237, 241, 244, 255), 8.0f * dpi_scale);
        dl->AddRect(ImVec2(bx, by), ImVec2(bx + totw, by + toth),
                    IM_COL32(220, 222, 228, 180), 8.0f * dpi_scale, 0, 1.0f);
        static bool skeuo_chk_st = true;
        DrawSkeuoCheckboxImpl(dl, ImVec2(bx + pad, by + pad), sz,
                              &skeuo_chk_st, "Option", dpi_scale);
        ImGui::Dummy(ImVec2(aw, toth));
        break;
    }
    case WidgetType::SkeuoRadio: {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        float sz = 22.0f * dpi_scale;
        float pad = 16.0f * dpi_scale;
        float totw = 200.0f * dpi_scale + pad * 2.0f;
        float toth = sz + pad * 2.0f;
        float aw = ImGui::GetContentRegionAvail().x;
        ImVec2 cur = ImGui::GetCursorScreenPos();
        float bx = cur.x + (aw - totw) * 0.5f, by = cur.y;
        dl->AddRectFilled(ImVec2(bx, by), ImVec2(bx + totw, by + toth),
                          IM_COL32(237, 241, 244, 255), 8.0f * dpi_scale);
        dl->AddRect(ImVec2(bx, by), ImVec2(bx + totw, by + toth),
                    IM_COL32(220, 222, 228, 180), 8.0f * dpi_scale, 0, 1.0f);
        DrawSkeuoRadioImpl(dl, ImVec2(bx + pad, by + pad), sz,
                           true, "Selected", dpi_scale);
        ImGui::Dummy(ImVec2(aw, toth));
        break;
    }
    case WidgetType::SkeuoInput: {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        float iw = 300.0f * dpi_scale, ih = 32.0f * dpi_scale;
        float pad = 16.0f * dpi_scale;
        float totw = iw + pad * 2.0f, toth = ih + pad * 2.0f;
        float aw = ImGui::GetContentRegionAvail().x;
        ImVec2 cur = ImGui::GetCursorScreenPos();
        float bx = cur.x + (aw - totw) * 0.5f, by = cur.y;
        dl->AddRectFilled(ImVec2(bx, by), ImVec2(bx + totw, by + toth),
                          IM_COL32(237, 241, 244, 255), 8.0f * dpi_scale);
        dl->AddRect(ImVec2(bx, by), ImVec2(bx + totw, by + toth),
                    IM_COL32(220, 222, 228, 180), 8.0f * dpi_scale, 0, 1.0f);
        DrawSkeuoInputImpl(dl, ImVec2(bx + pad, by + pad), iw, ih,
                           "Hello, Skeuomorph!", dpi_scale);
        ImGui::Dummy(ImVec2(aw, toth));
        break;
    }
    case WidgetType::SkeuoProgress: {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        float pw = 300.0f * dpi_scale, ph = 24.0f * dpi_scale;
        float pad = 16.0f * dpi_scale;
        float totw = pw + pad * 2.0f, toth = ph + pad * 2.0f;
        float aw = ImGui::GetContentRegionAvail().x;
        ImVec2 cur = ImGui::GetCursorScreenPos();
        float bx = cur.x + (aw - totw) * 0.5f, by = cur.y;
        dl->AddRectFilled(ImVec2(bx, by), ImVec2(bx + totw, by + toth),
                          IM_COL32(237, 241, 244, 255), 8.0f * dpi_scale);
        dl->AddRect(ImVec2(bx, by), ImVec2(bx + totw, by + toth),
                    IM_COL32(220, 222, 228, 180), 8.0f * dpi_scale, 0, 1.0f);
        DrawSkeuoProgressBarImpl(dl, ImVec2(bx + pad, by + pad), pw, ph,
                                 0.65f, dpi_scale);
        ImGui::Dummy(ImVec2(aw, toth));
        break;
    }
    }
    ImGui::PopID();
}

// ===================================================================
// Brushed Aluminum Panel (procedural metal texture)
// ===================================================================

static inline unsigned int AlumHash(unsigned int x) {
    x ^= x >> 16; x *= 0x45d9f3bU;
    x ^= x >> 16; x *= 0x45d9f3bU;
    x ^= x >> 16;
    return x;
}

static inline float AlumCornerInset(float dy, float r) {
    if (dy >= r) return 0.0f;
    float d = r - dy;
    return r - std::sqrt(r * r - d * d);
}

static void DrawBrushedAluminumPanel(ImDrawList* dl, ImVec2 pos,
                                      float w, float h, float rnd, float zm) {
    ImVec2 p0 = pos, p1(pos.x + w, pos.y + h);

    // --- Outer drop shadow (multi-pass, offset down) ---
    {
        constexpr int kPasses = 10;
        constexpr float kSpread = 10.0f;
        float sdy = 4.0f * zm;
        for (int i = kPasses; i >= 0; --i) {
            float f = (float)i / (float)kPasses;
            float ex = kSpread * f * zm;
            int a = (int)(18.0f * (1.0f - f) * (1.0f - f));
            dl->AddRectFilled(
                ImVec2(p0.x - ex, p0.y + sdy - ex),
                ImVec2(p1.x + ex, p1.y + sdy + ex),
                IM_COL32(0, 0, 0, a), rnd + ex);
        }
    }

    // --- Base rounded rect (clip shape) ---
    dl->AddRectFilled(p0, p1, IM_COL32(192, 196, 202, 255), rnd);

    // --- Horizontal grain + reflection curve (segmented gradient) ---
    {
        float step = std::max(1.0f, zm);

        for (float ly = 0.0f; ly < h; ly += step) {
            float t = ly / h;
            int iy = (int)(ly / step);

            // Base luminance from reflection curve
            float lum;
            if      (t < 0.15f) lum = 230.0f - (230.0f - 218.0f) * (t / 0.15f);
            else if (t < 0.25f) lum = 218.0f + (238.0f - 218.0f) * ((t - 0.15f) / 0.10f);
            else if (t < 0.35f) lum = 238.0f - (238.0f - 235.0f) * ((t - 0.25f) / 0.10f);
            else if (t < 0.55f) lum = 235.0f - (235.0f - 195.0f) * ((t - 0.35f) / 0.20f);
            else if (t < 0.85f) lum = 195.0f - (195.0f - 185.0f) * ((t - 0.55f) / 0.30f);
            else                lum = 185.0f + (192.0f - 185.0f) * ((t - 0.85f) / 0.15f);

            float fineNoise = (float)(AlumHash((unsigned int)iy) & 0xFFF) / 4095.0f;
            lum += (fineNoise - 0.5f) * 2.5f;

            if ((iy % 5) == 0) {
                float coarseNoise = (float)(AlumHash((unsigned int)(iy * 7 + 31)) & 0xFFF) / 4095.0f;
                lum += (coarseNoise - 0.5f) * 4.0f;
            }

            if ((AlumHash((unsigned int)(iy * 13 + 97)) & 0x3F) == 0)
                lum -= 3.0f;

            float insetTop = AlumCornerInset(ly, rnd);
            float insetBot = AlumCornerInset(h - ly - step, rnd);
            float inset = std::max(insetTop, insetBot);

            float lx0 = p0.x + inset;
            float lx1 = p1.x - inset;
            float lineW = lx1 - lx0;
            if (lineW <= 0.0f) continue;

            float lineY0 = p0.y + ly;
            float lineY1 = lineY0 + step;

            // Number of segments: 4-12, deterministic per line
            int nSeg = 4 + (int)(AlumHash((unsigned int)(iy * 3 + 5)) % 9);
            int nPts = nSeg + 1;

            // Build control points: x-positions and luminance offsets
            float ptX[14];
            float ptLum[14];
            ptX[0] = lx0;
            ptX[nPts - 1] = lx1;

            float segLen = lineW / (float)nSeg;
            for (int k = 1; k < nPts - 1; ++k) {
                float idealX = lx0 + segLen * (float)k;
                float jitter = (float)((int)(AlumHash((unsigned int)(iy * 17 + k * 131)) & 0xFFF) - 2048) / 2048.0f;
                ptX[k] = idealX + jitter * segLen * 0.20f;
                if (ptX[k] <= ptX[k - 1] + 1.0f) ptX[k] = ptX[k - 1] + 1.0f;
                if (ptX[k] >= lx1 - 1.0f) ptX[k] = lx1 - 1.0f;
            }

            // Brightness offset per point: +-7.5% of base lum
            for (int k = 0; k < nPts; ++k) {
                float nf = (float)((int)(AlumHash((unsigned int)(iy * 23 + k * 71)) & 0xFFF) - 2048) / 2048.0f;
                ptLum[k] = lum * (1.0f + nf * 0.075f);
            }

            // Draw gradient sub-segments
            for (int k = 0; k < nSeg; ++k) {
                float x0 = ptX[k], x1 = ptX[k + 1];
                if (x1 <= x0) continue;

                float lumL = ptLum[k], lumR = ptLum[k + 1];
                int rL = std::min(255, std::max(0, (int)(lumL - 2.0f)));
                int gL = std::min(255, std::max(0, (int)(lumL)));
                int bL = std::min(255, std::max(0, (int)(lumL + 3.0f)));
                int rR = std::min(255, std::max(0, (int)(lumR - 2.0f)));
                int gR = std::min(255, std::max(0, (int)(lumR)));
                int bR = std::min(255, std::max(0, (int)(lumR + 3.0f)));

                ImU32 colL = IM_COL32(rL, gL, bL, 255);
                ImU32 colR = IM_COL32(rR, gR, bR, 255);
                dl->AddRectFilledMultiColor(
                    ImVec2(x0, lineY0), ImVec2(x1, lineY1),
                    colL, colR, colR, colL);
            }
        }
    }

    // --- Second pass: sparse overlay lines (different step, breaks regularity) ---
    {
        float step2 = std::max(3.0f, 3.0f * zm);

        for (float ly = 0.0f; ly < h; ly += step2) {
            int iy2 = (int)(ly / step2);

            // Skip ~40% of lines for irregularity
            if ((AlumHash((unsigned int)(iy2 * 37 + 199)) & 7) < 3) continue;

            float t = ly / h;

            // Reuse same reflection curve for base lum
            float lum;
            if      (t < 0.15f) lum = 230.0f - (230.0f - 218.0f) * (t / 0.15f);
            else if (t < 0.25f) lum = 218.0f + (238.0f - 218.0f) * ((t - 0.15f) / 0.10f);
            else if (t < 0.35f) lum = 238.0f - (238.0f - 235.0f) * ((t - 0.25f) / 0.10f);
            else if (t < 0.55f) lum = 235.0f - (235.0f - 195.0f) * ((t - 0.35f) / 0.20f);
            else if (t < 0.85f) lum = 195.0f - (195.0f - 185.0f) * ((t - 0.55f) / 0.30f);
            else                lum = 185.0f + (192.0f - 185.0f) * ((t - 0.85f) / 0.15f);

            // Per-line variation for the overlay
            float n = (float)(AlumHash((unsigned int)(iy2 * 53 + 7)) & 0xFFF) / 4095.0f;
            lum += (n - 0.5f) * 7.0f;

            int r = std::min(255, std::max(0, (int)(lum - 2.0f)));
            int g = std::min(255, std::max(0, (int)(lum)));
            int b = std::min(255, std::max(0, (int)(lum + 3.0f)));

            float insetTop = AlumCornerInset(ly, rnd);
            float insetBot = AlumCornerInset(h - ly - step2, rnd);
            float inset = std::max(insetTop, insetBot);

            float lx0 = p0.x + inset;
            float lx1 = p1.x - inset;
            if (lx1 <= lx0) continue;

            float lineW = lx1 - lx0;
            if (lineW <= 0.0f) continue;

            // Random length: 20-80% of full width
            float pctLen = 0.20f + 0.60f * ((float)(AlumHash((unsigned int)(iy2 * 11 + 41)) & 0xFFF) / 4095.0f);
            float segW = lineW * pctLen;

            // Random offset within remaining space
            float maxOff = lineW - segW;
            float off = maxOff * ((float)(AlumHash((unsigned int)(iy2 * 29 + 83)) & 0xFFF) / 4095.0f);

            float sx0 = lx0 + off;
            float sx1 = sx0 + segW;

            int alpha = 60 + (int)(AlumHash((unsigned int)(iy2 * 43 + 61)) % 50);

            dl->AddLine(ImVec2(sx0, p0.y + ly + step2 * 0.5f),
                        ImVec2(sx1, p0.y + ly + step2 * 0.5f),
                        IM_COL32(r, g, b, alpha), std::max(0.5f, 0.5f * zm));
        }
    }

    // --- Specular highlight bloom (gaussian, ~30% from top) ---
    {
        float peakY = h * 0.28f;
        float sigma = h * 0.07f;
        constexpr int kSteps = 16;
        float bandTop = peakY - sigma * 3.0f;
        float bandBot = peakY + sigma * 3.0f;
        float stripH = (bandBot - bandTop) / (float)kSteps;

        for (int i = 0; i < kSteps; ++i) {
            float cy = bandTop + stripH * ((float)i + 0.5f);
            float d = (cy - peakY) / sigma;
            float intensity = std::exp(-0.5f * d * d);
            int a = (int)(45.0f * intensity);
            if (a < 1) continue;

            float dy = cy;
            float insetT = AlumCornerInset(dy, rnd);
            float insetB = AlumCornerInset(h - dy - stripH, rnd);
            float inset = std::max(insetT, insetB);

            dl->AddRectFilled(
                ImVec2(p0.x + inset, p0.y + cy - stripH * 0.5f),
                ImVec2(p1.x - inset, p0.y + cy + stripH * 0.5f),
                IM_COL32(255, 255, 255, a));
        }
    }

    // --- Inner bevel ---
    for (int i = 0; i < 3; ++i) {
        float off = (float)(i + 1) * zm;
        int a = (int)(60.0f / (float)(i + 1));
        dl->AddLine(ImVec2(p0.x + rnd, p0.y + off),
                    ImVec2(p1.x - rnd, p0.y + off),
                    IM_COL32(255, 255, 255, a), zm);
    }
    for (int i = 0; i < 3; ++i) {
        float off = (float)(i + 1) * zm;
        int a = (int)(35.0f / (float)(i + 1));
        dl->AddLine(ImVec2(p0.x + rnd, p1.y - off),
                    ImVec2(p1.x - rnd, p1.y - off),
                    IM_COL32(0, 0, 0, a), zm);
    }
    for (int i = 0; i < 2; ++i) {
        float off = (float)(i + 1) * zm;
        int a = (int)(30.0f / (float)(i + 1));
        dl->AddLine(ImVec2(p0.x + off, p0.y + rnd),
                    ImVec2(p0.x + off, p1.y - rnd),
                    IM_COL32(255, 255, 255, a), zm);
    }
    for (int i = 0; i < 2; ++i) {
        float off = (float)(i + 1) * zm;
        int a = (int)(20.0f / (float)(i + 1));
        dl->AddLine(ImVec2(p1.x - off, p0.y + rnd),
                    ImVec2(p1.x - off, p1.y - rnd),
                    IM_COL32(0, 0, 0, a), zm);
    }

    // --- Border ---
    dl->AddRect(p0, p1, IM_COL32(150, 155, 162, 200), rnd, 0, 1.2f * zm);
}

// ===================================================================
// Brushed Aluminum Panel — Skeuomorphic variant (raised, lit from top-left)
// ===================================================================

static void DrawBrushedAluminumPanelSkeuo(ImDrawList* dl, ImVec2 pos,
                                           float w, float h, float rnd, float zm) {
    ImVec2 p0 = pos, p1(pos.x + w, pos.y + h);

    // === Outer shadows (same principle as DrawSkeuoButtonImpl raised state) ===

    // Dark shadow: offset down-right (2x thickness)
    {
        constexpr int passes = 12;
        constexpr float spread = 16.0f;
        float sdx = 6.0f * zm, sdy = 6.0f * zm;
        for (int i = passes; i >= 0; --i) {
            float f = (float)i / (float)passes;
            float ex = spread * f * zm;
            int a = (int)(16.0f * (1.0f - f) * (1.0f - f));
            dl->AddRectFilled(ImVec2(p0.x + sdx - ex, p0.y + sdy - ex),
                              ImVec2(p1.x + sdx + ex, p1.y + sdy + ex),
                              IM_COL32(0, 0, 0, a), rnd + ex);
        }
    }
    // Light glow: offset up-left (2x thickness)
    {
        constexpr int passes = 12;
        constexpr float spread = 16.0f;
        float lx = -5.0f * zm, ly = -5.0f * zm;
        for (int i = passes; i >= 0; --i) {
            float f = (float)i / (float)passes;
            float ex = spread * f * zm;
            int a = (int)(30.0f * (1.0f - f) * (1.0f - f));
            dl->AddRectFilled(ImVec2(p0.x + lx - ex, p0.y + ly - ex),
                              ImVec2(p1.x + lx + ex, p1.y + ly + ex),
                              IM_COL32(255, 255, 255, a), rnd + ex);
        }
    }

    // === Base fill (same brushed texture) ===
    dl->AddRectFilled(p0, p1, IM_COL32(192, 196, 202, 255), rnd);

    // === Horizontal grain + reflection curve (segmented gradient) ===
    {
        float step = std::max(1.0f, zm);
        for (float ly = 0.0f; ly < h; ly += step) {
            float t = ly / h;
            int iy = (int)(ly / step);

            float lum;
            if      (t < 0.15f) lum = 230.0f - (230.0f - 218.0f) * (t / 0.15f);
            else if (t < 0.25f) lum = 218.0f + (238.0f - 218.0f) * ((t - 0.15f) / 0.10f);
            else if (t < 0.35f) lum = 238.0f - (238.0f - 235.0f) * ((t - 0.25f) / 0.10f);
            else if (t < 0.55f) lum = 235.0f - (235.0f - 195.0f) * ((t - 0.35f) / 0.20f);
            else if (t < 0.85f) lum = 195.0f - (195.0f - 185.0f) * ((t - 0.55f) / 0.30f);
            else                lum = 185.0f + (192.0f - 185.0f) * ((t - 0.85f) / 0.15f);

            float fineNoise = (float)(AlumHash((unsigned int)(iy + 500)) & 0xFFF) / 4095.0f;
            lum += (fineNoise - 0.5f) * 2.5f;
            if ((iy % 5) == 0) {
                float cn = (float)(AlumHash((unsigned int)(iy * 7 + 531)) & 0xFFF) / 4095.0f;
                lum += (cn - 0.5f) * 4.0f;
            }
            if ((AlumHash((unsigned int)(iy * 13 + 597)) & 0x3F) == 0)
                lum -= 3.0f;

            float insetTop = AlumCornerInset(ly, rnd);
            float insetBot = AlumCornerInset(h - ly - step, rnd);
            float inset = std::max(insetTop, insetBot);
            float lx0 = p0.x + inset, lx1 = p1.x - inset;
            float lineW = lx1 - lx0;
            if (lineW <= 0.0f) continue;

            float lineY0 = p0.y + ly, lineY1 = lineY0 + step;

            int nSeg = 4 + (int)(AlumHash((unsigned int)(iy * 3 + 505)) % 9);
            int nPts = nSeg + 1;
            float ptX[14], ptLum[14];
            ptX[0] = lx0; ptX[nPts - 1] = lx1;
            float segLen = lineW / (float)nSeg;
            for (int k = 1; k < nPts - 1; ++k) {
                float idealX = lx0 + segLen * (float)k;
                float jitter = (float)((int)(AlumHash((unsigned int)(iy * 17 + k * 131 + 500)) & 0xFFF) - 2048) / 2048.0f;
                ptX[k] = idealX + jitter * segLen * 0.20f;
                if (ptX[k] <= ptX[k - 1] + 1.0f) ptX[k] = ptX[k - 1] + 1.0f;
                if (ptX[k] >= lx1 - 1.0f) ptX[k] = lx1 - 1.0f;
            }
            for (int k = 0; k < nPts; ++k) {
                float nf = (float)((int)(AlumHash((unsigned int)(iy * 23 + k * 71 + 500)) & 0xFFF) - 2048) / 2048.0f;
                ptLum[k] = lum * (1.0f + nf * 0.075f);
            }
            for (int k = 0; k < nSeg; ++k) {
                float x0 = ptX[k], x1 = ptX[k + 1];
                if (x1 <= x0) continue;
                float lumL = ptLum[k], lumR = ptLum[k + 1];
                int rL = std::min(255, std::max(0, (int)(lumL - 2.0f)));
                int gL = std::min(255, std::max(0, (int)(lumL)));
                int bL = std::min(255, std::max(0, (int)(lumL + 3.0f)));
                int rR = std::min(255, std::max(0, (int)(lumR - 2.0f)));
                int gR = std::min(255, std::max(0, (int)(lumR)));
                int bR = std::min(255, std::max(0, (int)(lumR + 3.0f)));
                dl->AddRectFilledMultiColor(
                    ImVec2(x0, lineY0), ImVec2(x1, lineY1),
                    IM_COL32(rL, gL, bL, 255), IM_COL32(rR, gR, bR, 255),
                    IM_COL32(rR, gR, bR, 255), IM_COL32(rL, gL, bL, 255));
            }
        }
    }

    // === Second pass: sparse overlay ===
    {
        float step2 = std::max(3.0f, 3.0f * zm);
        for (float ly = 0.0f; ly < h; ly += step2) {
            int iy2 = (int)(ly / step2);
            if ((AlumHash((unsigned int)(iy2 * 37 + 699)) & 7) < 3) continue;
            float t = ly / h;
            float lum;
            if      (t < 0.15f) lum = 230.0f - (230.0f - 218.0f) * (t / 0.15f);
            else if (t < 0.25f) lum = 218.0f + (238.0f - 218.0f) * ((t - 0.15f) / 0.10f);
            else if (t < 0.35f) lum = 238.0f - (238.0f - 235.0f) * ((t - 0.25f) / 0.10f);
            else if (t < 0.55f) lum = 235.0f - (235.0f - 195.0f) * ((t - 0.35f) / 0.20f);
            else if (t < 0.85f) lum = 195.0f - (195.0f - 185.0f) * ((t - 0.55f) / 0.30f);
            else                lum = 185.0f + (192.0f - 185.0f) * ((t - 0.85f) / 0.15f);
            float n = (float)(AlumHash((unsigned int)(iy2 * 53 + 507)) & 0xFFF) / 4095.0f;
            lum += (n - 0.5f) * 7.0f;
            int r = std::min(255, std::max(0, (int)(lum - 2.0f)));
            int g = std::min(255, std::max(0, (int)(lum)));
            int b = std::min(255, std::max(0, (int)(lum + 3.0f)));
            float insetTop = AlumCornerInset(ly, rnd);
            float insetBot = AlumCornerInset(h - ly - step2, rnd);
            float inset = std::max(insetTop, insetBot);
            float lx0 = p0.x + inset, lx1 = p1.x - inset;
            float lineW = lx1 - lx0;
            if (lineW <= 0.0f) continue;
            float pctLen = 0.20f + 0.60f * ((float)(AlumHash((unsigned int)(iy2 * 11 + 541)) & 0xFFF) / 4095.0f);
            float segW = lineW * pctLen;
            float maxOff = lineW - segW;
            float off = maxOff * ((float)(AlumHash((unsigned int)(iy2 * 29 + 583)) & 0xFFF) / 4095.0f);
            float sx0 = lx0 + off, sx1 = sx0 + segW;
            int alpha = 60 + (int)(AlumHash((unsigned int)(iy2 * 43 + 561)) % 50);
            dl->AddLine(ImVec2(sx0, p0.y + ly + step2 * 0.5f),
                        ImVec2(sx1, p0.y + ly + step2 * 0.5f),
                        IM_COL32(r, g, b, alpha), std::max(0.5f, 0.5f * zm));
        }
    }

    // === Specular highlight bloom ===
    {
        float peakY = h * 0.28f;
        float sigma = h * 0.07f;
        constexpr int kSteps = 16;
        float bandTop = peakY - sigma * 3.0f;
        float bandBot = peakY + sigma * 3.0f;
        float stripH = (bandBot - bandTop) / (float)kSteps;
        for (int i = 0; i < kSteps; ++i) {
            float cy = bandTop + stripH * ((float)i + 0.5f);
            float d = (cy - peakY) / sigma;
            float intensity = std::exp(-0.5f * d * d);
            int a = (int)(45.0f * intensity);
            if (a < 1) continue;
            float insetT = AlumCornerInset(cy, rnd);
            float insetB = AlumCornerInset(h - cy - stripH, rnd);
            float inset = std::max(insetT, insetB);
            dl->AddRectFilled(
                ImVec2(p0.x + inset, p0.y + cy - stripH * 0.5f),
                ImVec2(p1.x - inset, p0.y + cy + stripH * 0.5f),
                IM_COL32(255, 255, 255, a));
        }
    }

    // === Inner highlights only (no darkening on panel itself) ===

    // Top inner highlight
    for (int i = 0; i < 4; ++i) {
        float off = (float)(i + 1) * zm;
        int a = (int)(90.0f / (float)(i + 1));
        dl->AddLine(ImVec2(p0.x + rnd, p0.y + off),
                    ImVec2(p1.x - rnd, p0.y + off),
                    IM_COL32(255, 255, 255, a), zm);
    }
    // Left inner highlight
    for (int i = 0; i < 3; ++i) {
        float off = (float)(i + 1) * zm;
        int a = (int)(50.0f / (float)(i + 1));
        dl->AddLine(ImVec2(p0.x + off, p0.y + rnd),
                    ImVec2(p0.x + off, p1.y - rnd),
                    IM_COL32(255, 255, 255, a), zm);
    }

}

// ===================================================================
// Brushed Aluminum Button (small skeuo panel with icon + label)
// ===================================================================

static void DrawAluminumButton(ImDrawList* dl, ImVec2 pos,
                                float w, float h, float rnd, float zm,
                                const char* icon, const char* label,
                                ImU32 fillCol = IM_COL32(75, 90, 120, 255)) {
    ImVec2 p0 = pos, p1(pos.x + w, pos.y + h);

    // === Outer shadows (skeuo raised) ===
    // Dark shadow down-right
    {
        constexpr int passes = 12;
        constexpr float spread = 16.0f;
        float sdx = 6.0f * zm, sdy = 6.0f * zm;
        for (int i = passes; i >= 0; --i) {
            float f = (float)i / (float)passes;
            float ex = spread * f * zm;
            int a = (int)(16.0f * (1.0f - f) * (1.0f - f));
            dl->AddRectFilled(ImVec2(p0.x + sdx - ex, p0.y + sdy - ex),
                              ImVec2(p1.x + sdx + ex, p1.y + sdy + ex),
                              IM_COL32(0, 0, 0, a), rnd + ex);
        }
    }
    // Light glow up-left
    {
        constexpr int passes = 12;
        constexpr float spread = 16.0f;
        float lx = -5.0f * zm, ly = -5.0f * zm;
        for (int i = passes; i >= 0; --i) {
            float f = (float)i / (float)passes;
            float ex = spread * f * zm;
            int a = (int)(30.0f * (1.0f - f) * (1.0f - f));
            dl->AddRectFilled(ImVec2(p0.x + lx - ex, p0.y + ly - ex),
                              ImVec2(p1.x + lx + ex, p1.y + ly + ex),
                              IM_COL32(255, 255, 255, a), rnd + ex);
        }
    }

    // === Colored fill ===
    dl->AddRectFilled(p0, p1, fillCol, rnd);

    // === Icon + label (light text) ===
    ImU32 textCol = IM_COL32(242, 242, 242, 255);
    ImFont* font = ImGui::GetFont();
    float fsz = ImGui::GetFontSize() * zm;

    ImVec2 iconSz = font->CalcTextSizeA(fsz * 1.3f, FLT_MAX, 0, icon);
    ImVec2 labelSz = font->CalcTextSizeA(fsz * 0.85f, FLT_MAX, 0, label);

    float totalH = iconSz.y + 4.0f * zm + labelSz.y;
    float startY = pos.y + (h - totalH) * 0.5f;

    float iconX = pos.x + (w - iconSz.x) * 0.5f;
    dl->AddText(font, fsz * 1.3f, ImVec2(iconX, startY), textCol, icon);

    float labelX = pos.x + (w - labelSz.x) * 0.5f;
    float labelY = startY + iconSz.y + 4.0f * zm;
    dl->AddText(font, fsz * 0.85f, ImVec2(labelX, labelY), textCol, label);
}

// ===================================================================
// macOS Aqua-style Button (pill shape, gradient, gloss, glow)
// ===================================================================

static void DrawAquaButton(ImDrawList* dl, ImVec2 pos,
                            float w, float h, float zm,
                            ImU32 colTop, ImU32 colBot, ImU32 glowCol,
                            ImU32 textCol, const char* label) {
    ImVec2 p0 = pos, p1(pos.x + w, pos.y + h);
    float rnd = h * 0.5f;

    // Decompose colors
    auto u8r = [](ImU32 c) -> int { return (int)(c & 0xFF); };
    auto u8g = [](ImU32 c) -> int { return (int)((c >> 8) & 0xFF); };
    auto u8b = [](ImU32 c) -> int { return (int)((c >> 16) & 0xFF); };

    int tR = u8r(colTop), tG = u8g(colTop), tB = u8b(colTop);
    int bR = u8r(colBot), bG = u8g(colBot), bB = u8b(colBot);

    // --- Outer colored glow ---
    {
        int gR = u8r(glowCol), gG = u8g(glowCol), gB = u8b(glowCol);
        constexpr int passes = 8;
        constexpr float spread = 10.0f;
        for (int i = passes; i >= 0; --i) {
            float f = (float)i / (float)passes;
            float ex = spread * f * zm;
            int a = (int)(20.0f * (1.0f - f) * (1.0f - f));
            dl->AddRectFilled(
                ImVec2(p0.x - ex, p0.y - ex + 2.0f * zm),
                ImVec2(p1.x + ex, p1.y + ex + 2.0f * zm),
                IM_COL32(gR, gG, gB, a), rnd + ex);
        }
    }

    // --- Body: base fill then horizontal gradient strips ---
    dl->AddRectFilled(p0, p1, colBot, rnd);
    {
        float step = std::max(1.0f, zm);
        for (float ly = 0.0f; ly < h; ly += step) {
            float t = ly / h;
            int r = bR + (int)((float)(tR - bR) * (1.0f - t));
            int g = bG + (int)((float)(tG - bG) * (1.0f - t));
            int b = bB + (int)((float)(tB - bB) * (1.0f - t));

            float insetTop = AlumCornerInset(ly, rnd);
            float insetBot = AlumCornerInset(h - ly - step, rnd);
            float inset = std::max(insetTop, insetBot);

            float lx0 = p0.x + inset;
            float lx1 = p1.x - inset;
            if (lx1 <= lx0) continue;

            dl->AddRectFilled(
                ImVec2(lx0, p0.y + ly),
                ImVec2(lx1, p0.y + ly + step),
                IM_COL32(r, g, b, 255));
        }
    }

    // --- Top gloss (white -> transparent, upper ~45%) ---
    {
        float glossH = h * 0.45f;
        float insetX = 4.0f * zm;
        float glossRnd = rnd * 0.85f;
        float step = std::max(1.0f, zm);
        for (float ly = 0.0f; ly < glossH; ly += step) {
            float t = ly / glossH;
            int a = (int)(160.0f * (1.0f - t) * (1.0f - t));
            if (a < 1) continue;

            float dy_top = ly + 1.0f * zm;
            float insetT = AlumCornerInset(dy_top, glossRnd);
            float insetB = AlumCornerInset(glossH - ly, glossRnd);
            float inset = std::max(insetT, insetB);

            float lx0 = p0.x + insetX + inset;
            float lx1 = p1.x - insetX - inset;
            if (lx1 <= lx0) continue;

            dl->AddRectFilled(
                ImVec2(lx0, p0.y + 1.0f * zm + ly),
                ImVec2(lx1, p0.y + 1.0f * zm + ly + step),
                IM_COL32(255, 255, 255, a));
        }
    }

    // --- Bottom glow (blurred colored reflection) ---
    {
        float glowTop = h * 0.72f;
        float glowH = h * 0.22f;
        int gR = u8r(glowCol), gG = u8g(glowCol), gB = u8b(glowCol);
        constexpr int passes = 6;
        constexpr float spread = 4.0f;
        for (int i = passes; i >= 0; --i) {
            float f = (float)i / (float)passes;
            float ex = spread * f * zm;
            int a = (int)(25.0f * (1.0f - f) * (1.0f - f));
            float gy0 = p0.y + glowTop - ex;
            float gy1 = p0.y + glowTop + glowH + ex;
            float gx0 = p0.x + rnd * 0.3f - ex;
            float gx1 = p1.x - rnd * 0.3f + ex;
            dl->AddRectFilled(
                ImVec2(gx0, gy0), ImVec2(gx1, gy1),
                IM_COL32(gR, gG, gB, a), rnd * 0.5f);
        }
    }

    // --- Text (centered) ---
    ImFont* font = ImGui::GetFont();
    float fsz = ImGui::GetFontSize() * 0.9f * zm;
    ImVec2 tsz = font->CalcTextSizeA(fsz, FLT_MAX, 0, label);
    float tx = p0.x + (w - tsz.x) * 0.5f;
    float ty = p0.y + (h - tsz.y) * 0.5f;
    dl->AddText(font, fsz, ImVec2(tx, ty), textCol, label);
}

// ===================================================================
// Canvas panel (center) — pannable/zoomable workspace
// ===================================================================

struct CanvasState {
    ImVec2 pan        = ImVec2(0.0f, 0.0f);
    float  zoom       = 1.0f;
    bool   initialized = false;
};

static CanvasState g_canvas;

static void DrawCanvasPanel(const PanelLayout& zone, float dpi_scale) {
    ImGui::SetNextWindowPos(zone.pos);
    ImGui::SetNextWindowSize(zone.size);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("##Canvas", nullptr,
                 kPanelFlags | ImGuiWindowFlags_NoScrollbar |
                 ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::PopStyleVar();

    ImVec2 p0 = ImGui::GetCursorScreenPos();
    ImVec2 sz = ImGui::GetContentRegionAvail();
    ImVec2 p1(p0.x + sz.x, p0.y + sz.y);

    if (!g_canvas.initialized) {
        g_canvas.pan = ImVec2(sz.x * 0.5f, sz.y * 0.5f);
        g_canvas.initialized = true;
    }

    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(p0, p1, ImGui::ColorConvertFloat4ToU32(DrUI::Colors::CanvasBg));

    ImGui::InvisibleButton("##canvas_area", sz);
    bool canvas_hovered = ImGui::IsItemHovered();
    bool canvas_active  = ImGui::IsItemActive();

    ImVec2 prevSz = GetPreviewWorldSize(g_selected_widget, dpi_scale);
    float kPrevWorldW = prevSz.x;
    float kPrevWorldH = prevSz.y;

    if (ImGui::IsItemClicked(ImGuiMouseButton_Left) &&
        g_selected_widget != WidgetType::None) {
        float zm_ = g_canvas.zoom;
        float ox_ = p0.x + g_canvas.pan.x;
        float oy_ = p0.y + g_canvas.pan.y;
        ImVec2 mp = ImGui::GetIO().MousePos;
        float ptl_x = ox_ + (-kPrevWorldW * 0.5f) * zm_;
        float ptl_y = oy_ + (-kPrevWorldH * 0.5f) * zm_;
        float pbr_x = ox_ + ( kPrevWorldW * 0.5f) * zm_;
        float pbr_y = oy_ + ( kPrevWorldH * 0.5f) * zm_;
        if (mp.x < ptl_x || mp.x > pbr_x || mp.y < ptl_y || mp.y > pbr_y)
            g_selected_widget = WidgetType::None;
    }

    ImGuiIO& io = ImGui::GetIO();
    bool mouse_in_canvas = io.MousePos.x >= p0.x && io.MousePos.x <= p1.x &&
                           io.MousePos.y >= p0.y && io.MousePos.y <= p1.y;

    // Pan: middle-mouse or right-mouse drag
    if (mouse_in_canvas && (ImGui::IsMouseDragging(ImGuiMouseButton_Middle, 0.0f) ||
                            ImGui::IsMouseDragging(ImGuiMouseButton_Right, 0.0f))) {
        ImVec2 delta = ImGui::IsMouseDragging(ImGuiMouseButton_Middle, 0.0f)
                         ? io.MouseDelta : io.MouseDelta;
        g_canvas.pan.x += delta.x;
        g_canvas.pan.y += delta.y;
    }

    // Zoom: mouse wheel (Ctrl+scroll forwards to widget instead)
    if (mouse_in_canvas && !io.KeyCtrl && io.MouseWheel != 0.0f) {
        float zoom_factor = 1.1f;
        float old_zoom = g_canvas.zoom;
        if (io.MouseWheel > 0.0f)
            g_canvas.zoom = std::min(g_canvas.zoom * zoom_factor, 8.0f);
        else
            g_canvas.zoom = std::max(g_canvas.zoom / zoom_factor, 0.1f);

        ImVec2 mouse_canvas(io.MousePos.x - p0.x, io.MousePos.y - p0.y);
        float ratio = (1.0f - g_canvas.zoom / old_zoom);
        g_canvas.pan.x += (mouse_canvas.x - g_canvas.pan.x) * ratio;
        g_canvas.pan.y += (mouse_canvas.y - g_canvas.pan.y) * ratio;
    }

    // World-to-screen transform helpers
    float zm = g_canvas.zoom;
    float ox = p0.x + g_canvas.pan.x;
    float oy = p0.y + g_canvas.pan.y;
    auto w2s = [&](float wx, float wy) -> ImVec2 {
        return ImVec2(ox + wx * zm, oy + wy * zm);
    };

    dl->PushClipRect(p0, p1, true);

    // --- Grid ---
    {
        const float base_step = 24.0f;
        float step = base_step * zm;
        if (step < 4.0f) step *= std::ceil(4.0f / step);

        float start_x = std::fmod(ox - p0.x, step);
        if (start_x < 0.0f) start_x += step;
        float start_y = std::fmod(oy - p0.y, step);
        if (start_y < 0.0f) start_y += step;

        float major_step = step * 4.0f;
        float major_ref_x = std::fmod(ox - p0.x, major_step);
        if (major_ref_x < 0.0f) major_ref_x += major_step;
        float major_ref_y = std::fmod(oy - p0.y, major_step);
        if (major_ref_y < 0.0f) major_ref_y += major_step;

        for (float x = start_x; x < sz.x; x += step) {
            bool mx = std::fabs(std::fmod(x - major_ref_x, major_step)) < 0.5f;
            for (float y = start_y; y < sz.y; y += step) {
                bool my = std::fabs(std::fmod(y - major_ref_y, major_step)) < 0.5f;
                bool major = mx && my;
                float r   = major ? 1.8f : 1.0f;
                ImU32 col = major ? DrUI::Colors::CanvasDotMajor
                                  : DrUI::Colors::CanvasDot;
                dl->AddCircleFilled(ImVec2(p0.x + x, p0.y + y), r, col);
            }
        }
    }

    // --- Origin crosshair ---
    {
        ImU32 axis_col = DrUI::Colors::CanvasDotMajor;
        if (ox > p0.x && ox < p1.x)
            dl->AddLine(ImVec2(ox, p0.y), ImVec2(ox, p1.y), axis_col, 1.0f);
        if (oy > p0.y && oy < p1.y)
            dl->AddLine(ImVec2(p0.x, oy), ImVec2(p1.x, oy), axis_col, 1.0f);
    }

    // --- Brushed aluminum panels ---
    {
        // Big panel (base layer, centered)
        constexpr float kBigW = 1200.0f;
        constexpr float kBigH = 800.0f;
        ImVec2 btl = w2s(-kBigW * 0.5f, -kBigH * 0.5f);
        DrawBrushedAluminumPanel(dl, btl, kBigW * zm, kBigH * zm, 16.0f * zm, zm);

        // Aluminum buttons on big panel
        constexpr float kBtnW = 120.0f;
        constexpr float kBtnH = 90.0f;
        constexpr float kBtnGap = 24.0f;
        constexpr float kBtnRnd = 10.0f;

        // Base hue: (75,90,120) = steel blue
        // Variations: tweak lightness & saturation while keeping the hue
        struct AlumBtn { const char* icon; const char* label; ImU32 col; };
        AlumBtn btns[] = {
            { ICON_FA_PLAY,   "\xd0\x97\xd0\xb0\xd0\xbf\xd1\x83\xd1\x81\xd0\xba",
              IM_COL32(55, 68, 95, 255) },     // darker, more saturated
            { ICON_FA_PAUSE,  "\xd0\x9f\xd0\xb0\xd1\x83\xd0\xb7\xd0\xb0",
              IM_COL32(75, 90, 120, 255) },    // base
            { ICON_FA_STOP,   "\xd0\xa1\xd1\x82\xd0\xbe\xd0\xbf",
              IM_COL32(95, 112, 148, 255) },   // lighter, more saturated
            { ICON_FA_SAVE,   "\xd0\xa1\xd0\xbe\xd1\x85\xd1\x80.",
              IM_COL32(65, 78, 100, 255) },    // slightly darker
            { ICON_FA_COG,    "\xd0\x9d\xd0\xb0\xd1\x81\xd1\x82\xd1\x80.",
              IM_COL32(85, 98, 118, 255) },    // lighter, desaturated
            { ICON_FA_SEARCH, "\xd0\x9f\xd0\xbe\xd0\xb8\xd1\x81\xd0\xba",
              IM_COL32(110, 128, 165, 255) },  // lightest, most saturated
        };
        int nBtns = 6;
        float totalBtnW = nBtns * kBtnW + (nBtns - 1) * kBtnGap;
        float startX = -totalBtnW * 0.5f;
        float btnY = -kBtnH * 0.5f;

        for (int i = 0; i < nBtns; ++i) {
            float bx = startX + (float)i * (kBtnW + kBtnGap);
            ImVec2 btl2 = w2s(bx, btnY);
            DrawAluminumButton(dl, btl2, kBtnW * zm, kBtnH * zm,
                               kBtnRnd * zm, zm, btns[i].icon, btns[i].label,
                               btns[i].col);
        }

        // Aqua-style buttons (second row, below steel buttons)
        constexpr float kAquaW = 180.0f;
        constexpr float kAquaH = 55.0f;
        constexpr float kAquaGap = 30.0f;
        float aquaRow = btnY + kBtnH + 40.0f;
        float aquaTotalW = kAquaW * 2.0f + kAquaGap;
        float aquaStartX = -aquaTotalW * 0.5f;

        // Warm (peach)
        ImVec2 aq1 = w2s(aquaStartX, aquaRow);
        DrawAquaButton(dl, aq1, kAquaW * zm, kAquaH * zm, zm,
                       IM_COL32(231, 178, 161, 255),
                       IM_COL32(247, 243, 224, 255),
                       IM_COL32(235, 200, 180, 255),
                       IM_COL32(160, 110, 80, 255),
                       "Aqua Warm");

        // Cool (lavender)
        ImVec2 aq2 = w2s(aquaStartX + kAquaW + kAquaGap, aquaRow);
        DrawAquaButton(dl, aq2, kAquaW * zm, kAquaH * zm, zm,
                       IM_COL32(184, 174, 236, 255),
                       IM_COL32(221, 238, 251, 255),
                       IM_COL32(200, 195, 240, 255),
                       IM_COL32(80, 70, 150, 255),
                       "Aqua Cool");
    }

    // --- Widget preview background (in world space, clipped) ---
    if (g_selected_widget != WidgetType::None) {
        ImVec2 ptl = w2s(-kPrevWorldW * 0.5f, -kPrevWorldH * 0.5f);
        ImVec2 pbr = w2s( kPrevWorldW * 0.5f,  kPrevWorldH * 0.5f);
        float rnd = 8.0f * zm;
        dl->AddRectFilled(ptl, pbr,
                          ImGui::GetColorU32(ImGuiCol_WindowBg), rnd);
        dl->AddRect(ptl, pbr,
                    ImGui::GetColorU32(ImGuiCol_Border), rnd);
    }

    // --- Vignette ---
    {
        const float vig = 60.0f;
        const ImVec4& cb = DrUI::Colors::CanvasBg;
        ImU32 dark = IM_COL32((int)(cb.x*255), (int)(cb.y*255), (int)(cb.z*255), 180);
        ImU32 vnone = IM_COL32((int)(cb.x*255), (int)(cb.y*255), (int)(cb.z*255), 0);
        dl->AddRectFilledMultiColor(p0, ImVec2(p0.x + vig, p1.y),
                                    dark, vnone, vnone, dark);
        dl->AddRectFilledMultiColor(ImVec2(p1.x - vig, p0.y), p1,
                                    vnone, dark, dark, vnone);
        dl->AddRectFilledMultiColor(p0, ImVec2(p1.x, p0.y + vig),
                                    dark, dark, vnone, vnone);
        dl->AddRectFilledMultiColor(ImVec2(p0.x, p1.y - vig), p1,
                                    vnone, vnone, dark, dark);
    }

    dl->PopClipRect();

    // --- Widget preview content (child window, scaled by zoom) ---
    if (g_selected_widget != WidgetType::None) {
        ImVec2 ctl = w2s(-kPrevWorldW * 0.5f + kPrevWorldPad,
                         -kPrevWorldH * 0.5f + kPrevWorldPad);
        ImVec2 cbr = w2s( kPrevWorldW * 0.5f - kPrevWorldPad,
                          kPrevWorldH * 0.5f - kPrevWorldPad);
        float cw = cbr.x - ctl.x;
        float ch = cbr.y - ctl.y;

        if (cw > 1.0f && ch > 1.0f) {
            const ImGuiStyle& style = ImGui::GetStyle();
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                ImVec2(style.FramePadding.x * zm, style.FramePadding.y * zm));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                ImVec2(style.ItemSpacing.x * zm, style.ItemSpacing.y * zm));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing,
                ImVec2(style.ItemInnerSpacing.x * zm,
                       style.ItemInnerSpacing.y * zm));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding,
                style.FrameRounding * zm);

            ImGui::SetCursorScreenPos(ctl);
            ImGuiWindowFlags child_flags = ImGuiWindowFlags_NoScrollbar;
            if (!io.KeyCtrl)
                child_flags |= ImGuiWindowFlags_NoScrollWithMouse;
            ImGui::BeginChild("##preview_content", ImVec2(cw, ch),
                              false, child_flags);
            ImGui::PushFont(nullptr, ImGui::GetStyle().FontSizeBase * zm);

            DrawWidgetPreview(g_selected_widget, dpi_scale * zm);

            ImGui::PopFont();
            ImGui::EndChild();
            ImGui::PopStyleVar(5);
        }
    }

    // --- Zoom indicator (bottom-right corner) ---
    {
        char zbuf[32];
        std::snprintf(zbuf, sizeof(zbuf), "%.0f%%", g_canvas.zoom * 100.0f);
        ImVec2 tsz = ImGui::CalcTextSize(zbuf);
        dl->AddText(ImVec2(p1.x - tsz.x - 8.0f, p1.y - tsz.y - 6.0f),
                    DrUI::Colors::CanvasLabel, zbuf);
    }

    ImGui::End();
}

// ===================================================================
// Status bar
// ===================================================================

static void DrawStatusBar(float dpi_scale) {
    ImGuiViewport* vp = ImGui::GetMainViewport();
    const float h = 24.0f * dpi_scale;

    ImGui::SetNextWindowPos(
        ImVec2(vp->WorkPos.x, vp->WorkPos.y + vp->WorkSize.y - h));
    ImGui::SetNextWindowSize(ImVec2(vp->WorkSize.x, h));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 3));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, DrUI::Colors::BackgroundPrimary);
    ImGui::Begin("##StatusBar", nullptr,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar |
                 ImGuiWindowFlags_NoBringToFrontOnFocus);

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImGuiIO& io = ImGui::GetIO();

    ImVec2 cpos = ImGui::GetCursorScreenPos();
    float dot_r = 4.0f * dpi_scale;
    float dot_cx = cpos.x + dot_r;
    float dot_cy = cpos.y + (h - 6) * 0.5f;
    dl->AddCircleFilled(ImVec2(dot_cx, dot_cy), dot_r,
                        ImGui::ColorConvertFloat4ToU32(DrUI::Colors::Success));
    ImGui::Dummy(ImVec2(dot_r * 2 + 6, 1));
    ImGui::SameLine();

    ImGui::TextColored(DrUI::Colors::TextSecondary,
                       ICON_FA_BOLT " UI Sandbox");

    ImGui::SameLine(0, 8);
    ImGui::TextColored(DrUI::Colors::TextDisabled, "|");
    ImGui::SameLine(0, 8);
    ImGui::TextColored(DrUI::Colors::TextSecondary,
                       ICON_FA_COG " ImGui %s", ImGui::GetVersion());

    ImGui::SameLine(0, 8);
    ImGui::TextColored(DrUI::Colors::TextDisabled, "|");
    ImGui::SameLine(0, 8);

    ImVec4 fps_col = (io.Framerate >= 55.0f)
        ? DrUI::Colors::Success
        : DrUI::Colors::Warning;
    ImGui::TextColored(fps_col, "%.0f FPS", io.Framerate);

    auto& toasts = DrUI::GetToasts();
    if (!toasts.empty()) {
        float rw = ImGui::CalcTextSize(ICON_FA_BOLT).x + 40.0f;
        ImGui::SameLine(ImGui::GetWindowWidth() - rw);
        ImGui::TextColored(DrUI::Colors::AccentPrimary,
                           ICON_FA_BOLT " %d", (int)toasts.size());
    }

    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
}
