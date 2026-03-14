#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include "sandbox_theme.h"
#include <algorithm>
#include <cmath>
#include <cstring>

namespace DrUI {

// ── Internal helpers ────────────────────────────────────────────────

namespace detail {

static constexpr float kButtonFontScale = 1.1f;

inline void PushButtonFont() {
    ImGui::PushFont(nullptr,
                    ImGui::GetStyle().FontSizeBase * kButtonFontScale);
}

inline ImU32 ButtonBg(bool held, bool hovered) {
    ImVec4 b = ImGui::GetStyleColorVec4(ImGuiCol_Button);
    if (held)
        return ImGui::ColorConvertFloat4ToU32(
            ImVec4(b.x * 0.75f, b.y * 0.75f, b.z * 0.75f, b.w));
    if (hovered)
        return ImGui::ColorConvertFloat4ToU32(
            ImVec4(std::min(b.x * 1.12f, 1.0f),
                   std::min(b.y * 1.12f, 1.0f),
                   std::min(b.z * 1.12f, 1.0f), b.w));
    return ImGui::GetColorU32(ImGuiCol_Button);
}

// Optical Y correction so the visual center of capital letters
// aligns with the geometric center of the button.
inline float TextCenterCorrection() {
    ImFontBaked* fb = ImGui::GetFontBaked();
    const ImFontGlyph* g = fb->FindGlyph((ImWchar)'A');
    if (!g) return 0.0f;
    return fb->Size * 0.5f - (g->Y0 + g->Y1) * 0.5f;
}

inline int DetectIconPrefix(const char* s) {
    if (!s) return 0;
    auto u = [](char c) { return (unsigned char)c; };
    if (u(s[0]) == 0xEF && (u(s[1]) & 0xC0) == 0x80 && (u(s[2]) & 0xC0) == 0x80)
        return 3;
    return 0;
}

inline ImWchar DecodeIconCodepoint(const char* s) {
    auto u = [](char c) -> unsigned { return (unsigned char)c; };
    return (ImWchar)(((u(s[0]) & 0x0F) << 12) | ((u(s[1]) & 0x3F) << 6) | (u(s[2]) & 0x3F));
}

inline ImVec2 CenterIconInRect(const char* icon, float cx, float cy, float w, float h) {
    ImFontBaked* fb = ImGui::GetFontBaked();
    ImWchar cp = DecodeIconCodepoint(icon);
    const ImFontGlyph* g = fb->FindGlyph(cp);
    if (!g) {
        ImVec2 sz = ImGui::CalcTextSize(icon);
        return ImVec2(cx + (w - sz.x) * 0.5f, cy + (h - sz.y) * 0.5f);
    }
    float vis_w = g->X1 - g->X0;
    float vis_h = g->Y1 - g->Y0;
    return ImVec2(cx + (w - vis_w) * 0.5f - g->X0,
                  cy + (h - vis_h) * 0.5f - g->Y0);
}

} // namespace detail

// ── Widgets ─────────────────────────────────────────────────────────

// General-purpose button. Auto-detects a leading FA icon in the label
// and renders icon / text at independently centered Y positions.
inline bool Button(const char* label, const ImVec2& size_arg = ImVec2(0, 0)) {
    detail::PushButtonFont();
    const ImGuiStyle& style = ImGui::GetStyle();
    ImVec2 label_sz = ImGui::CalcTextSize(label, nullptr, true);

    ImVec2 sz;
    sz.x = (size_arg.x > 0) ? size_arg.x : (label_sz.x + style.FramePadding.x * 2.0f);
    sz.y = (size_arg.y > 0) ? size_arg.y : (label_sz.y + style.FramePadding.y * 2.0f);

    ImVec2 cursor = ImGui::GetCursorScreenPos();

    ImGui::PushID(label);
    bool pressed = ImGui::InvisibleButton("##ui", sz);
    ImGui::PopID();

    bool hovered = ImGui::IsItemHovered();
    bool held    = ImGui::IsItemActive();

    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(cursor, ImVec2(cursor.x + sz.x, cursor.y + sz.y),
                      detail::ButtonBg(held, hovered), style.FrameRounding);

    ImU32  col      = ImGui::GetColorU32(ImGuiCol_Text);
    float  base_y   = cursor.y + (sz.y - label_sz.y) * 0.5f;
    float  lx       = cursor.x + (sz.x - label_sz.x) * 0.5f;
    ImFont* cur_font = ImGui::GetFont();
    float   cur_size = ImGui::GetFontSize();

    int icon_len = detail::DetectIconPrefix(label);
    if (icon_len > 0) {
        float text_corr = detail::TextCenterCorrection();

        ImWchar icon_cp = detail::DecodeIconCodepoint(label);
        const ImFontGlyph* ig = ImGui::GetFontBaked()->FindGlyph(icon_cp);
        float icon_y = base_y;
        if (ig)
            icon_y = cursor.y + sz.y * 0.5f - (ig->Y0 + ig->Y1) * 0.5f;
        dl->AddText(cur_font, cur_size, ImVec2(lx, icon_y), col, label, label + icon_len);

        const char* rest = label + icon_len;
        while (*rest == ' ') ++rest;
        if (*rest) {
            float icon_w  = ImGui::CalcTextSize(label, label + icon_len).x;
            float space_w = ImGui::CalcTextSize(" ").x;
            dl->AddText(cur_font, cur_size,
                        ImVec2(lx + icon_w + space_w, base_y + text_corr), col, rest);
        }
    } else {
        float text_corr = detail::TextCenterCorrection();
        dl->AddText(cur_font, cur_size, ImVec2(lx, base_y + text_corr), col, label);
    }

    ImGui::PopFont();
    return pressed;
}

// Icon + text button with explicit separate arguments.
inline bool Button(const char* icon, const char* text) {
    detail::PushButtonFont();
    ImVec2 icon_sz = ImGui::CalcTextSize(icon);
    ImVec2 text_sz = ImGui::CalcTextSize(text);
    const ImGuiStyle& style = ImGui::GetStyle();
    float gap = 6.0f;

    float content_w = icon_sz.x + gap + text_sz.x;
    float content_h = std::max(icon_sz.y, text_sz.y);
    ImVec2 btn_sz(content_w + style.FramePadding.x * 2.0f,
                  content_h + style.FramePadding.y * 2.0f);

    ImVec2 cursor = ImGui::GetCursorScreenPos();

    ImGui::PushID(text);
    bool pressed = ImGui::InvisibleButton("##btn", btn_sz);
    ImGui::PopID();

    bool hovered = ImGui::IsItemHovered();
    bool held    = ImGui::IsItemActive();

    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(cursor, ImVec2(cursor.x + btn_sz.x, cursor.y + btn_sz.y),
                      detail::ButtonBg(held, hovered), style.FrameRounding);

    ImFont* cur_font = ImGui::GetFont();
    float   cur_size = ImGui::GetFontSize();
    ImU32 text_col = ImGui::GetColorU32(ImGuiCol_Text);
    float text_y = cursor.y + (btn_sz.y - text_sz.y) * 0.5f + detail::TextCenterCorrection();

    ImWchar icon_cp = detail::DecodeIconCodepoint(icon);
    const ImFontGlyph* ig = ImGui::GetFontBaked()->FindGlyph(icon_cp);
    float icon_y = cursor.y + (btn_sz.y - icon_sz.y) * 0.5f;
    if (ig)
        icon_y = cursor.y + btn_sz.y * 0.5f - (ig->Y0 + ig->Y1) * 0.5f;

    dl->AddText(cur_font, cur_size,
                ImVec2(cursor.x + style.FramePadding.x, icon_y), text_col, icon);
    dl->AddText(cur_font, cur_size,
                ImVec2(cursor.x + style.FramePadding.x + icon_sz.x + gap, text_y),
                text_col, text);

    ImGui::PopFont();
    return pressed;
}

// Square icon-only button.
inline bool IconButton(const char* icon, float size = 0.0f) {
    detail::PushButtonFont();
    if (size <= 0.0f) size = ImGui::GetFrameHeight();
    ImVec2 cursor = ImGui::GetCursorScreenPos();

    ImGui::PushID(icon);
    bool pressed = ImGui::InvisibleButton("##ico", ImVec2(size, size));
    ImGui::PopID();

    bool hovered = ImGui::IsItemHovered();
    bool held    = ImGui::IsItemActive();

    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(cursor, ImVec2(cursor.x + size, cursor.y + size),
                      detail::ButtonBg(held, hovered), ImGui::GetStyle().FrameRounding);

    ImVec2 ipos = detail::CenterIconInRect(icon, cursor.x, cursor.y, size, size);
    dl->AddText(ImGui::GetFont(), ImGui::GetFontSize(),
                ipos, ImGui::GetColorU32(ImGuiCol_Text), icon);

    ImGui::PopFont();
    return pressed;
}

// InputInt with icon +/- stepper buttons.
inline bool InputInt(const char* label, int* v, int step = 1, int step_fast = 10,
                     float field_width = 0.0f) {
    bool changed = false;
    float btn_sz = ImGui::GetFrameHeight();

    ImGui::PushID(label);
    if (IconButton(ICON_FA_MINUS, btn_sz)) { *v -= step; changed = true; }
    ImGui::SameLine(0, 2);
    if (field_width > 0) ImGui::SetNextItemWidth(field_width);
    else                 ImGui::SetNextItemWidth(btn_sz * 3.0f);
    if (ImGui::InputInt("##v", v, 0, 0)) changed = true;
    ImGui::SameLine(0, 2);
    if (IconButton(ICON_FA_PLUS, btn_sz)) { *v += step; changed = true; }
    ImGui::PopID();

    ImGui::SameLine();
    ImGui::TextUnformatted(label);
    return changed;
}

// InputFloat with icon +/- stepper buttons.
inline bool InputFloat(const char* label, float* v, float step = 0.1f,
                       float step_fast = 1.0f, const char* fmt = "%.2f",
                       float field_width = 0.0f) {
    bool changed = false;
    float btn_sz = ImGui::GetFrameHeight();

    ImGui::PushID(label);
    if (IconButton(ICON_FA_MINUS, btn_sz)) { *v -= step; changed = true; }
    ImGui::SameLine(0, 2);
    if (field_width > 0) ImGui::SetNextItemWidth(field_width);
    else                 ImGui::SetNextItemWidth(btn_sz * 3.0f);
    if (ImGui::InputFloat("##v", v, 0, 0, fmt)) changed = true;
    ImGui::SameLine(0, 2);
    if (IconButton(ICON_FA_PLUS, btn_sz)) { *v += step; changed = true; }
    ImGui::PopID();

    ImGui::SameLine();
    ImGui::TextUnformatted(label);
    return changed;
}

// CollapsingHeader without frame border.
inline bool CollapsingHeader(const char* label, ImGuiTreeNodeFlags flags = 0) {
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
    bool open = ImGui::CollapsingHeader(label, flags);
    ImGui::PopStyleVar();
    return open;
}

// Animated toggle switch (iOS-style oval track + sliding knob).
inline bool ToggleSwitch(const char* str_id, bool* v) {
    const float h = ImGui::GetFrameHeight() * 0.75f;
    const float w = h * 1.9f;
    const float r = h * 0.5f;

    ImVec2 pos = ImGui::GetCursorScreenPos();

    ImGui::PushID(str_id);
    ImGuiID anim_id = ImGui::GetID("##t_anim");
    bool pressed = ImGui::InvisibleButton("##toggle", ImVec2(w, h));
    if (pressed) *v = !*v;
    bool hovered = ImGui::IsItemHovered();
    ImGuiStorage* storage = ImGui::GetStateStorage();
    ImGui::PopID();

    float t = storage->GetFloat(anim_id, *v ? 1.0f : 0.0f);
    float target = *v ? 1.0f : 0.0f;
    float spd = ImGui::GetIO().DeltaTime * 8.0f;
    t = (t < target) ? std::min(t + spd, target) : std::max(t - spd, target);
    storage->SetFloat(anim_id, t);

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec4 off_c = ImGui::GetStyleColorVec4(ImGuiCol_FrameBg);
    ImVec4 on_c  = ImGui::GetStyleColorVec4(ImGuiCol_Button);
    ImVec4 trk(off_c.x + (on_c.x - off_c.x) * t,
               off_c.y + (on_c.y - off_c.y) * t,
               off_c.z + (on_c.z - off_c.z) * t, 1.0f);
    dl->AddRectFilled(pos, ImVec2(pos.x + w, pos.y + h),
                      ImGui::ColorConvertFloat4ToU32(trk), r);

    float kr = r - 2.0f;
    float kx = pos.x + r + t * (w - h);
    float ky = pos.y + r;
    ImU32 knob = hovered ? IM_COL32(255, 255, 255, 255)
                         : IM_COL32(220, 220, 225, 255);
    dl->AddCircleFilled(ImVec2(kx, ky), kr, knob);

    return pressed;
}

// Search field with embedded magnifier icon and clear button.
inline bool SearchInput(const char* str_id, char* buf, size_t buf_size,
                        float width = -1.0f) {
    ImGui::PushID(str_id);
    const ImGuiStyle& style = ImGui::GetStyle();
    float h = ImGui::GetFrameHeight();
    if (width < 0) width = ImGui::GetContentRegionAvail().x;

    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 icon_sz = ImGui::CalcTextSize(ICON_FA_SEARCH);
    float pad = style.FramePadding.x;
    float icon_space = pad + icon_sz.x + 4.0f;

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                        ImVec2(icon_space, style.FramePadding.y));
    ImGui::SetNextItemWidth(width);
    bool changed = ImGui::InputTextWithHint(
        "##in", "\xd0\x9f\xd0\xbe\xd0\xb8\xd1\x81\xd0\xba...", buf, buf_size);
    ImGui::PopStyleVar();

    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddText(ImVec2(pos.x + pad, pos.y + (h - icon_sz.y) * 0.5f),
                ImGui::GetColorU32(ImGuiCol_TextDisabled), ICON_FA_SEARCH);

