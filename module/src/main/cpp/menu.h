#ifndef ZYGISK_MENU_TEMPLATE_MENU_H
#define ZYGISK_MENU_TEMPLATE_MENU_H

#include <string>
using namespace ImGui;

extern bool isSafeToDraw;
extern bool setupimg;
extern int glWidth;
extern int glHeight;

inline void ApplyPremiumStyle() {
    ImGuiStyle& style = GetStyle();
    StyleColorsDark();
    style.WindowRounding = 8.0f;
    style.ScaleAllSizes(3.0f); // Standar HP
}

inline void DrawMenu()
{
    static bool enableESP = false;
    static float recoilControl = 0.0f;

    SetNextWindowSize(ImVec2(550, 420), ImGuiCond_FirstUseEver);

    if (Begin("GYMFLEX - CLEAN BASE", nullptr, ImGuiWindowFlags_NoCollapse)) 
    {
        TextColored(ImVec4(0.15f, 0.75f, 0.15f, 1.00f), "Status: Terhubung ke Mesin UE4");
        Separator();
        Spacing();

        if (BeginTabBar("MainTabs", ImGuiTabBarFlags_None)) 
        {
            if (BeginTabItem(" VISUALS ")) {
                Spacing();
                Checkbox("Enable ESP Box", &enableESP);
                EndTabItem();
            }

            if (BeginTabItem(" COMBAT ")) {
                Spacing();
                SliderFloat("No Recoil", &recoilControl, 0.0f, 100.0f, "%.0f%%");
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
    ImGui_ImplOpenGL3_Init("#version 100");
    ApplyPremiumStyle();
    
    // Fitur standar wajib
    ImGuiIO &io = GetIO();
    io.MouseDrawCursor = true; // Nyalakan untuk ngecek presisi!
}

inline EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);

inline EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    if (!setupimg) {
        SetupImgui();
        setupimg = true;
    }

    if (isSafeToDraw) {
        ImGuiIO &io = GetIO();
        
        // ========================================================
        // 🎯 KUNCI KESEMPURNAAN RENDER (TIDAK ADA MANIPULASI)
        // ========================================================
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        
        // Sinkronkan ukuran layar ImGui dengan Viewport Game secara absolut
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
