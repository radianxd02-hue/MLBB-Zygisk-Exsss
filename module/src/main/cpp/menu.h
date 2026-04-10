#ifndef ZYGISK_MENU_TEMPLATE_MENU_H
#define ZYGISK_MENU_TEMPLATE_MENU_H

#include <string>
using namespace ImGui;

extern bool isSafeToDraw;
extern bool setupimg;

inline void DrawMenu()
{
    static bool feature1 = false;
    
    SetNextWindowSize(ImVec2(500, 350), ImGuiCond_FirstUseEver);
    if (Begin("GYMFLEX - LATEST ENGINE", nullptr)) 
    {
        TextColored(ImVec4(0, 1, 0, 1), "ImGui v1.90+ : Android Mode!");
        Separator();
        
        Checkbox("Enable ESP", &feature1);
        
        if (Button("Tutup Menu", ImVec2(-1, 50))) {
            isSafeToDraw = false;
        }
        End();
    }
}

inline void SetupImgui() {
    IMGUI_CHECKVERSION();
    CreateContext();
    ImGuiIO &io = GetIO();
    
    // Inisialisasi Android & OpenGL (Pakai ES 3.0 Standard Mobile)
    ImGui_ImplAndroid_Init(nullptr); 
    ImGui_ImplOpenGL3_Init("#version 300 es");
    
    StyleColorsDark();
    GetStyle().ScaleAllSizes(3.5f);
    io.MouseDrawCursor = true; 
}

inline EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);

inline EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    // 🛡️ ANTI-FC: Ambil Viewport dengan aman
    GLint viewport[4] = {0, 0, 0, 0};
    glGetIntegerv(GL_VIEWPORT, viewport);

    // 🛡️ ANTI-FC: Kalau layar belum siap (masih 0x0), jangan gambar ImGui dulu!
    if (viewport[2] <= 0 || viewport[3] <= 0) {
        return old_eglSwapBuffers(dpy, surface);
    }

    if (!setupimg) {
        SetupImgui();
        setupimg = true;
    }

    if (isSafeToDraw) {
        ImGuiIO &io = GetIO();
        
        io.DisplaySize = ImVec2((float)viewport[2], (float)viewport[3]);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplAndroid_NewFrame();
        NewFrame();

        DrawMenu(); 

        EndFrame();
        Render();
        
        glDisable(GL_SCISSOR_TEST); 
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    return old_eglSwapBuffers(dpy, surface);
}
#endif
