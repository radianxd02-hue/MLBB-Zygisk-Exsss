#ifndef ZYGISK_MENU_TEMPLATE_MENU_H
#define ZYGISK_MENU_TEMPLATE_MENU_H

#include <string>

using namespace ImGui;

extern bool isSafeToDraw;
extern bool setupimg;
extern int glWidth;
extern int glHeight;

// Touch input dari hook.cpp
extern float touch_x;
extern float touch_y;
extern bool is_touch_down;

// ================= STYLE MEWAH =================
inline void ApplyLuxuryStyle()
{
    ImGuiStyle& style = GetStyle();

    style.WindowRounding = 18.0f;
    style.FrameRounding = 10.0f;
    style.ScrollbarRounding = 12.0f;
    style.GrabRounding = 8.0f;
    style.TabRounding = 10.0f;

    style.WindowPadding = ImVec2(15, 15);
    style.FramePadding = ImVec2(10, 8);
    style.ItemSpacing = ImVec2(10, 8);

    ImVec4* colors = style.Colors;

    ImVec4 accent = ImVec4(0.0f, 1.0f, 0.4f, 1.0f);

    colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.08f, 0.95f);

    colors[ImGuiCol_TitleBg] = ImVec4(0.05f, 0.05f, 0.07f, 1.0f);
    colors[ImGuiCol_TitleBgActive] = accent;

    colors[ImGuiCol_Button] = ImVec4(0.10f, 0.10f, 0.12f, 1.0f);
    colors[ImGuiCol_ButtonHovered] = accent;
    colors[ImGuiCol_ButtonActive] = ImVec4(0.0f, 0.8f, 0.3f, 1.0f);

    colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.12f, 0.14f, 1.0f);
    colors[ImGuiCol_FrameBgHovered] = accent;
    colors[ImGuiCol_FrameBgActive] = accent;

    colors[ImGuiCol_CheckMark] = accent;
    colors[ImGuiCol_SliderGrab] = accent;
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.0f, 0.8f, 0.3f, 1.0f);

    colors[ImGuiCol_Tab] = ImVec4(0.10f, 0.10f, 0.12f, 1.0f);
    colors[ImGuiCol_TabHovered] = accent;
    colors[ImGuiCol_TabActive] = accent;
}

// ================= MENU =================
inline void DrawMenu()
{
    static bool enableESP = false;
    static bool enableAimbot = false;
    static float recoilControl = 0.0f;
    static std::string scanStatus = "Belum di-scan";
    static uintptr_t GWorld = 0;

    SetNextWindowSize(ImVec2(520, 420), ImGuiCond_FirstUseEver);

    Begin("GYMFLEX PRO PANEL", nullptr,
          ImGuiWindowFlags_NoResize |
          ImGuiWindowFlags_NoCollapse);

    // HEADER
    TextColored(ImVec4(0.0f, 1.0f, 0.4f, 1.0f), "● SYSTEM ONLINE");
    SameLine();
    TextDisabled("| PUBG Injector");

    Separator();

    // STATUS BOX
    BeginChild("StatusBox", ImVec2(0, 90), true);

    Text("Map Status:");
    SameLine();
    TextColored(ImVec4(1,1,0,1), "%s", scanStatus.c_str());

    if (GWorld != 0) {
        TextColored(ImVec4(0,1,0,1), "GWorld: %p", (void*)GWorld);
    } else {
        TextDisabled("GWorld: Not Found");
    }

    EndChild();

    Spacing();

    // BUTTON
    if (Button("SCAN GWorld", ImVec2(-1, 45))) {
        scanStatus = "Scanning memori... (Segera hadir)";
    }

    Spacing();
    Separator();

    // TABS
    if (BeginTabBar("MainTabs")) {

        if (BeginTabItem("VISUAL")) {

            BeginChild("VisualBox", ImVec2(0, 120), true);

            Checkbox("Enable ESP Box", &enableESP);

            if (enableESP) {
                TextColored(ImVec4(1,1,0,1), "Requires GWorld Offset");
            }

            EndChild();
            EndTabItem();
        }

        if (BeginTabItem("COMBAT")) {

            BeginChild("CombatBox", ImVec2(0, 140), true);

            Checkbox("Aimbot (Safe Mode)", &enableAimbot);

            SliderFloat("Recoil Control", &recoilControl, 0.0f, 100.0f, "%.0f%%");

            EndChild();
            EndTabItem();
        }

        EndTabBar();
    }

    End();
}

// ================= SETUP =================
inline void SetupImgui() {
    IMGUI_CHECKVERSION();
    CreateContext();
    ImGuiIO &io = GetIO();

    io.DisplaySize = ImVec2((float) glWidth, (float) glHeight);

    ImGui_ImplOpenGL3_Init("#version 100");

    StyleColorsDark();
    ApplyLuxuryStyle();

    GetStyle().ScaleAllSizes(3.0f);

    io.Fonts->AddFontFromMemoryTTF(Roboto_Regular, 30, 30.0f);
}

// ================= HOOK =================
inline EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);

inline EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    eglQuerySurface(dpy, surface, EGL_WIDTH, &glWidth);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &glHeight);

    // Fix portrait bug
    if (glHeight > glWidth) {
        int temp = glWidth;
        glWidth = glHeight;
        glHeight = temp;
    }

    if (!setupimg)
    {
        SetupImgui();
        setupimg = true;
    }

    if (isSafeToDraw) {
        ImGuiIO &io = GetIO();
        io.DisplaySize = ImVec2((float)glWidth, (float)glHeight);

        // TOUCH INPUT
        if (touch_x >= 0.0f && touch_y >= 0.0f) {
            io.MousePos = ImVec2(touch_x, touch_y);
        }
        io.MouseDown[0] = is_touch_down;

        ImGui_ImplOpenGL3_NewFrame();
        NewFrame();

        DrawMenu();

        EndFrame();
        Render();

        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glDisable(GL_SCISSOR_TEST);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    return old_eglSwapBuffers(dpy, surface);
}

#endif // ZYGISK_MENU_TEMPLATE_MENU_H
