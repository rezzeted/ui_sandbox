#pragma once

#include "editor_layout.h"
#include <drui/drui.h>

#include <imgui.h>
#include <cstdio>
#include <cmath>
#include <string>
#include <vector>

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
        ImGui::MenuItem(ICON_FA_FOLDER " \xd0\x9e\xd0\xb1\xd0\xbe\xd0\xb7\xd1\x80\xd0\xb5\xd0\xb2\xd0\xb0\xd1\x82\xd0\xb5\xd0\xbb\xd1\x8c", nullptr, &panels.tab_explorer);
        ImGui::MenuItem(ICON_FA_COG " \xd0\x92\xd0\xb8\xd0\xb4\xd0\xb6\xd0\xb5\xd1\x82\xd1\x8b", nullptr, &panels.tab_widgets);
        ImGui::Separator();
        ImGui::MenuItem(ICON_FA_PLAY " \xd0\xa3\xd0\xbf\xd1\x80\xd0\xb0\xd0\xb2\xd0\xbb\xd0\xb5\xd0\xbd\xd0\xb8\xd0\xb5", nullptr, &panels.tab_controls);
        ImGui::MenuItem(ICON_FA_BARS " \xd0\x9b\xd0\xbe\xd0\xb3", nullptr, &panels.tab_log);
        ImGui::MenuItem(ICON_FA_SITEMAP " \xd0\xa1\xd0\xb2\xd0\xbe\xd0\xb9\xd1\x81\xd1\x82\xd0\xb2\xd0\xb0", nullptr, &panels.tab_properties);
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
// Left panel — tabs: Explorer + Widgets showcase
// ===================================================================

static void DrawExplorerTab() {
    static int selected = -1;
    static char search_buf[128] = "";

    struct TreeNode { const char* icon; const char* name; int depth; };
    static const TreeNode tree[] = {
        { ICON_FA_FOLDER,    "src/",             0 },
        { ICON_FA_FOLDER,    "apps/",            1 },
        { ICON_FA_FILE_CODE, "main.cpp",         2 },
        { ICON_FA_FILE_CODE, "sandbox_theme.h",  2 },
        { ICON_FA_FILE_CODE, "ui_helpers.h",     2 },
        { ICON_FA_FOLDER,    "libs/",            1 },
        { ICON_FA_FOLDER,    "thirdparty/",      0 },
        { ICON_FA_FOLDER,    "imgui/",           1 },
        { ICON_FA_FOLDER,    "glfw/",            1 },
        { ICON_FA_FILE,      "CMakeLists.txt",   0 },
        { ICON_FA_FILE,      "APPROACH.md",      0 },
    };
    constexpr int tree_count = sizeof(tree) / sizeof(tree[0]);

    DrUI::SearchInput("##explorer_search", search_buf, sizeof(search_buf));
    ImGui::Spacing();

    ImGui::BeginChild("##file_list", ImVec2(0, 0), false);

    const float indent_w = 16.0f;
    float line_h = ImGui::GetTextLineHeightWithSpacing();

    for (int i = 0; i < tree_count; ++i) {
        const auto& n = tree[i];

        if (search_buf[0] != '\0') {
            bool match = false;
            for (const char *a = n.name, *b = search_buf; *b; ) {
                if (*a == '\0') break;
                if ((*a | 32) == (*b | 32)) { ++a; ++b; if (!*b) match = true; }
                else { ++a; b = search_buf; }
            }
            if (!match) continue;
        }

        ImVec2 cpos = ImGui::GetCursorScreenPos();
        ImDrawList* dl = ImGui::GetWindowDrawList();
        for (int d = 0; d < n.depth; ++d) {
            float lx = cpos.x + d * indent_w + indent_w * 0.5f;
            dl->AddLine(ImVec2(lx, cpos.y),
                        ImVec2(lx, cpos.y + line_h),
                        ImGui::GetColorU32(ImGuiCol_Text, 0.08f));
        }

        if (n.depth > 0)
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + n.depth * indent_w);

        char label[128];
        std::snprintf(label, sizeof(label), "%s %s", n.icon, n.name);
        if (ImGui::Selectable(label, selected == i))
            selected = i;
    }

    ImGui::EndChild();
}

