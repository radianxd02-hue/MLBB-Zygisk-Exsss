#ifndef ZYGISK_MENU_TEMPLATE_MENU_H
#define ZYGISK_MENU_TEMPLATE_MENU_H

#include "imgui.h"

extern bool isSafeToDraw;
extern bool setupimg;
extern int glWidth;
extern int glHeight;

// =======================================================
// ⚙️ SETUP RENDERER IMGUI (POLOSAN)
// =======================================================
inline void SetupImgui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    
    // Inisialisasi OpenGL murni
    ImGui_ImplOpenGL3_Init("#version 100");
    
    // Tema gelap bawaan ImGui
    ImGui::StyleColorsDark();
    
    // Perbesar skala biar gampang dipencet pakai jari
    ImGui::GetStyle().ScaleAllSizes(3.0f); 
    
    // Pakai font bawaan mesin aja, gak usah load Roboto dulu
    io.Fonts->AddFontDefault(); 
}

// =======================================================
// 🖥️ HOOK EGLSWAPBUFFERS 
// =======================================================
inline EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);

inline EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    // Selalu ambil resolusi ter-update setiap frame
    eglQuerySurface(dpy, surface, EGL_WIDTH, &glWidth);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &glHeight);

    if (!setupimg)
    {
        SetupImgui();
        setupimg = true;
    }

    if (isSafeToDraw) {
        ImGuiIO &io = ImGui::GetIO();
        
        // Update Display Size
        io.DisplaySize = ImVec2((float)glWidth, (float)glHeight);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        // =======================================================
        // 🔥 TAMPILKAN JENDELA DEMO ORIGINAL IMGUI 🔥
        // =======================================================
        static bool show_demo_window = true;
        if (show_demo_window) {
            ImGui::ShowDemoWindow(&show_demo_window);
        }

        ImGui::EndFrame();
        ImGui::Render();
        
        // Render dengan viewport full ngikutin glWidth & glHeight
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glDisable(GL_SCISSOR_TEST); 
        
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    return old_eglSwapBuffers(dpy, surface);
}

#endif //ZYGISK_MENU_TEMPLATE_MENU_H