    if (buf[0] != '\0') {
        ImVec2 clr_sz = ImGui::CalcTextSize(ICON_FA_TIMES);
        float cx = pos.x + width - pad - clr_sz.x;
        float cy = pos.y + (h - clr_sz.y) * 0.5f;
        ImVec2 saved = ImGui::GetCursorScreenPos();
        ImGui::SetCursorScreenPos(ImVec2(cx - 4, pos.y));
        if (ImGui::InvisibleButton("##clr", ImVec2(clr_sz.x + 8, h))) {
            buf[0] = '\0';
            changed = true;
        }
        ImU32 cc = ImGui::IsItemHovered()
                       ? ImGui::GetColorU32(ImGuiCol_Text)
                       : ImGui::GetColorU32(ImGuiCol_TextDisabled);
        dl->AddText(ImVec2(cx, cy), cc, ICON_FA_TIMES);
        ImGui::SetCursorScreenPos(saved);
    }

    ImGui::PopID();
    return changed;
}

// Spinning arc loading indicator.
inline void Spinner(const char* str_id, float radius = 10.0f,
                    float thickness = 3.0f) {
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImGui::Dummy(ImVec2(radius * 2.0f, radius * 2.0f));
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 c(pos.x + radius, pos.y + radius);
    float a0 = std::fmod((float)ImGui::GetTime() * 5.0f, IM_PI * 2.0f);
    float a1 = a0 + IM_PI * 1.2f;
    ImU32 col = ImGui::GetColorU32(ImGuiCol_Button);
    dl->PathClear();
    for (int i = 0; i <= 30; ++i) {
        float a = a0 + (a1 - a0) * (float)i / 30.0f;
        dl->PathLineTo(ImVec2(c.x + std::cos(a) * radius,
                               c.y + std::sin(a) * radius));
    }
    dl->PathStroke(col, 0, thickness);
}