static void DrawWidgetsTab(float dpi_scale) {
    ImGui::TextDisabled(ICON_FA_COG " \xd0\x92\xd0\xb8\xd1\x82\xd1\x80\xd0\xb8\xd0\xbd\xd0\xb0 \xd0\xb2\xd0\xb8\xd0\xb4\xd0\xb6\xd0\xb5\xd1\x82\xd0\xbe\xd0\xb2");
    DrUI::GradientSeparator();

    ImGui::BeginChild("##widgets_scroll", ImVec2(0, 0), false);

    DrUI::CardBegin(ICON_FA_BOLT " \xd0\x9a\xd0\xbd\xd0\xbe\xd0\xbf\xd0\xba\xd0\xb8 \xd0\xb8 \xd0\xb2\xd0\xb2\xd0\xbe\xd0\xb4", false);
    {
        if (DrUI::Button(ICON_FA_PLUS " \xd0\x94\xd0\xbe\xd0\xb1\xd0\xb0\xd0\xb2\xd0\xb8\xd1\x82\xd1\x8c"))  {}
        ImGui::SameLine();
        if (DrUI::Button(ICON_FA_TRASH " \xd0\xa3\xd0\xb4\xd0\xb0\xd0\xbb\xd0\xb8\xd1\x82\xd1\x8c"))  {}
        ImGui::SameLine();
        if (DrUI::Button(ICON_FA_SAVE " \xd0\xa1\xd0\xbe\xd1\x85\xd1\x80\xd0\xb0\xd0\xbd\xd0\xb8\xd1\x82\xd1\x8c"))  {}

        ImGui::Spacing();
        if (DrUI::Button(ICON_FA_PLAY, "\xd0\x97\xd0\xb0\xd0\xbf\xd1\x83\xd1\x81\xd0\xba"))  {}
        ImGui::SameLine();
        if (DrUI::Button(ICON_FA_STOP, "\xd0\xa1\xd1\x82\xd0\xbe\xd0\xbf"))  {}

        ImGui::Spacing();
        if (DrUI::IconButton(ICON_FA_UNDO))  {}
        ImGui::SameLine();
        if (DrUI::IconButton(ICON_FA_REDO))  {}
        DrUI::Badge(3);
        ImGui::SameLine();
        if (DrUI::IconButton(ICON_FA_SEARCH))  {}
        ImGui::SameLine();
        if (DrUI::IconButton(ICON_FA_COG))  {}
        DrUI::Badge(12);

        ImGui::Spacing();
        static bool tog_grid = true, tog_snap = false, tog_eye = true;
        DrUI::IconToggle(ICON_FA_DICE, &tog_grid);
        ImGui::SameLine();
        DrUI::IconToggle(ICON_FA_ROUTE, &tog_snap);
        ImGui::SameLine();
        DrUI::IconToggle(ICON_FA_EYE, &tog_eye);

        ImGui::Spacing();
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

        DrUI::GradientSeparator();

        static bool dark_mode = true, autosave = false, notif = true;
        DrUI::ToggleSwitch("dark_mode", &dark_mode);
        ImGui::SameLine();
        ImGui::Text(ICON_FA_EYE " \xd0\xa2\xd1\x91\xd0\xbc\xd0\xbd\xd0\xb0\xd1\x8f \xd1\x82\xd0\xb5\xd0\xbc\xd0\xb0");
        DrUI::ToggleSwitch("autosave", &autosave);
        ImGui::SameLine();
        ImGui::Text(ICON_FA_SAVE " \xd0\x90\xd0\xb2\xd1\x82\xd0\xbe\xd1\x81\xd0\xbe\xd1\x85\xd1\x80\xd0\xb0\xd0\xbd\xd0\xb5\xd0\xbd\xd0\xb8\xd0\xb5");
        DrUI::ToggleSwitch("notif", &notif);
        ImGui::SameLine();
        ImGui::Text(ICON_FA_BOLT " \xd0\xa3\xd0\xb2\xd0\xb5\xd0\xb4\xd0\xbe\xd0\xbc\xd0\xbb\xd0\xb5\xd0\xbd\xd0\xb8\xd1\x8f");

        DrUI::GradientSeparator();

        static char text_buf[256] = "Hello, UI Sandbox!";
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##text", text_buf, sizeof(text_buf));

        static char multi_buf[1024] =
            "\xd0\x9c\xd0\xbd\xd0\xbe\xd0\xb3\xd0\xbe\xd1\x81\xd1\x82\xd1\x80"
            "\xd0\xbe\xd1\x87\xd0\xbd\xd1\x8b\xd0\xb9\n\xd1\x82\xd0\xb5\xd0\xba"
            "\xd1\x81\xd1\x82";
        ImGui::InputTextMultiline("##multi", multi_buf, sizeof(multi_buf),
                                  ImVec2(-1, 60.0f * dpi_scale));

        static int int_val = 42;
        DrUI::InputInt("Int", &int_val, 1, 10, 80.0f * dpi_scale);

        static float float_val = 3.14f;
        DrUI::InputFloat("Float", &float_val, 0.1f, 1.0f, "%.2f",
                         80.0f * dpi_scale);

        DrUI::GradientSeparator();

        static std::vector<std::string> demo_tags = {"ui_sandbox", "imgui", "C++20"};
        static char tag_buf[64] = "";
        ImGui::TextDisabled(ICON_FA_BOLT " Tag Input");
        DrUI::TagInput("##tags", demo_tags, tag_buf, sizeof(tag_buf));
    }
    DrUI::CardEnd();

    DrUI::CardBegin(ICON_FA_EYE " \xd0\xa1\xd0\xbb\xd0\xb0\xd0\xb9\xd0\xb4\xd0\xb5\xd1\x80\xd1\x8b \xd0\xb8 \xd0\xb2\xd1\x8b\xd0\xb1\xd0\xbe\xd1\x80", false);
    {
        static float slider1 = 0.5f;
        DrUI::SliderFloat("\xd0\x97\xd0\xbd\xd0\xb0\xd1\x87\xd0\xb5\xd0\xbd\xd0\xb8\xd0\xb5",
                          &slider1, 0.0f, 1.0f);

        static int slider_int = 50;
        DrUI::SliderInt("\xd0\xa8\xd0\xb0\xd0\xb3\xd0\xb8",
                        &slider_int, 0, 100);

        static float progress = 0.65f;
        DrUI::ProgressBar(progress, ImVec2(-1, 0), "65%");

        ImGui::Spacing();
        DrUI::Spinner("##spin1", 8.0f, 2.5f);
        ImGui::SameLine();
        ImGui::TextDisabled("\xd0\x97\xd0\xb0\xd0\xb3\xd1\x80\xd1\x83\xd0\xb7\xd0\xba\xd0\xb0...");

        DrUI::GradientSeparator();

        static bool chk1 = true, chk2 = false, chk3 = true;
        ImGui::Checkbox("\xd0\x9e\xd0\xbf\xd1\x86\xd0\xb8\xd1\x8f A", &chk1);
        ImGui::Checkbox("\xd0\x9e\xd0\xbf\xd1\x86\xd0\xb8\xd1\x8f B", &chk2);
        ImGui::Checkbox("\xd0\x9e\xd0\xbf\xd1\x86\xd0\xb8\xd1\x8f C", &chk3);

        static int radio = 0;
        ImGui::RadioButton("\xd0\xa0\xd0\xb5\xd0\xb6\xd0\xb8\xd0\xbc 1", &radio, 0);
        ImGui::SameLine();
        ImGui::RadioButton("\xd0\xa0\xd0\xb5\xd0\xb6\xd0\xb8\xd0\xbc 2", &radio, 1);
        ImGui::SameLine();
        ImGui::RadioButton("\xd0\xa0\xd0\xb5\xd0\xb6\xd0\xb8\xd0\xbc 3", &radio, 2);

        static int combo_sel = 0;
        const char* combo_items[] = {"Sequence", "Silhouette", "Custom"};
        ImGui::SetNextItemWidth(150.0f * dpi_scale);
        ImGui::Combo("\xd0\xa2\xd0\xb8\xd0\xbf", &combo_sel, combo_items, 3);

        DrUI::GradientSeparator();

        static ImVec4 col1(0.37f, 0.55f, 0.95f, 1.0f);
        static ImVec4 col2(0.63f, 0.45f, 0.98f, 1.0f);
        ImGui::ColorEdit4("Accent 1", &col1.x);
        ImGui::ColorEdit4("Accent 2", &col2.x);
    }
    DrUI::CardEnd();

    DrUI::CardBegin(ICON_FA_BOLT " Shimmer Text", false);
    {
        DrUI::ShimmerText("Hello, Shimmer World!");
        ImGui::Spacing();
        DrUI::ShimmerText("\xd0\x9f\xd0\xb5\xd1\x80\xd0\xb5\xd0\xbb\xd0\xb8\xd0\xb2\xd0\xb0\xd1\x8e\xd1\x89\xd0\xb8\xd0\xb9\xd1\x81\xd1\x8f \xd1\x82\xd0\xb5\xd0\xba\xd1\x81\xd1\x82 \xd0\xbd\xd0\xb0 \xd1\x80\xd1\x83\xd1\x81\xd1\x81\xd0\xba\xd0\xbe\xd0\xbc");
        ImGui::Spacing();
        DrUI::ShimmerText(ICON_FA_COG " AI is thinking...", 1.2f, 0.25f);
        ImGui::Spacing();
        DrUI::ShimmerText("Slow shimmer demo", 4.0f, 0.5f);
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
            DrUI::GlowGradientBorder(wdl, rp, ImVec2(avail_w - pad * 2.0f, rh),
                                     border_colors, 4, t, 0.2f, 2.0f, 10.0f);
        }
    }
    DrUI::CardEnd();

    ImGui::EndChild();
}

