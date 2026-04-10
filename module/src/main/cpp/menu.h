#ifndef ZYGISK_MENU_TEMPLATE_MENU_H
#define ZYGISK_MENU_TEMPLATE_MENU_H

#include <string>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
using namespace ImGui;

extern bool isSafeToDraw;
extern bool setupimg;

// Variabel Sinkronisasi Resolusi
float g_GameW = 1.0f, g_GameH = 1.0f;
float g_HardwareW = 1.0f, g_HardwareH = 1.0f;

inline void DrawMenu() {
    SetNextWindowSize(ImVec2(550, 400), ImGuiCond_FirstUseEver);
    if (Begin("GYMFLEX - AUTO SCALE 1:1", nullptr)) {
        TextColored(ImVec4(0, 1, 0, 1), "Sentuhan: Sinkron Otomatis");
        Separator();
        static bool dummy = false;
        Checkbox("Feature Test", &dummy);
        End();
    }
}

inline void SetupImgui() {
    IMGUI_CHECKVERSION();
    CreateContext();
    ImGui_ImplAndroid_Init(nullptr); 
    ImGui_ImplOpenGL3_Init("#version 300 es");
    StyleColorsDark();
    GetStyle().ScaleAllSizes(3.5f);
}

inline EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);
inline EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    // A. Ambil Ukuran HP Asli (Hardware)
    EGLint hw_w, hw_h;
    eglQuerySurface(dpy, surface, EGL_WIDTH, &hw_w);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &hw_h);

    // B. Ambil Ukuran Kanvas Game (Viewport)
    GLint v[4]; glGetIntegerv(GL_VIEWPORT, v);

    if (v[2] > 0 && hw_w > 0) {
        g_HardwareW = (float)hw_w; g_HardwareH = (float)hw_h;
        g_GameW = (float)v[2]; g_GameH = (float)v[3];
    }

    if (!setupimg) { SetupImgui(); setupimg = true; }

    if (isSafeToDraw) {
        ImGuiIO &io = GetIO();
        io.DisplaySize = ImVec2(g_GameW, g_GameH);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplAndroid_NewFrame();
        NewFrame();
        DrawMenu(); 
        EndFrame();
        Render();
        
        glDisable(GL_SCISSOR_TEST); 
        glViewport(v[0], v[1], v[2], v[3]); 
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    return old_eglSwapBuffers(dpy, surface);
}
#endif
