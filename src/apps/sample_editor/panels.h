#pragma once

#include "editor_layout.h"
#include <drui/drui.h>

#include <imgui.h>
#include <cstdio>
#include <cmath>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>

namespace fs = std::filesystem;

// ===================================================================
// Project file tree model
// ===================================================================

struct FileNode {
    std::string name;
    fs::path    full_path;
    bool        is_dir = false;
    std::vector<FileNode> children;
};

static void BuildFileTree(FileNode& node, int depth_limit = 6) {
    if (!node.is_dir || depth_limit <= 0) return;
    try {
        for (auto& entry : fs::directory_iterator(node.full_path,
                 fs::directory_options::skip_permission_denied)) {
            auto fname = entry.path().filename().string();
            if (fname.empty() || fname[0] == '.') continue;
            if (fname == "_build" || fname == "build" || fname == ".git") continue;

            FileNode child;
            child.name      = fname;
            child.full_path = entry.path();
            child.is_dir    = entry.is_directory();
            if (child.is_dir)
                BuildFileTree(child, depth_limit - 1);
            node.children.push_back(std::move(child));
        }
        std::sort(node.children.begin(), node.children.end(),
                  [](const FileNode& a, const FileNode& b) {
                      if (a.is_dir != b.is_dir) return a.is_dir > b.is_dir;
                      return a.name < b.name;
                  });
    } catch (...) {}
}

// ===================================================================
// Open file state
// ===================================================================

struct OpenFile {
    fs::path    path;
    std::string buffer;
    std::string original;
    bool        is_open = false;

    bool modified() const { return buffer != original; }

    void open(const fs::path& p) {
        std::ifstream ifs(p, std::ios::binary);
        if (!ifs) return;
        std::string content((std::istreambuf_iterator<char>(ifs)),
                             std::istreambuf_iterator<char>());
        path     = p;
        buffer   = content;
        original = content;
        is_open  = true;
    }

    bool save() {
        std::ofstream ofs(path, std::ios::binary);
        if (!ofs) return false;
        ofs.write(buffer.data(), (std::streamsize)buffer.size());
        original = buffer;
        return true;
    }

    void close() {
        path.clear();
        buffer.clear();
        original.clear();
        is_open = false;
    }
};

struct AppState {
    FileNode  root;
    OpenFile  file;
    std::vector<std::string> log_lines;
    int       log_counter = 0;

    void log(const char* msg) {
        char buf[256];
        std::snprintf(buf, sizeof(buf), "[%04d] %s", log_counter++, msg);
        log_lines.emplace_back(buf);
    }
};

// ===================================================================
// Main menu bar
// ===================================================================

