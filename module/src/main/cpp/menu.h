#ifndef ZYGISK_MENU_TEMPLATE_MENU_H
#define ZYGISK_MENU_TEMPLATE_MENU_H

#include <EGL/egl.h>
#include <GLES2/gl2.h>
using namespace ImGui;

extern bool isSafeToDraw, setupimg;

inline void DrawMenu() {
    SetNextWindowSize(ImVec2(550, 400), ImGuiCond_FirstUseEver);
    if (Begin("GYMFLEX - 1:1 ABSOLUTE", nullptr)) {
        TextColored(ImVec4(0, 1, 0, 1), "Decoupled Mode: AKTIF!");
        Text("Sentuhan dijamin tidak akan meleset.");
        Separator();
        static bool dummy = false; 
        Checkbox("Tes Klik ESP", &dummy);
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
    // 1. Ambil ukuran fisik HP murni
    EGLint hw_w, hw_h;
    eglQuerySurface(dpy, surface, EGL_WIDTH, &hw_w);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &hw_h);

    // 2. SIMPAN VIEWPORT ASLI GAME (Biar game gak rusak)
    GLint original_viewport[4];
    glGetIntegerv(GL_VIEWPORT, original_viewport);

    if (hw_w <= 0 || hw_h <= 0) return old_eglSwapBuffers(dpy, surface);

    if (!setupimg) { SetupImgui(); setupimg = true; }

    if (isSafeToDraw) {
        ImGuiIO &io = GetIO();
        
        // 3. PAKSA ImGui berukuran sama persis dengan fisik HP (1:1 dgn touch)
        io.DisplaySize = ImVec2((float)hw_w, (float)hw_h);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplAndroid_NewFrame();
        NewFrame();
        DrawMenu();
        Render();
        
        glDisable(GL_SCISSOR_TEST);
        
        // 4. PAKSA OpenGL gambar ImGui seukuran layar fisik
        glViewport(0, 0, hw_w, hw_h);
        ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());

        // 5. KEMBALIKAN viewport game seperti semula!
        glViewport(original_viewport[0], original_viewport[1], original_viewport[2], original_viewport[3]);
    }
    
    return old_eglSwapBuffers(dpy, surface);
}
#endif
