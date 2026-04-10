#ifndef ZYGISK_MENU_TEMPLATE_MENU_H
#define ZYGISK_MENU_TEMPLATE_MENU_H

#include <string>

using namespace ImGui;

extern bool isSafeToDraw;
extern bool setupimg;
extern int glWidth;
extern int glHeight;

// Panggil variabel kalibrasi dari hook.cpp
extern float g_TouchOffsetX;
extern float g_TouchOffsetY;

inline void ApplyPremiumStyle() {
    ImGuiStyle& style = GetStyle();
    style.WindowRounding = 8.0f;
    style.ChildRounding = 6.0f;
    style.FrameRounding = 5.0f;
    style.GrabRounding = 5.0f;
    style.TabRounding = 6.0f;
    style.WindowPadding = ImVec2(15, 15);
    
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.09f, 0.94f);
    colors[ImGuiCol_Header] = ImVec4(0.15f, 0.75f, 0.15f, 0.40f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.75f, 0.15f, 0.80f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.15f, 0.75f, 0.15f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.15f, 0.75f, 0.15f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.15f, 0.75f, 0.15f, 1.00f);
}

inline void DrawMenu()
{
    static bool enableESP = false;
    static float recoilControl = 0.0f;

    SetNextWindowSize(ImVec2(550, 420), ImGuiCond_FirstUseEver);

    if (Begin("GYMFLEX PREMIER - PUBG MOBILE", nullptr, ImGuiWindowFlags_NoCollapse)) 
    {
        if (BeginTabBar("MainTabs", ImGuiTabBarFlags_None)) 
        {
            if (BeginTabItem(" VISUALS ")) {
                Spacing();
                Checkbox("Enable ESP Box", &enableESP);
                SliderFloat("No Recoil", &recoilControl, 0.0f, 100.0f, "%.0f%%");
                EndTabItem();
            }

            // ========================================================
            // ⚙️ TAB KALIBRASI (SANG PENYELAMAT)
            // ========================================================
            if (BeginTabItem(" ⚙️ KALIBRASI ")) {
                Spacing();
                TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "Meleset? Lihat Kursor Panah Putih!");
                TextDisabled("Geser panah ke slider ini, lalu atur sampai pas.");
                
                Spacing();
                // Kalau menu kurang ke bawah, atur slider Y ini ke arah minus
                SliderFloat("Geser Atas/Bawah (Y)", &g_TouchOffsetY, -200.0f, 200.0f, "%.0f px");
                SliderFloat("Geser Kiri/Kanan (X)", &g_TouchOffsetX, -200.0f, 200.0f, "%.0f px");
                
                Spacing();
                if (Button("Reset Kalibrasi", ImVec2(-1, 40))) {
                    g_TouchOffsetX = 0.0f; g_TouchOffsetY = 0.0f;
                }
                EndTabItem();
            }
            EndTabBar();
        }
        End();
    }
}

inline void SetupImgui() {
    IMGUI_CHECKVERSION();
    CreateContext();
    ImGuiIO &io = GetIO();
    ImGui_ImplOpenGL3_Init("#version 100");
    ApplyPremiumStyle();
    GetStyle().ScaleAllSizes(3.5f); 

    // 🔥 JURUS MATA DEWA: TAMPILKAN KURSOR PANAH! 🔥
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
        
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        
        // Ukuran kanvas ngikutin EGL
        io.DisplaySize = ImVec2((float)viewport[2], (float)viewport[3]);

        ImGui_ImplOpenGL3_NewFrame();
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