// Gradient fade-in / fade-out horizontal separator.
inline void GradientSeparator(float pad_x = 0.0f) {
    ImVec2 pos = ImGui::GetCursorScreenPos();
    float w = ImGui::GetContentRegionAvail().x - pad_x * 2.0f;
    ImGui::Dummy(ImVec2(w + pad_x * 2.0f, 4.0f));
    ImDrawList* dl = ImGui::GetWindowDrawList();
    float y = pos.y + 1.5f;
    ImVec2 p0(pos.x + pad_x, y);
    ImVec2 pm(pos.x + pad_x + w * 0.5f, y + 1.0f);
    ImVec2 p1(pos.x + pad_x + w, y + 1.0f);
    ImU32 trans  = Colors::Accent(0);
    ImU32 accent = Colors::Accent(100);
    dl->AddRectFilledMultiColor(p0, pm, trans, accent, accent, trans);
    dl->AddRectFilledMultiColor(ImVec2(pm.x, p0.y), p1,
                                accent, trans, trans, accent);
}

// Card container with elevated background.
// Pattern: if (CardBegin("Title")) { ...content... } CardEnd();
inline bool CardBegin(const char* title, bool collapsible = true) {
    ImGui::PushID(title);
    ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_ChildBg, Colors::CardBg);
    ImGui::PushStyleColor(ImGuiCol_Border,  Colors::CardBorder);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding,  8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);

    ImGui::BeginChild("##card", ImVec2(0, 0),
                      ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders);

    bool open = true;
    if (collapsible) {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
        open = ImGui::CollapsingHeader(title, ImGuiTreeNodeFlags_DefaultOpen);
        ImGui::PopStyleVar();
    } else {
        ImGui::TextDisabled("%s", title);
    }
    return open;
}