static void DrawMainMenuBar(PanelVisibility& panels, AppState& app) {
    if (!ImGui::BeginMainMenuBar()) return;

    if (ImGui::BeginMenu(ICON_FA_FILE "  \xd0\xa4\xd0\xb0\xd0\xb9\xd0\xbb")) {
        if (ImGui::MenuItem(ICON_FA_SAVE " \xd0\xa1\xd0\xbe\xd1\x85\xd1\x80\xd0\xb0\xd0\xbd\xd0\xb8\xd1\x82\xd1\x8c", "Ctrl+S", false, app.file.is_open)) {
            if (app.file.save())
                app.log("\xd0\xa4\xd0\xb0\xd0\xb9\xd0\xbb \xd1\x81\xd0\xbe\xd1\x85\xd1\x80\xd0\xb0\xd0\xbd\xd1\x91\xd0\xbd");
            else
                app.log("\xd0\x9e\xd1\x88\xd0\xb8\xd0\xb1\xd0\xba\xd0\xb0 \xd1\x81\xd0\xbe\xd1\x85\xd1\x80\xd0\xb0\xd0\xbd\xd0\xb5\xd0\xbd\xd0\xb8\xd1\x8f");
        }
        if (ImGui::MenuItem(ICON_FA_TIMES " \xd0\x97\xd0\xb0\xd0\xba\xd1\x80\xd1\x8b\xd1\x82\xd1\x8c \xd1\x84\xd0\xb0\xd0\xb9\xd0\xbb", nullptr, false, app.file.is_open)) {
            app.file.close();
        }
        ImGui::Separator();
        if (ImGui::MenuItem(ICON_FA_TIMES " \xd0\x92\xd1\x8b\xd1\x85\xd0\xbe\xd0\xb4")) {
            // handled via glfwWindowShouldClose
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu(ICON_FA_EYE "  \xd0\x92\xd0\xb8\xd0\xb4")) {
        ImGui::MenuItem(ICON_FA_FOLDER " \xd0\x9e\xd0\xb1\xd0\xbe\xd0\xb7\xd1\x80\xd0\xb5\xd0\xb2\xd0\xb0\xd1\x82\xd0\xb5\xd0\xbb\xd1\x8c", nullptr, &panels.tab_explorer);
        ImGui::MenuItem(ICON_FA_BARS " \xd0\x9b\xd0\xbe\xd0\xb3", nullptr, &panels.tab_log);
        ImGui::Separator();
        ImGui::MenuItem(ICON_FA_BARS " \xd0\xa1\xd1\x82\xd1\x80\xd0\xbe\xd0\xba\xd0\xb0 \xd1\x81\xd0\xbe\xd1\x81\xd1\x82\xd0\xbe\xd1\x8f\xd0\xbd\xd0\xb8\xd1\x8f", nullptr, &panels.status_bar);
        ImGui::Separator();
        if (ImGui::BeginMenu(ICON_FA_EYE " \xd0\xa2\xd0\xb5\xd0\xbc\xd0\xb0")) {
            for (int i = 0; i < (int)DrUI::ThemeId::COUNT; ++i) {
                auto tid = static_cast<DrUI::ThemeId>(i);
                bool selected = (DrUI::g_current_theme == tid);
                if (ImGui::MenuItem(DrUI::ThemeName(tid), nullptr, selected))
                    DrUI::ApplyTheme(tid, panels.dpi_scale);
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu(ICON_FA_INFO_CIRCLE "  \xd0\xa1\xd0\xbf\xd1\x80\xd0\xb0\xd0\xb2\xd0\xba\xd0\xb0")) {
        if (ImGui::MenuItem("\xd0\x9e \xd0\xbf\xd1\x80\xd0\xbe\xd0\xb3\xd1\x80\xd0\xb0\xd0\xbc\xd0\xbc\xd0\xb5")) {
            DrUI::ShowToast("Sample Editor v1.0  \xc2\xb7  ImGui " IMGUI_VERSION,
                            DrUI::ToastType::Info, 4.0f);
        }
        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
}

// ===================================================================
// Left panel — file tree
// ===================================================================

static void DrawFileTreeNode(const FileNode& node, AppState& app) {
    if (node.is_dir) {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
                                   ImGuiTreeNodeFlags_SpanAvailWidth;
        char label[256];
        std::snprintf(label, sizeof(label), ICON_FA_FOLDER " %s", node.name.c_str());
        bool open = ImGui::TreeNodeEx(label, flags);
        if (open) {
            for (auto& child : node.children)
                DrawFileTreeNode(child, app);
            ImGui::TreePop();
        }
    } else {
        char label[256];
        std::snprintf(label, sizeof(label), ICON_FA_FILE_CODE " %s", node.name.c_str());
        if (ImGui::Selectable(label, app.file.is_open && app.file.path == node.full_path)) {
            app.file.open(node.full_path);
            char msg[512];
            std::snprintf(msg, sizeof(msg),
                          "\xd0\x9e\xd1\x82\xd0\xba\xd1\x80\xd1\x8b\xd1\x82: %s",
                          node.name.c_str());
            app.log(msg);
        }
    }
}

static void DrawLeftPanel(const PanelLayout& zone, float dpi_scale,
                          PanelVisibility& panels, AppState& app) {
    ImGui::SetNextWindowPos(zone.pos);
    ImGui::SetNextWindowSize(zone.size);
    ImGui::Begin("##LeftPanel", nullptr, kPanelFlags);

    if (ImGui::BeginTabBar("##LeftTabs", ImGuiTabBarFlags_DrawSelectedOverline)) {
        if (panels.tab_explorer && ImGui::BeginTabItem(ICON_FA_FOLDER " \xd0\x9f\xd1\x80\xd0\xbe\xd0\xb5\xd0\xba\xd1\x82", &panels.tab_explorer)) {
            if (DrUI::Button(ICON_FA_REDO " \xd0\x9e\xd0\xb1\xd0\xbd\xd0\xbe\xd0\xb2\xd0\xb8\xd1\x82\xd1\x8c")) {
                app.root.children.clear();
                BuildFileTree(app.root);
                app.log("\xd0\x94\xd0\xb5\xd1\x80\xd0\xb5\xd0\xb2\xd0\xbe \xd0\xbe\xd0\xb1\xd0\xbd\xd0\xbe\xd0\xb2\xd0\xbb\xd0\xb5\xd0\xbd\xd0\xbe");
            }
            ImGui::Separator();
            ImGui::BeginChild("##file_tree", ImVec2(0, 0), false);
            for (auto& child : app.root.children)
                DrawFileTreeNode(child, app);
            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}

// ===================================================================
// Center panel — text editor
// ===================================================================

static int TextEditCallback(ImGuiInputTextCallbackData* data) {
    // Resize the string buffer when ImGui needs more space
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
        auto* str = static_cast<std::string*>(data->UserData);
        str->resize(data->BufTextLen);
        data->Buf = str->data();
    }
    return 0;
}

static void DrawCenterPanel(const PanelLayout& zone, AppState& app) {
    ImGui::SetNextWindowPos(zone.pos);
    ImGui::SetNextWindowSize(zone.size);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("##CenterPanel", nullptr,
                 kPanelFlags | ImGuiWindowFlags_NoScrollbar |
                 ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::PopStyleVar();

    if (!app.file.is_open) {
        ImVec2 sz = ImGui::GetContentRegionAvail();
        const char* placeholder = "\xd0\x9d\xd0\xb5\xd1\x82 \xd0\xbe\xd1\x82\xd0\xba\xd1\x80\xd1\x8b\xd1\x82\xd0\xbe\xd0\xb3\xd0\xbe \xd1\x84\xd0\xb0\xd0\xb9\xd0\xbb\xd0\xb0";
        ImVec2 tsz = ImGui::CalcTextSize(placeholder);
        ImGui::SetCursorPos(ImVec2((sz.x - tsz.x) * 0.5f, (sz.y - tsz.y) * 0.5f));
        ImGui::TextDisabled("%s", placeholder);
    } else {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));

        // File tab header
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4));
            ImGui::SetCursorPos(ImVec2(4, 4));

            std::string tab_label = app.file.path.filename().string();
            if (app.file.modified()) tab_label += " *";
            ImGui::TextUnformatted(tab_label.c_str());
            ImGui::SameLine();
            if (DrUI::IconButton(ICON_FA_TIMES)) {
                app.file.close();
                ImGui::PopStyleVar(3);
                ImGui::End();
                return;
            }
            ImGui::Separator();
            ImGui::PopStyleVar();
        }

        ImVec2 avail = ImGui::GetContentRegionAvail();

        ImGuiInputTextFlags flags = ImGuiInputTextFlags_CallbackResize |
                                    ImGuiInputTextFlags_AllowTabInput;
        ImGui::InputTextMultiline("##editor_text", app.file.buffer.data(),
                                  app.file.buffer.capacity() + 1,
                                  avail, flags, TextEditCallback, &app.file.buffer);
        // InputTextMultiline may have modified the string in place; sync size
        app.file.buffer.resize(std::strlen(app.file.buffer.c_str()));

        ImGui::PopStyleVar(2);
    }

    ImGui::End();
}

// ===================================================================
// Bottom panel — Log
// ===================================================================

static void DrawLogTab(AppState& app) {
    if (DrUI::Button(ICON_FA_PLUS " \xd0\x94\xd0\xbe\xd0\xb1\xd0\xb0\xd0\xb2\xd0\xb8\xd1\x82\xd1\x8c")) {
        app.log("Test event");
    }
    ImGui::SameLine();
    if (DrUI::Button(ICON_FA_TRASH " \xd0\x9e\xd1\x87\xd0\xb8\xd1\x81\xd1\x82\xd0\xb8\xd1\x82\xd1\x8c")) {
        app.log_lines.clear();
        app.log_counter = 0;
    }

    ImGui::BeginChild("##log_scroll", ImVec2(0, 0), ImGuiChildFlags_Borders);
    for (auto& line : app.log_lines)
        ImGui::TextUnformatted(line.c_str());
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);
    ImGui::EndChild();
}

