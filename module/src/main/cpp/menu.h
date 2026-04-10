#ifndef ZYGISK_MENU_TEMPLATE_MENU_H
#define ZYGISK_MENU_TEMPLATE_MENU_H

#include <string>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

using namespace ImGui;

extern bool isSafeToDraw;
extern bool setupimg;

// Variabel Global untuk Rumus Auto-Scale
float g_GameW = 1.0f, g_GameH = 1.0f;
float g_HardwareW = 1.0f, g_HardwareH = 1.0f;

inline void DrawMenu()
{
    SetNextWindowSize(ImVec2(500, 350), ImGuiCond_FirstUseEver);
    if (Begin("GYMFLEX - AUTO SCALE", nullptr)) 
    {
        TextColored(ImVec4(0, 1, 0, 1), "Sistem Auto-Scale Aktif");
        Separator();
        
        static bool dummy = false;
        Checkbox("Tes Akurasi Klik", &dummy);
        
        if (Button("Tutup Menu", ImVec2(-1, 50))) {
            isSafeToDraw = false;
        }
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
    // 1. Ambil ukuran asli layar HP (Hardware)
    EGLint hw_width = 0, hw_height = 0;
    eglQuerySurface(dpy, surface, EGL_WIDTH, &hw_width);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &hw_height);

    // 2. Ambil ukuran render game PUBG (Viewport)
    GLint viewport[4] = {0, 0, 0, 0};
    glGetIntegerv(GL_VIEWPORT, viewport);

    if (viewport[2] <= 0 || viewport[3] <= 0 || hw_width <= 0 || hw_height <= 0) {
        return old_eglSwapBuffers(dpy, surface);
    }

    // 3. Simpan ke variabel global untuk rumus sentuhan
    g_HardwareW = (float)hw_width;
    g_HardwareH = (float)hw_height;
    g_GameW = (float)viewport[2];
    g_GameH = (float)viewport[3];

    if (!setupimg) {
        SetupImgui();
        setupimg = true;
    }

    if (isSafeToDraw) {
        ImGuiIO &io = GetIO();
        
        // Kanvas ImGui mengikuti ukuran game
        io.DisplaySize = ImVec2(g_GameW, g_GameH);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplAndroid_NewFrame();
        NewFrame();

        DrawMenu(); 

        EndFrame();
        Render();
        
        glDisable(GL_SCISSOR_TEST); 
        
        // Render mengikuti ukuran game
        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]); 
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    return old_eglSwapBuffers(dpy, surface);
}
#endif
