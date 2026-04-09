#ifndef MENU_H
#define MENU_H

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
    ImGuiStyle& s = GetStyle();

    s.WindowRounding = 6;
    s.FrameRounding = 4;
    s.TabRounding = 4;
    s.WindowBorderSize = 1;

    s.WindowPadding = ImVec2(16,16);
    s.FramePadding = ImVec2(10,8);
    s.ItemSpacing = ImVec2(12,10);

    ImVec4* c = s.Colors;
    ImVec4 accent = ImVec4(0.0f,0.85f,0.55f,1);

    c[ImGuiCol_WindowBg] = ImVec4(0.05f,0.05f,0.06f,1);
    c[ImGuiCol_Button] = ImVec4(0.1f,0.1f,0.12f,1);
    c[ImGuiCol_ButtonHovered] = accent;
    c[ImGuiCol_FrameBg] = ImVec4(0.08f,0.08f,0.1f,1);
    c[ImGuiCol_CheckMark] = accent;
}

// ================= MENU =================
inline void DrawMenu()
{
    static bool esp = false;
    static bool aim = false;
    static float recoil = 0;

    SetNextWindowSize(ImVec2(600,420), ImGuiCond_FirstUseEver);

    Begin("GYMFLEX INTERFACE");

    Text("GYMFLEX PANEL");
    Separator();

    if (BeginTabBar("tabs")) {

        if (BeginTabItem("Visual")) {
            Checkbox("ESP", &esp);
            EndTabItem();
        }

        if (BeginTabItem("Combat")) {
            Checkbox("Aimbot", &aim);
            SliderFloat("Recoil", &recoil, 0, 100);
            EndTabItem();
        }

        EndTabBar();
    }

    End();
}

// ================= SETUP =================
inline void SetupImgui()
{
    IMGUI_CHECKVERSION();
    CreateContext();

    ImGuiIO &io = GetIO();
    io.DisplaySize = ImVec2(glWidth, glHeight);

    ImGui_ImplOpenGL3_Init("#version 100");

    StyleColorsDark();
    ApplyLuxuryStyle();

    io.Fonts->AddFontFromMemoryTTF(Roboto_Regular, 30, 40);
}

// ================= HOOK =================
inline EGLBoolean (*old_eglSwapBuffers)(EGLDisplay, EGLSurface);

inline EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface)
{
    eglQuerySurface(dpy, surface, EGL_WIDTH, &glWidth);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &glHeight);

    if (!setupimg) {
        SetupImgui();
        setupimg = true;
    }

    if (isSafeToDraw) {

        ImGuiIO &io = GetIO();
        io.DisplaySize = ImVec2(glWidth, glHeight);

        // ✅ TOUCH FINAL (RAW 1:1)
        if (touch_x >= 0 && touch_y >= 0) {
            io.MousePos = ImVec2(touch_x, touch_y);
        }

        io.MouseDown[0] = is_touch_down;

        ImGui_ImplOpenGL3_NewFrame();
        NewFrame();

        DrawMenu();

        EndFrame();
        Render();

        glViewport(0,0,glWidth,glHeight);
        ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
    }

    return old_eglSwapBuffers(dpy, surface);
}

#endif
