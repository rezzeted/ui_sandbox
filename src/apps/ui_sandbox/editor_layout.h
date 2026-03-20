#pragma once

#include <drui/drui.h>

#include <imgui.h>
#include <algorithm>

struct PanelLayout {
    ImVec2 pos;
    ImVec2 size;
};

struct EditorLayout {
    PanelLayout left;
    PanelLayout canvas;
};

struct SplitterState {
    bool dragging = false;
};

struct EditorSplitters {
    float split_h = 0.286f;
    SplitterState h_state;
};

struct PanelVisibility {
    bool tab_widgets       = true;
    bool tab_custom        = true;
    bool tab_skeuomorph    = true;
    bool tab_aluminum      = true;
    bool status_bar        = true;
    float dpi_scale        = 1.0f;
};

static constexpr ImGuiWindowFlags kPanelFlags =
    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;

static EditorLayout CalculateLayout(ImGuiViewport* vp, float gap,
                                    float menu_h, float status_h,
                                    const EditorSplitters& sp) {
    const float x = vp->WorkPos.x;
    const float y = vp->WorkPos.y + menu_h;
    const float w = vp->WorkSize.x;
    const float h = vp->WorkSize.y - menu_h - status_h;

    const float usable_w = w - gap * 3.0f;
    const float left_w = usable_w * sp.split_h;
    const float right_col_w = usable_w - left_w;

    EditorLayout l{};

    l.left.pos  = ImVec2(x + gap, y + gap);
    l.left.size = ImVec2(left_w, h - gap * 2.0f);

    l.canvas.pos  = ImVec2(x + gap + left_w + gap, y + gap);
    l.canvas.size = ImVec2(right_col_w, h - gap * 2.0f);

    return l;
}

static bool HandleSplitterH(const EditorLayout& layout, float gap,
                             float usable_w, float origin_x,
                             SplitterState& state, float& split) {
    ImGuiIO& io = ImGui::GetIO();

    float x0 = layout.left.pos.x + layout.left.size.x;
    float x1 = layout.canvas.pos.x;
    float y0 = layout.left.pos.y;
    float y1 = layout.left.pos.y + layout.left.size.y;

    float extend = 4.0f;
    bool hovered = io.MousePos.x >= x0 - extend && io.MousePos.x <= x1 + extend &&
                   io.MousePos.y >= y0 && io.MousePos.y <= y1;

    if (hovered || state.dragging)
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

    if (hovered && io.MouseClicked[0])
        state.dragging = true;

    if (state.dragging) {
        if (io.MouseDown[0]) {
            float new_left = io.MousePos.x - origin_x - gap - gap * 0.5f;
            split = std::clamp(new_left / usable_w, 0.12f, 0.50f);
            return true;
        }
        state.dragging = false;
    }
    return false;
}

static void HandleSplitters(const EditorLayout& layout, float gap,
                             ImGuiViewport* vp, float menu_h, float status_h,
                             EditorSplitters& sp) {
    const float w = vp->WorkSize.x;
    const float usable_w = w - gap * 3.0f;
    const float origin_x = vp->WorkPos.x;

    HandleSplitterH(layout, gap, usable_w, origin_x, sp.h_state, sp.split_h);
}

static void DrawSplitterIndicators(const EditorLayout& layout, float gap,
                                   const EditorSplitters& sp) {
    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* dl = ImGui::GetForegroundDrawList();
    float extend = 4.0f;

    auto draw = [&](float x0, float y0, float x1, float y1, bool vert,
                    bool dragging) {
        bool hov = vert
            ? (io.MousePos.x >= x0 - extend && io.MousePos.x <= x1 + extend &&
               io.MousePos.y >= y0 && io.MousePos.y <= y1)
            : (io.MousePos.y >= y0 - extend && io.MousePos.y <= y1 + extend &&
               io.MousePos.x >= x0 && io.MousePos.x <= x1);
        if (!hov && !dragging) return;

        ImU32 line_col = dragging ? DrUI::Colors::Accent(200)
                                  : DrUI::Colors::Accent(100);
        ImU32 dot_col  = dragging ? DrUI::Colors::Accent(240)
                                  : DrUI::Colors::Accent(150);

        if (vert) {
            float cx = (x0 + x1) * 0.5f;
            dl->AddLine(ImVec2(cx, y0), ImVec2(cx, y1), line_col, 1.0f);
            float cy = (y0 + y1) * 0.5f;
            for (int i = -1; i <= 1; ++i)
                dl->AddCircleFilled(ImVec2(cx, cy + i * 8.0f), 2.5f, dot_col);
        } else {
            float cy = (y0 + y1) * 0.5f;
            dl->AddLine(ImVec2(x0, cy), ImVec2(x1, cy), line_col, 1.0f);
            float cx = (x0 + x1) * 0.5f;
            for (int i = -1; i <= 1; ++i)
                dl->AddCircleFilled(ImVec2(cx + i * 8.0f, cy), 2.5f, dot_col);
        }
    };

    draw(layout.left.pos.x + layout.left.size.x,
         layout.left.pos.y,
         layout.canvas.pos.x,
         layout.left.pos.y + layout.left.size.y,
         true, sp.h_state.dragging);
}
