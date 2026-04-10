#ifndef ZYGISK_MENU_TEMPLATE_MENU_H
#define ZYGISK_MENU_TEMPLATE_MENU_H

#include <string>
using namespace ImGui;

extern bool isSafeToDraw;
extern bool setupimg;

// Panggil variabel kalibrasi
extern float g_TouchOffsetX;
extern float g_TouchOffsetY;

inline void DrawMenu()
{
    static bool feature1 = false;
    
    SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    if (Begin("GYMFLEX - ZYGISK ENGINE", nullptr)) 
    {
        if (BeginTabBar("Tabs")) 
        {
            if (BeginTabItem(" MENU UTAMA ")) {
                Spacing();
                TextColored(ImVec4(0, 1, 0, 1), "Mesin Aktif & Anti-FC!");
                Checkbox("Enable ESP", &feature1);
                EndTabItem();
            }

            if (BeginTabItem(" ⚙️ KALIBRASI ")) {
                TextColored(ImVec4(1, 1, 0, 1), "LIHAT PANAH PUTIH DI LAYAR!");
                Text("Geser slider sampai panah nempel di jempol");
                
                Separator();
                SliderFloat("Geser X", &g_TouchOffsetX, -300.0f, 300.0f, "%.0f px");
                SliderFloat("Geser Y", &g_TouchOffsetY, -300.0f, 300.0f, "%.0f px");
                
                if (Button("RESET", ImVec2(-1, 40))) { g_TouchOffsetX = 0; g_TouchOffsetY = 0; }
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
    
    ImGui_ImplAndroid_Init(nullptr); 
    ImGui_ImplOpenGL3_Init("#version 100"); // 🛡️ ANTI-FC: Mode Aman (ES 2.0)
    
    StyleColorsDark();
    GetStyle().ScaleAllSizes(3.5f);
    io.MouseDrawCursor = true; // Nyalakan kursor panah
}

inline EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);

inline EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    // 🛡️ ANTI-FC: Cegah ImGui render kalau layar belum siap (masih loading)
    GLint viewport[4] = {0, 0, 0, 0};
    glGetIntegerv(GL_VIEWPORT, viewport);
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