inline void CardEnd() {
    ImGui::EndChild();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(2);
    ImGui::PopID();
}

// Notification badge drawn at the top-right corner of the previous item.
inline void Badge(int count) {
    if (count <= 0) return;
    ImVec2 mn = ImGui::GetItemRectMin();
    ImVec2 mx = ImGui::GetItemRectMax();
    char text[16];
    std::snprintf(text, sizeof(text), "%d", count > 99 ? 99 : count);
    ImVec2 tsz = ImGui::CalcTextSize(text);
    float r = std::max(tsz.x, tsz.y) * 0.5f + 3.0f;
    ImVec2 center(mx.x - 2.0f, mn.y + 2.0f);
    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddCircleFilled(center, r, IM_COL32(0xC7, 0x47, 0x4D, 255));
    dl->AddText(ImVec2(center.x - tsz.x * 0.5f, center.y - tsz.y * 0.5f),
                IM_COL32(255, 255, 255, 255), text);
}

// Custom slider (float) styled like ProgressBar: gradient track, gloss, round thumb.
inline bool SliderFloat(const char* label, float* v, float v_min, float v_max,
                        const char* fmt = "%.2f", const ImVec2& size_arg = ImVec2(-1, 0)) {
    ImGui::PushID(label);

    const ImGuiStyle& style = ImGui::GetStyle();
    float avail  = ImGui::GetContentRegionAvail().x;
    ImVec2 tsz   = ImGui::CalcTextSize(label);
    float label_w = tsz.x + style.ItemInnerSpacing.x;
    float w = (size_arg.x > 0) ? size_arg.x : (avail - label_w);
    float h = (size_arg.y > 0) ? size_arg.y
              : (ImGui::GetFontSize() + style.FramePadding.y * 2.0f);

    ImVec2 pos = ImGui::GetCursorScreenPos();
    bool pressed = ImGui::InvisibleButton("##slider", ImVec2(w, h));
    bool hovered = ImGui::IsItemHovered();
    bool active  = ImGui::IsItemActive();

    if (active) {
        float mx = ImGui::GetIO().MousePos.x;
        float t  = std::clamp((mx - pos.x) / w, 0.0f, 1.0f);
        *v = v_min + t * (v_max - v_min);
    }

    float frac = (v_max > v_min)
                     ? std::clamp((*v - v_min) / (v_max - v_min), 0.0f, 1.0f)
                     : 0.0f;
    float rnd = h * 0.5f;
    float thumb_r = h * 0.42f;
    float thumb_cx = std::clamp(pos.x + w * frac,
                                pos.x + thumb_r, pos.x + w - thumb_r);
    float thumb_y  = pos.y + h * 0.5f;
    float fill_end = thumb_cx + thumb_r;

    ImDrawList* dl = ImGui::GetWindowDrawList();

    dl->AddRectFilled(pos, ImVec2(pos.x + w, pos.y + h),
                      ImGui::GetColorU32(ImGuiCol_FrameBg), rnd);

    if (fill_end > pos.x + 1.0f) {
        ImU32 fill = active  ? Colors::AccentBright(1.25f)
                   : hovered ? Colors::AccentBright(1.15f)
                             : Colors::Accent();
        dl->AddRectFilled(pos, ImVec2(fill_end, pos.y + h), fill, rnd);
        float mid = pos.y + h * 0.4f;
        dl->AddRectFilledMultiColor(
            pos, ImVec2(fill_end, mid),
            IM_COL32(255, 255, 255, 35), IM_COL32(255, 255, 255, 15),
            IM_COL32(255, 255, 255, 0),  IM_COL32(255, 255, 255, 0));
    }

    ImU32 thumb_shadow = IM_COL32(0, 0, 0, 60);
    ImU32 thumb_col    = active  ? IM_COL32(255, 255, 255, 255)
                       : hovered ? IM_COL32(240, 240, 245, 255)
                                 : IM_COL32(220, 220, 225, 255);
    dl->AddCircleFilled(ImVec2(thumb_cx, thumb_y + 1.0f), thumb_r, thumb_shadow);
    dl->AddCircleFilled(ImVec2(thumb_cx, thumb_y), thumb_r, thumb_col);
    dl->AddCircle(ImVec2(thumb_cx, thumb_y), thumb_r, Colors::Accent(120), 0, 1.2f);

    char val_buf[64];
    std::snprintf(val_buf, sizeof(val_buf), fmt, *v);
    ImVec2 val_sz = ImGui::CalcTextSize(val_buf);
    float val_x = pos.x + (w - val_sz.x) * 0.5f;
    float val_y = pos.y + (h - val_sz.y) * 0.5f;
    bool thumb_overlaps_text = (thumb_cx - thumb_r < val_x + val_sz.x) &&
                               (thumb_cx + thumb_r > val_x);
    if (thumb_overlaps_text) {
        dl->AddText(ImVec2(val_x + 1.0f, val_y + 1.0f), IM_COL32(0, 0, 0, 140), val_buf);
    }
    dl->AddText(ImVec2(val_x, val_y),
                ImGui::GetColorU32(ImGuiCol_Text), val_buf);

    ImGui::SameLine();
    ImGui::TextUnformatted(label);

    ImGui::PopID();
    return active;
}