static void DrawBottomPanel(const PanelLayout& zone, float dpi_scale,
                             PanelVisibility& panels, AppState& app) {
    ImGui::SetNextWindowPos(zone.pos);
    ImGui::SetNextWindowSize(zone.size);
    if (panels.bottom_collapsed)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 0));
    ImGui::Begin("##BottomPanel", nullptr, kPanelFlags);
    if (panels.bottom_collapsed)
        ImGui::PopStyleVar();

    if (ImGui::BeginTabBar("##BottomTabs", ImGuiTabBarFlags_DrawSelectedOverline)) {
        if (panels.tab_log && ImGui::BeginTabItem(ICON_FA_BARS " \xd0\x9b\xd0\xbe\xd0\xb3", &panels.tab_log)) {
            if (!panels.bottom_collapsed) DrawLogTab(app);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}

// ===================================================================
// Status bar
// ===================================================================

static void DrawStatusBar(float dpi_scale, const AppState& app) {
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
                       ICON_FA_BOLT " Sample Editor");

    ImGui::SameLine(0, 8);
    ImGui::TextColored(DrUI::Colors::TextDisabled, "|");
    ImGui::SameLine(0, 8);

    if (app.file.is_open) {
        ImGui::TextColored(DrUI::Colors::TextSecondary, "%s",
                           app.file.path.filename().string().c_str());
        if (app.file.modified()) {
            ImGui::SameLine(0, 4);
            ImGui::TextColored(DrUI::Colors::Warning, "(modified)");
        }
        ImGui::SameLine(0, 8);
        ImGui::TextColored(DrUI::Colors::TextDisabled, "|");
        ImGui::SameLine(0, 8);
    }

    ImVec4 fps_col = (io.Framerate >= 55.0f)
        ? DrUI::Colors::Success
        : DrUI::Colors::Warning;
    ImGui::TextColored(fps_col, "%.0f FPS", io.Framerate);

    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
}

// ===================================================================
// Keyboard shortcuts
// ===================================================================

static void HandleShortcuts(AppState& app) {
    ImGuiIO& io = ImGui::GetIO();
    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S) && app.file.is_open) {
        if (app.file.save())
            app.log("\xd0\xa4\xd0\xb0\xd0\xb9\xd0\xbb \xd1\x81\xd0\xbe\xd1\x85\xd1\x80\xd0\xb0\xd0\xbd\xd1\x91\xd0\xbd");
        else
            app.log("\xd0\x9e\xd1\x88\xd0\xb8\xd0\xb1\xd0\xba\xd0\xb0 \xd1\x81\xd0\xbe\xd1\x85\xd1\x80\xd0\xb0\xd0\xbd\xd0\xb5\xd0\xbd\xd0\xb8\xd1\x8f");
    }
}
