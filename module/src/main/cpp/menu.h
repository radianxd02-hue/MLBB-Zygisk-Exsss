#ifndef ZYGISK_MENU_TEMPLATE_MENU_H
#define ZYGISK_MENU_TEMPLATE_MENU_H

#include <string>
#include <algorithm>

using namespace ImGui;

extern bool isSafeToDraw;
extern bool setupimg;
extern int glWidth;
extern int glHeight;

extern float touch_x;
extern float touch_y;
extern bool is_touch_down;

// ================= STYLE =================
inline void ApplyLuxuryStyle()
{
    ImGuiStyle& style = GetStyle();

    style.WindowRounding = 6.0f;
    style.FrameRounding = 4.0f;
    style.TabRounding = 4.0f;
    style.GrabRounding = 3.0f;

    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;

    style.WindowPadding = ImVec2(16, 16);
    style.FramePadding = ImVec2(10, 8);
    style.ItemSpacing = ImVec2(12, 10);

    ImVec4* c = style.Colors;

    ImVec4 accent = ImVec4(0.0f, 0.85f, 0.55f, 1.0f);

    c[ImGuiCol_WindowBg] = ImVec4(0.04f, 0.04f, 0.05f, 0.98f);
    c[ImGuiCol_Border]   = ImVec4(0.15f, 0.15f, 0.17f, 0.6f);

    c[ImGuiCol_TitleBg]       = ImVec4(0.05f, 0.05f, 0.06f, 1.0f);
    c[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.08f, 1.0f);

    c[ImGuiCol_Button]        = ImVec4(0.10f, 0.10f, 0.12f, 1.0f);
    c[ImGuiCol_ButtonHovered] = accent;
    c[ImGuiCol_ButtonActive]  = ImVec4(0.0f, 0.7f, 0.4f, 1.0f);

    c[ImGuiCol_FrameBg]        = ImVec4(0.08f, 0.08f, 0.10f, 1.0f);
    c[ImGuiCol_FrameBgHovered] = ImVec4(0.10f, 0.10f, 0.12f, 1.0f);
    c[ImGuiCol_FrameBgActive]  = accent;

    c[ImGuiCol_CheckMark]       = accent;
    c[ImGuiCol_SliderGrab]      = accent;
    c[ImGuiCol_SliderGrabActive]= ImVec4(0.0f, 0.7f, 0.4f, 1.0f);

    c[ImGuiCol_Tab]        = ImVec4(0.08f, 0.08f, 0.10f, 1.0f);
    c[ImGuiCol_TabHovered] = accent;
    c[ImGuiCol_TabActive]  = ImVec4(0.12f, 0.12f, 0.14f, 1.0f);
}

// ================= MENU =================
inline void DrawMenu()
{
    static bool enableESP = false;
    static bool enableAimbot = false;
    static float recoilControl = 0.0f;
    static std::string scanStatus = "Belum di-scan";
    static uintptr_t GWorld = 0;

    SetNextWindowSize(ImVec2(600, 420), ImGuiCond_FirstUseEver);

    Begin("GYMFLEX INTERFACE", nullptr, ImGuiWindowFlags_NoCollapse);

    Text("GYMFLEX");
    SameLine();
    TextDisabled("| Professional Panel");

    Separator();

    BeginChild("StatusBox", ImVec2(0, 70), true);

    Text("Status:");
    SameLine();
    TextColored(ImVec4(0.0f, 0.85f, 0.55f, 1.0f), "%s", scanStatus.c_str());

    if (GWorld != 0)
        TextColored(ImVec4(0.0f, 0.85f, 0.55f, 1.0f), "GWorld Connected");
    else
        TextDisabled("GWorld Not Found");

    EndChild();

    Spacing();

    if (Button("Scan Memory", ImVec2(-1, 42))) {
        scanStatus = "Scanning...";
    }

    Spacing();
    Separator();

    if (BeginTabBar("MainTabs")) {

        if (BeginTabItem("Visual")) {

            BeginChild("VisualBox", ImVec2(0, 150), true);

            Checkbox("Enable ESP Box", &enableESP);

            if (enableESP) {
                TextDisabled("Requires GWorld Offset");
            }

            EndChild();
            EndTabItem();
        }

        if (BeginTabItem("Combat")) {

            BeginChild("CombatBox", ImVec2(0, 150), true);

            Checkbox("Enable Aimbot", &enableAimbot);
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

    // ✅ FIX UTAMA: jangan over scale
    GetStyle().ScaleAllSizes(1.2f);

    // ✅ Perbesar font, bukan scale UI
    io.Fonts->AddFontFromMemoryTTF(Roboto_Regular, 30, 40.0f);
}

// ================= HOOK =================
inline EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);

inline EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    eglQuerySurface(dpy, surface, EGL_WIDTH, &glWidth);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &glHeight);

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

        // ✅ CLAMP biar stabil
        touch_x = std::max(0.0f, std::min(touch_x, (float)glWidth));
        touch_y = std::max(0.0f, std::min(touch_y, (float)glHeight));

        if (touch_x >= 0.0f && touch_y >= 0.0f) {
            io.MousePos = ImVec2(touch_x, touch_y);
        }

        io.MouseDown[0] = is_touch_down;

        ImGui_ImplOpenGL3_NewFrame();
        NewFrame();

        DrawMenu();

        EndFrame();
        Render();

        glViewport(0, 0, glWidth, glHeight);
        glDisable(GL_SCISSOR_TEST);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    return old_eglSwapBuffers(dpy, surface);
}

#endif
