#ifndef ZYGISK_MENU_TEMPLATE_MENU_H
#define ZYGISK_MENU_TEMPLATE_MENU_H

#include <string>
using namespace ImGui;

extern bool isSafeToDraw;
extern bool setupimg;
extern int glWidth;
extern int glHeight;

// Panggil variabel offset dari hook
extern int v_offset_x;
extern int v_offset_y;

inline void ApplyPremiumStyle() {
    // (Biar kodenya gak kepanjangan di sini, style-nya pakai yang gelap standar aja dulu ya buat ngetes presisi)
    StyleColorsDark();
    GetStyle().WindowRounding = 8.0f;
    GetStyle().ScaleAllSizes(3.5f); 
}

inline void DrawMenu()
{
    static bool enableESP = false;
    static float recoilControl = 0.0f;

    SetNextWindowSize(ImVec2(550, 420), ImGuiCond_FirstUseEver);
    if (Begin("GYMFLEX PREMIER - PUBG MOBILE", nullptr, ImGuiWindowFlags_NoCollapse)) 
    {
        TextColored(ImVec4(0.15f, 0.75f, 0.15f, 1.00f), "Injected: libanort.so (UE4)");
        Separator();
        Checkbox("Enable ESP Box", &enableESP);
        SliderFloat("No Recoil", &recoilControl, 0.0f, 100.0f, "%.0f%%");
        if (Button("Unload Module", ImVec2(-1, 40))) isSafeToDraw = false;
        End();
    }
}

inline void SetupImgui() {
    IMGUI_CHECKVERSION();
    CreateContext();
    ImGuiIO &io = GetIO();
    ImGui_ImplOpenGL3_Init("#version 100");
    ApplyPremiumStyle();
    io.Fonts->AddFontFromMemoryTTF(Roboto_Regular, 30, 28.0f);
}

inline EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);

inline EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    if (!setupimg) {
        SetupImgui();
        setupimg = true;
    }

    if (isSafeToDraw) {
        ImGuiIO &io = GetIO();
        
        // 🎯 AMBIL KOORDINAT ASLI GAME
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        
        v_offset_x = viewport[0]; 
        v_offset_y = viewport[1];

        // Set ukuran kanvas
        io.DisplaySize = ImVec2((float)viewport[2], (float)viewport[3]);
        
        // 🔥 FITUR RAHASIA IMGUI: DISPLAY POS 🔥
        // Ini yang ngasih tahu ImGui kalau layar gamenya kegeser gara-gara poni!
        io.DisplayPos = ImVec2((float)viewport[0], (float)viewport[1]);

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
