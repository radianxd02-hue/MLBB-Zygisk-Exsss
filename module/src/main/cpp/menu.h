#ifndef ZYGISK_MENU_TEMPLATE_MENU_H
#define ZYGISK_MENU_TEMPLATE_MENU_H

#include <string>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

using namespace ImGui;

extern bool isSafeToDraw;
extern bool setupimg;

inline void DrawMenu()
{
    SetNextWindowSize(ImVec2(600, 450), ImGuiCond_FirstUseEver);
    if (Begin("GYMFLEX - HARDWARE SYNC 1:1", nullptr)) 
    {
        TextColored(ImVec4(0, 1, 0, 1), "Sinkronisasi Layar & Sentuhan Sempurna!");
        Separator();
        
        static bool dummy = false;
        Checkbox("Tes Klik Akurat", &dummy);
        
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
    // 1. TODONG UKURAN ASLI HARDWARE HP (BUKAN UKURAN GAME)
    EGLint hw_width = 0, hw_height = 0;
    eglQuerySurface(dpy, surface, EGL_WIDTH, &hw_width);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &hw_height);

    if (hw_width <= 0 || hw_height <= 0) {
        return old_eglSwapBuffers(dpy, surface);
    }

    if (!setupimg) {
        SetupImgui();
        setupimg = true;
    }

    if (isSafeToDraw) {
        ImGuiIO &io = GetIO();
        
        // 2. SAMAKAN KANVAS IMGUI DENGAN UKURAN SENTUHAN JARI
        io.DisplaySize = ImVec2((float)hw_width, (float)hw_height);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplAndroid_NewFrame();
        NewFrame();

        DrawMenu(); 

        EndFrame();
        Render();
        
        glDisable(GL_SCISSOR_TEST); 
        
        // 3. PAKSA GAMBAR IMGUI MENUHI SELURUH LAYAR HP (MENGABAIKAN KOMPRESI GAME)
        glViewport(0, 0, hw_width, hw_height); 
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    return old_eglSwapBuffers(dpy, surface);
}
#endif
