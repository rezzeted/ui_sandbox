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
    PanelLayout bottom;
};

struct SplitterState {
    bool dragging = false;
};

struct EditorSplitters {
    float split_h = 0.22f;
    float split_v = 0.72f;
    SplitterState h_state;
    SplitterState v_state;
};

struct PanelVisibility {
    bool tab_explorer      = true;
    bool tab_widgets       = true;
    bool tab_controls      = true;
    bool tab_log           = true;
    bool tab_properties    = true;
    bool tab_skeuomorph    = true;
    bool status_bar        = true;
    bool bottom_collapsed  = false;
    float dpi_scale        = 1.0f;
};

static constexpr ImGuiWindowFlags kPanelFlags =
    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;

static EditorLayout CalculateLayout(ImGuiViewport* vp, float gap,
                                    float menu_h, float status_h,
                                    const EditorSplitters& sp,
                                    const PanelVisibility& pv = {}) {
    const float x = vp->WorkPos.x;
    const float y = vp->WorkPos.y + menu_h;
    const float w = vp->WorkSize.x;
    const float h = vp->WorkSize.y - menu_h - status_h;

    const float usable_w = w - gap * 3.0f;
    const float left_w = usable_w * sp.split_h;
    const float right_col_w = usable_w - left_w;

    const float usable_h = h - gap * 3.0f;
    float canvas_h, bottom_h;
    if (pv.bottom_collapsed) {
        const auto& style = ImGui::GetStyle();
        const float tab_bar_h = ImGui::GetFrameHeight() + style.WindowPadding.y * 2.0f;
        bottom_h = tab_bar_h;
        canvas_h = usable_h - bottom_h;
    } else {
        canvas_h = usable_h * sp.split_v;
        bottom_h = usable_h - canvas_h;
    }

    const float bottom_top = y + gap + canvas_h + gap;

    EditorLayout l{};

    l.left.pos  = ImVec2(x + gap, y + gap);
    l.left.size = ImVec2(left_w, h - gap * 2.0f);

    l.canvas.pos  = ImVec2(x + gap + left_w + gap, y + gap);
    l.canvas.size = ImVec2(right_col_w, canvas_h);

    l.bottom.pos  = ImVec2(x + gap + left_w + gap, bottom_top);
    l.bottom.size = ImVec2(right_col_w, bottom_h);

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

static bool HandleSplitterV(const EditorLayout& layout, float gap,
                             float usable_h, float origin_y,
                             SplitterState& state, float& split) {
    ImGuiIO& io = ImGui::GetIO();

    float y0 = layout.canvas.pos.y + layout.canvas.size.y;
    float y1 = layout.bottom.pos.y;
    float x0 = layout.canvas.pos.x;
    float x1 = layout.canvas.pos.x + layout.canvas.size.x;

    float extend = 4.0f;
    bool hovered = io.MousePos.y >= y0 - extend && io.MousePos.y <= y1 + extend &&
                   io.MousePos.x >= x0 && io.MousePos.x <= x1;

    if (hovered || state.dragging)
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);

    if (hovered && io.MouseClicked[0])
        state.dragging = true;

    if (state.dragging) {
        if (io.MouseDown[0]) {
            float new_canvas = io.MousePos.y - origin_y - gap - gap * 0.5f;
            split = std::clamp(new_canvas / usable_h, 0.30f, 0.90f);
            return true;
        }
        state.dragging = false;
    }
    return false;
}

static void HandleSplitters(const EditorLayout& layout, float gap,
                             ImGuiViewport* vp, float menu_h, float status_h,
                             EditorSplitters& sp,
                             bool bottom_collapsed = false) {
    const float w = vp->WorkSize.x;
    const float h = vp->WorkSize.y - menu_h - status_h;
    const float usable_w = w - gap * 3.0f;
    const float usable_h = h - gap * 3.0f;
    const float origin_x = vp->WorkPos.x;
    const float origin_y = vp->WorkPos.y + menu_h;

    HandleSplitterH(layout, gap, usable_w, origin_x, sp.h_state, sp.split_h);
    if (!bottom_collapsed)
        HandleSplitterV(layout, gap, usable_h, origin_y, sp.v_state, sp.split_v);
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

    draw(layout.canvas.pos.x,
         layout.canvas.pos.y + layout.canvas.size.y,
         layout.canvas.pos.x + layout.canvas.size.x,
         layout.bottom.pos.y,
         false, sp.v_state.dragging);
}