static bool g_card_active = false;

static void DrawSkeuomorphTab(float dpi_scale) {
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 cursor = ImGui::GetCursorScreenPos();
    ImVec2 avail = ImGui::GetContentRegionAvail();

    ImVec2 p0 = cursor;
    ImVec2 p1(cursor.x + avail.x, cursor.y + avail.y);

    ImU32 fill   = IM_COL32(238, 234, 226, 245);
    ImU32 border = IM_COL32(230, 225, 215, 180);
    float bg_rnd = 8.0f * dpi_scale;
    dl->AddRectFilled(p0, p1, fill, bg_rnd);
    dl->AddRect(p0, p1, border, bg_rnd, 0, 1.0f);

    ImGui::InvisibleButton("##skeuo_area", avail);
    bool area_hovered = ImGui::IsItemHovered();

    constexpr float PI = 3.14159265f;
    const float zm = 1.0f;
    float card_sz = 220.0f;
    float rnd = 24.0f;
    float pad = 22.0f;

    float cx = p0.x + (avail.x - card_sz) * 0.5f;
    float cy = p0.y + (avail.y - card_sz) * 0.5f;
    ImVec2 cp0(cx, cy);
    ImVec2 cp1(cx + card_sz, cy + card_sz);

    if (area_hovered && ImGui::IsMouseClicked(0)) {
        ImVec2 mp = io.MousePos;
        if (mp.x >= cp0.x && mp.x <= cp1.x &&
            mp.y >= cp0.y && mp.y <= cp1.y)
            g_card_active = !g_card_active;
    }

    if (g_card_active) {
        ImU32 blue_dark = IM_COL32(40, 90, 200, 255);
        dl->AddRectFilled(cp0, cp1, blue_dark, rnd);

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
            dl->AddRectFilled(tp0, tp1, IM_COL32(35, 85, 190, 200), trnd);
            dl->AddRect(tp0, tp1, IM_COL32(100, 170, 255, 80), trnd, 0, 1.0f * zm);
            float kr = (th * 0.5f) - 3.0f * zm;
            dl->AddCircleFilled(ImVec2(tp1.x - th * 0.5f, tp0.y + th * 0.5f), kr, wh);
        }
    } else {
        ImU32 card_fill = IM_COL32(242, 238, 230, 255);

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

        ImU32 dk = IM_COL32(80, 75, 68, 255);
        ImU32 dk70 = IM_COL32(80, 75, 68, 140);

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
            dl->AddRectFilled(tp0, tp1, IM_COL32(200, 195, 185, 200), trnd);
            dl->AddRect(tp0, tp1, IM_COL32(180, 175, 165, 100), trnd, 0, 1.0f * zm);
            float kr = (th * 0.5f) - 3.0f * zm;
            dl->AddCircleFilled(ImVec2(tp0.x + th * 0.5f, tp0.y + th * 0.5f), kr,
                                IM_COL32(255, 255, 255, 255));
        }
    }
}

