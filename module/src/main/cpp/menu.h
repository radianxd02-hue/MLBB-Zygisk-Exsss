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
    if (Begin("GYMFLEX - RESOLUSI 1:1", nullptr)) 
    {
        TextColored(ImVec4(0, 1, 0, 1), "Sentuhan 100% Akurat!");
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
    
    ImGui_ImplAndroid_Init(nullptr); 
    ImGui_ImplOpenGL3_Init("#version 300 es");
    
    StyleColorsDark();
    GetStyle().ScaleAllSizes(3.5f);
}

inline EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);

inline EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    if (!setupimg) {
        SetupImgui();
        setupimg = true;
    }

    if (isSafeToDraw) {
        ImGuiIO &io = GetIO();
        
        // =======================================================
        // 🎯 KUNCI PRESISI: AMBIL RESOLUSI HARDWARE MURNI (EGL)
        // =======================================================
        EGLint real_width = 0, real_height = 0;
        eglQuerySurface(dpy, surface, EGL_WIDTH, &real_width);
        eglQuerySurface(dpy, surface, EGL_HEIGHT, &real_height);

        // Jangan render ImGui kalau EGL belum siap
        if (real_width <= 0 || real_height <= 0) {
            return old_eglSwapBuffers(dpy, surface);
        }

        // Set kanvas ImGui persis dengan resolusi Hardware!
        io.DisplaySize = ImVec2((float)real_width, (float)real_height);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplAndroid_NewFrame();
        NewFrame();

        DrawMenu(); 

        EndFrame();
        Render();
        
        glDisable(GL_SCISSOR_TEST); 
        
        // Paksa OpenGL ngerender ImGui di ukuran resolusi hardware (Anti-Meleset)
        glViewport(0, 0, real_width, real_height);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    return old_eglSwapBuffers(dpy, surface);
}
#endif