// Custom slider (int) styled like ProgressBar.
inline bool SliderInt(const char* label, int* v, int v_min, int v_max,
                      const ImVec2& size_arg = ImVec2(-1, 0)) {
    float fv = static_cast<float>(*v);
    bool changed = SliderFloat(label, &fv,
                               static_cast<float>(v_min),
                               static_cast<float>(v_max),
                               "%0.0f", size_arg);
    *v = static_cast<int>(std::round(fv));
    *v = std::clamp(*v, v_min, v_max);
    return changed;
}

// Progress bar with gradient fill and glossy highlight.
inline void ProgressBar(float fraction, const ImVec2& size_arg = ImVec2(-1, 0),
                        const char* overlay = nullptr) {
    ImVec2 pos = ImGui::GetCursorScreenPos();
    const ImGuiStyle& style = ImGui::GetStyle();
    float w = (size_arg.x > 0) ? size_arg.x : ImGui::GetContentRegionAvail().x;
    float h = (size_arg.y > 0) ? size_arg.y
              : (ImGui::GetFontSize() + style.FramePadding.y * 2.0f);
    ImGui::Dummy(ImVec2(w, h));
    ImDrawList* dl = ImGui::GetWindowDrawList();
    float rnd = h * 0.5f;
    dl->AddRectFilled(pos, ImVec2(pos.x + w, pos.y + h),
                      ImGui::GetColorU32(ImGuiCol_FrameBg), rnd);
    float fw = w * std::clamp(fraction, 0.0f, 1.0f);
    if (fw > 1.0f) {
        ImVec2 fp1(pos.x + fw, pos.y + h);
        dl->AddRectFilled(pos, fp1, Colors::Accent(), rnd);
        float mid = pos.y + h * 0.4f;
        dl->AddRectFilledMultiColor(
            pos, ImVec2(pos.x + fw, mid),
            IM_COL32(255, 255, 255, 35), IM_COL32(255, 255, 255, 15),
            IM_COL32(255, 255, 255, 0),  IM_COL32(255, 255, 255, 0));
    }
    if (overlay) {
        ImVec2 tsz = ImGui::CalcTextSize(overlay);
        dl->AddText(ImVec2(pos.x + (w - tsz.x) * 0.5f,
                           pos.y + (h - tsz.y) * 0.5f),
                    ImGui::GetColorU32(ImGuiCol_Text), overlay);
    }
}