static void DrawLeftPanel(const PanelLayout& zone, float dpi_scale,
                          PanelVisibility& panels) {
    ImGui::SetNextWindowPos(zone.pos);
    ImGui::SetNextWindowSize(zone.size);
    ImGui::Begin("##LeftPanel", nullptr, kPanelFlags);

    if (ImGui::BeginTabBar("##LeftTabs", ImGuiTabBarFlags_DrawSelectedOverline)) {
        if (panels.tab_explorer && ImGui::BeginTabItem(ICON_FA_FOLDER " \xd0\x9e\xd0\xb1\xd0\xb7\xd0\xbe\xd1\x80", &panels.tab_explorer)) {
            DrawExplorerTab();
            ImGui::EndTabItem();
        }
        if (panels.tab_widgets && ImGui::BeginTabItem(ICON_FA_COG " \xd0\x92\xd0\xb8\xd0\xb4\xd0\xb6\xd0\xb5\xd1\x82\xd1\x8b", &panels.tab_widgets)) {
            DrawWidgetsTab(dpi_scale);
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
// Canvas panel (center) — pannable/zoomable workspace
// ===================================================================

struct CanvasState {
    ImVec2 pan        = ImVec2(0.0f, 0.0f);
    float  zoom       = 1.0f;
    bool   initialized = false;
};

static CanvasState g_canvas;

static void DrawCanvasPanel(const PanelLayout& zone) {
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

    ImGuiIO& io = ImGui::GetIO();

    // Pan: middle-mouse or right-mouse drag
    if (canvas_hovered && (ImGui::IsMouseDragging(ImGuiMouseButton_Middle, 0.0f) ||
                           ImGui::IsMouseDragging(ImGuiMouseButton_Right, 0.0f))) {
        ImVec2 delta = ImGui::IsMouseDragging(ImGuiMouseButton_Middle, 0.0f)
                         ? io.MouseDelta : io.MouseDelta;
        g_canvas.pan.x += delta.x;
        g_canvas.pan.y += delta.y;
    }

    // Zoom: mouse wheel, zoom towards cursor
    if (canvas_hovered && io.MouseWheel != 0.0f) {
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
// Bottom panel — tabs: Controls + Log + Properties
// ===================================================================

static void DrawControlsTab(float dpi_scale) {
    static bool playing = false;
    static int speed = 1;
    static float ms_per_step = 100.0f;

    if (DrUI::Button(playing ? ICON_FA_PAUSE : ICON_FA_PLAY,
                     playing ? "\xd0\x9f\xd0\xb0\xd1\x83\xd0\xb7\xd0\xb0"
                             : "\xd0\x97\xd0\xb0\xd0\xbf\xd1\x83\xd1\x81\xd0\xba")) {
        playing = !playing;
    }
    ImGui::SameLine();
    if (DrUI::IconButton(ICON_FA_STEP_FORWARD)) {}
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("\xd0\xa8\xd0\xb0\xd0\xb3");
    ImGui::SameLine();
    if (DrUI::Button(ICON_FA_STOP, "\xd0\xa1\xd0\xb1\xd1\x80\xd0\xbe\xd1\x81")) {
        playing = false;
    }

    if (playing) {
        ImGui::SameLine();
        DrUI::Spinner("##ctrl_spin", 8.0f, 2.5f);
    }

    ImGui::SameLine();
    DrUI::InputInt("Steps/frame", &speed, 1, 10, 50.0f * dpi_scale);
    speed = std::clamp(speed, 1, 200);

    ImGui::SameLine();
    ImGui::SetNextItemWidth(120.0f * dpi_scale);
    DrUI::SliderFloat("\xd0\xbc\xd1\x81/\xd1\x88\xd0\xb0\xd0\xb3",
                      &ms_per_step, 20.0f, 900.0f, "%.0f");
}

static void DrawLogTab() {
    static std::vector<std::string> log_lines;
    static int counter = 0;

    if (DrUI::Button(ICON_FA_PLUS " \xd0\x94\xd0\xbe\xd0\xb1\xd0\xb0\xd0\xb2\xd0\xb8\xd1\x82\xd1\x8c \xd0\xb7\xd0\xb0\xd0\xbf\xd0\xb8\xd1\x81\xd1\x8c")) {
        char buf[128];
        std::snprintf(buf, sizeof(buf),
                      "[%04d] \xd0\xa1\xd0\xbe\xd0\xb1\xd1\x8b\xd1\x82\xd0\xb8\xd0\xb5 #%d",
                      counter, counter);
        log_lines.emplace_back(buf);
        ++counter;
        static const DrUI::ToastType types[] = {
            DrUI::ToastType::Info, DrUI::ToastType::Success,
            DrUI::ToastType::Warning, DrUI::ToastType::Error
        };
        DrUI::ShowToast(buf, types[counter % 4]);
    }
    ImGui::SameLine();
    if (DrUI::Button(ICON_FA_TRASH " \xd0\x9e\xd1\x87\xd0\xb8\xd1\x81\xd1\x82\xd0\xb8\xd1\x82\xd1\x8c")) {
        log_lines.clear();
        counter = 0;
        DrUI::ShowToast("\xd0\x9b\xd0\xbe\xd0\xb3 \xd0\xbe\xd1\x87\xd0\xb8\xd1\x89\xd0\xb5\xd0\xbd",
                  DrUI::ToastType::Warning);
    }

    ImGui::BeginChild("##log_scroll", ImVec2(0, 0),
                      ImGuiChildFlags_Borders);
    for (const auto& line : log_lines) {
        ImGui::TextUnformatted(line.c_str());
    }
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);
    ImGui::EndChild();
}

static void DrawPropertiesTab(float dpi_scale) {
    static char name_buf[128] = "Node_01";
    static int node_type = 0;
    static float pos_x = 100.0f, pos_y = 200.0f;
    static float width = 150.0f, height = 60.0f;
    static ImVec4 node_color(0.37f, 0.55f, 0.95f, 1.0f);

    ImGui::TextDisabled(ICON_FA_SITEMAP " \xd0\xa1\xd0\xb2\xd0\xbe\xd0\xb9\xd1\x81\xd1\x82\xd0\xb2\xd0\xb0 \xd1\x83\xd0\xb7\xd0\xbb\xd0\xb0");
    ImGui::Separator();

    ImGui::SetNextItemWidth(150.0f * dpi_scale);
    ImGui::InputText("\xd0\x98\xd0\xbc\xd1\x8f", name_buf, sizeof(name_buf));

    const char* types[] = {"Action", "Question", "Fork", "End"};
    ImGui::SetNextItemWidth(150.0f * dpi_scale);
    ImGui::Combo("\xd0\xa2\xd0\xb8\xd0\xbf", &node_type, types, 4);

    ImGui::SetNextItemWidth(100.0f * dpi_scale);
    ImGui::DragFloat("X", &pos_x, 1.0f);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100.0f * dpi_scale);
    ImGui::DragFloat("Y", &pos_y, 1.0f);

    ImGui::SetNextItemWidth(100.0f * dpi_scale);
    ImGui::DragFloat("W", &width, 1.0f, 10.0f, 500.0f);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100.0f * dpi_scale);
    ImGui::DragFloat("H", &height, 1.0f, 10.0f, 500.0f);

    ImGui::ColorEdit4("\xd0\xa6\xd0\xb2\xd0\xb5\xd1\x82", &node_color.x);

    ImGui::Spacing();
    if (DrUI::Button(ICON_FA_CHECK " \xd0\x9f\xd1\x80\xd0\xb8\xd0\xbc\xd0\xb5\xd0\xbd\xd0\xb8\xd1\x82\xd1\x8c")) {}
    ImGui::SameLine();
    if (DrUI::Button(ICON_FA_UNDO " \xd0\xa1\xd0\xb1\xd1\x80\xd0\xbe\xd1\x81")) {}
}

static void DrawBottomPanel(const PanelLayout& zone, float dpi_scale,
                             PanelVisibility& panels) {
    ImGui::SetNextWindowPos(zone.pos);
    ImGui::SetNextWindowSize(zone.size);
    if (panels.bottom_collapsed)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 0));
    ImGui::Begin("##BottomPanel", nullptr, kPanelFlags);
    if (panels.bottom_collapsed)
        ImGui::PopStyleVar();

    static int active_tab = 0;
    int current_tab = active_tab;

    if (ImGui::BeginTabBar("##BottomTabs", ImGuiTabBarFlags_DrawSelectedOverline)) {
        if (panels.tab_controls && ImGui::BeginTabItem(ICON_FA_PLAY " \xd0\xa3\xd0\xbf\xd1\x80\xd0\xb0\xd0\xb2\xd0\xbb\xd0\xb5\xd0\xbd\xd0\xb8\xd0\xb5", &panels.tab_controls)) {
            current_tab = 0;
            if (ImGui::IsItemClicked() && active_tab == 0)
                panels.bottom_collapsed = !panels.bottom_collapsed;
            if (!panels.bottom_collapsed) DrawControlsTab(dpi_scale);
            ImGui::EndTabItem();
        }
        if (panels.tab_log && ImGui::BeginTabItem(ICON_FA_BARS " \xd0\x9b\xd0\xbe\xd0\xb3", &panels.tab_log)) {
            current_tab = 1;
            if (ImGui::IsItemClicked() && active_tab == 1)
                panels.bottom_collapsed = !panels.bottom_collapsed;
            if (!panels.bottom_collapsed) DrawLogTab();
            ImGui::EndTabItem();
        }
        if (panels.tab_properties && ImGui::BeginTabItem(ICON_FA_SITEMAP " \xd0\xa1\xd0\xb2\xd0\xbe\xd0\xb9\xd1\x81\xd1\x82\xd0\xb2\xd0\xb0", &panels.tab_properties)) {
            current_tab = 2;
            if (ImGui::IsItemClicked() && active_tab == 2)
                panels.bottom_collapsed = !panels.bottom_collapsed;
            if (!panels.bottom_collapsed) DrawPropertiesTab(dpi_scale);
            ImGui::EndTabItem();
        }
        if (current_tab != active_tab)
            panels.bottom_collapsed = false;
        active_tab = current_tab;
        ImGui::EndTabBar();
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
