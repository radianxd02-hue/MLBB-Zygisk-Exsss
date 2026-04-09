#ifndef MENU_H
#define MENU_H

#include <string>
#include <algorithm>
#include <android/log.h>

using namespace ImGui;

// ✅ LOG TAG
#define LOG_TAG2 "IMGUI_RENDER"
#define LOGD2(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG2, __VA_ARGS__)

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
}

// ================= MENU =================
inline void DrawMenu()
{
    static bool esp = false;

    SetNextWindowSize(ImVec2(600,400), ImGuiCond_FirstUseEver);

    Begin("DEBUG PANEL");

    Text("Touch Debug Active");
    Checkbox("ESP", &esp);

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

        // 🔥 LOG RENDER INFO
        LOGD2("GL SIZE: %d x %d", glWidth, glHeight);
        LOGD2("TOUCH FINAL: %.2f %.2f | DOWN: %d",
              touch_x, touch_y, is_touch_down);

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
