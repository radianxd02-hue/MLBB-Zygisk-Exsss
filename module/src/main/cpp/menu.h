#ifndef ZYGISK_MENU_TEMPLATE_MENU_H
#define ZYGISK_MENU_TEMPLATE_MENU_H

#include <EGL/egl.h>
#include <GLES2/gl2.h>

using namespace ImGui;
extern bool isSafeToDraw, setupimg;

// Variabel Penentu Keadilan Kordinat
float g_VpX = 0, g_VpY = 0, g_VpW = 0, g_VpH = 0;

inline void DrawMenu() {
    SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    if (Begin("GYMFLEX - NOTCH FIX", nullptr)) {
        Text("Sentuhan Presisi: AKTIF");
        Separator();
        static bool b = false; Checkbox("Hack Aktif", &b);
        End();
    }
}

inline void SetupImgui() {
    IMGUI_CHECKVERSION();
    CreateContext();
    ImGui_ImplAndroid_Init(nullptr);
    ImGui_ImplOpenGL3_Init("#version 300 es");
    GetStyle().ScaleAllSizes(3.5f);
}

inline EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);
inline EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    // 1. Ambil data Viewport Game (Termasuk Offset Poni)
    GLint v[4]; 
    glGetIntegerv(GL_VIEWPORT, v);
    
    // Simpan ke variabel global buat dipake di backend sentuhan
    g_VpX = (float)v[0]; g_VpY = (float)v[1];
    g_VpW = (float)v[2]; g_VpH = (float)v[3];

    if (g_VpW <= 0) return old_eglSwapBuffers(dpy, surface);

    if (!setupimg) { SetupImgui(); setupimg = true; }

    if (isSafeToDraw) {
        ImGuiIO &io = GetIO();
        // Paksa kordinat menu sesuai luas gambar game
        io.DisplaySize = ImVec2(g_VpW, g_VpH);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplAndroid_NewFrame();
        NewFrame();
        DrawMenu();
        Render();
        
        glViewport(v[0], v[1], v[2], v[3]);
        ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
    }
    return old_eglSwapBuffers(dpy, surface);
}
#endif
