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
    if (Begin("GYMFLEX - CLEAN ENGINE", nullptr)) 
    {
        TextColored(ImVec4(0, 1, 0, 1), "ImGui v1.90+ : Posisi Sinkron!");
        Separator();
        
        Checkbox("Enable ESP", &feature1);
        
        if (Button("Unload Menu", ImVec2(-1, 50))) {
            isSafeToDraw = false;
        }
        End();
    }
}

inline void SetupImgui() {
    IMGUI_CHECKVERSION();
    CreateContext();
    ImGuiIO &io = GetIO();
    
    // Inisialisasi Kabel Backend
    ImGui_ImplAndroid_Init(nullptr); 
    ImGui_ImplOpenGL3_Init("#version 100");
    
    StyleColorsDark();
    GetStyle().ScaleAllSizes(3.5f);
    
    // Aktifkan kursor untuk mempermudah pengecekan
    io.MouseDrawCursor = true; 
}

inline EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);

inline EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    if (!setupimg) {
        SetupImgui();
        setupimg = true;
    }

    if (isSafeToDraw) {
        ImGuiIO &io = GetIO();
        
        // 🎯 AMBIL TITIK KOORDINAT ASLI GAME (Viewport)
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        
        // Samakan ukuran kanvas ImGui dengan area gambar game
        io.DisplaySize = ImVec2((float)viewport[2], (float)viewport[3]);

        // Frame Baru
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