// ── Decorative helpers ──────────────────────────────────────────────

inline void AccentBar(ImDrawList* dl, const ImVec2& pos, const ImVec2& size, bool vertical = true) {
    ImU32 accent1 = Colors::Accent();
    ImU32 accent2 = Colors::AccentDim(0.65f);
    if (vertical) {
        float w = 3.0f;
        dl->AddRectFilledMultiColor(pos, ImVec2(pos.x + w, pos.y + size.y),
                                    accent1, accent1, accent2, accent2);
    } else {
        float h = 2.5f;
        dl->AddRectFilledMultiColor(pos, ImVec2(pos.x + size.x, pos.y + h),
                                    accent1, accent2, accent2, accent1);
    }
}

inline void PanelBorder(ImDrawList* dl, const ImVec2& mn, const ImVec2& mx, bool active = false) {
    ImU32 border = ImGui::GetColorU32(ImGuiCol_Border);
    ImU32 accent = Colors::Accent();
    dl->AddRect(mn, mx, border, 8.0f, 0, 1.0f);
    if (active) {
        float w = (mx.x - mn.x) * 0.3f;
        dl->AddLine(ImVec2(mn.x + 8, mn.y), ImVec2(mn.x + 8 + w, mn.y), accent, 2.0f);
    }
}

} // namespace DrUI
