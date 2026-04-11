#ifndef ZYGISK_MENU_TEMPLATE_MENU_H
#define ZYGISK_MENU_TEMPLATE_MENU_H

#include <EGL/egl.h>
#include <GLES2/gl2.h>
using namespace ImGui;

extern bool isSafeToDraw;
extern bool setupimg;

// Deklarasi fungsi writeGameMemory dari hook.cpp
extern bool writeGameMemory(unsigned long address, void* value, size_t size);

// Pakai 'inline' agar C++20 tidak error multiple definition
inline float g_GameW = 1.0f;
inline float g_GameH = 1.0f;

inline void DrawMenu() {
    SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    if (Begin("GYMFLEX - POCO X3 PRO", nullptr)) {
        TextColored(ImVec4(0, 1, 0, 1), "Hardware Locked: 2400x1080");
        TextColored(ImVec4(1, 1, 0, 1), "MemKernel: /dev/skietm Ready");
        Separator();
        
        // ========== FITUR MEMKERNEL ==========
        static int godModeValue = 9999;
        SliderInt("God Mode Value", &godModeValue, 1, 99999);
        
        if (Button("ACTIVATE GOD MODE")) {
            // Ganti 0x7922A4C8 dengan offset Health di game target
            if (writeGameMemory(0x7922A4C8, &godModeValue, sizeof(godModeValue))) {
                // Sukses - bisa tambahin notifikasi kalau mau
            } else {
                // Gagal - bisa tambahin notifikasi kalau mau
            }
        }
        Separator();
        // ========== AKHIR FITUR MEMKERNEL ==========
        
        static bool esp_test = false; 
        Checkbox("Tes Klik ESP", &esp_test);
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
    // Ambil resolusi kompresan dari engine game
    GLint v[4]; 
    glGetIntegerv(GL_VIEWPORT, v);

    if (v[2] > 0 && v[3] > 0) {
        g_GameW = (float)v[2]; 
        g_GameH = (float)v[3];
    }

    if (!setupimg) { 
        SetupImgui(); 
        setupimg = true; 
    }

    if (isSafeToDraw) {
        ImGuiIO &io = GetIO();
        
        // Kanvas ImGui mengikuti ukuran asli game
        io.DisplaySize = ImVec2(g_GameW, g_GameH);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplAndroid_NewFrame();
        NewFrame();
        
        DrawMenu();
        
        Render();
        
        glDisable(GL_SCISSOR_TEST);
        glViewport(v[0], v[1], v[2], v[3]);
        ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
    }
    return old_eglSwapBuffers(dpy, surface);
}
#endif
