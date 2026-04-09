#ifndef ZYGISK_MENU_TEMPLATE_MENU_H
#define ZYGISK_MENU_TEMPLATE_MENU_H

#include <string>

using namespace ImGui;

extern bool isSafeToDraw;
extern bool setupimg;
extern int glWidth;
extern int glHeight;

// Panggil penampung jari dari hook.cpp
extern float touch_x;
extern float touch_y;
extern bool is_touch_down;

inline void DrawMenu()
{
    static bool enableESP = false;
    static bool enableAimbot = false;
    static float recoilControl = 0.0f;
    static std::string scanStatus = "Belum di-scan";
    static uintptr_t GWorld = 0; 

    static ImVec4 clear_color = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);

    Begin("GymFlex PUBG - Zygisk Injector", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    
    TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Status: Terhubung ke libanort.so");
    Separator();

    Text("Status Peta: %s", scanStatus.c_str());
    if (GWorld != 0) {
        TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "GWorld: %p", (void*)GWorld);
    }

    if (Button("Scan Peta (GWorld)", ImVec2(200, 40))) {
        scanStatus = "Scanning memori... (Fitur segera hadir)";
    }
    
    Separator();

    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_FittingPolicyResizeDown;
    if (BeginTabBar("MenuTabs", tab_bar_flags)) {
        if (BeginTabItem("Visuals")) {
            Checkbox("Enable ESP Box", &enableESP);
            if (enableESP) {
                TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "  -> Butuh GWorld Offset!");
            }
            EndTabItem();
        }

        if (BeginTabItem("Combat")) {
            Checkbox("Aimbot (Safe Mode)", &enableAimbot);
            SliderFloat("Less Recoil", &recoilControl, 0.0f, 100.0f, "%.0f%%");
            EndTabItem();
        }
        EndTabBar();
    }
    End();
}

inline void SetupImgui() {
    IMGUI_CHECKVERSION();
    CreateContext();
    ImGuiIO &io = GetIO();
    io.DisplaySize = ImVec2((float) glWidth, (float) glHeight);
    ImGui_ImplOpenGL3_Init("#version 100");
    StyleColorsDark();
    GetStyle().ScaleAllSizes(3.0f); 
    io.Fonts->AddFontFromMemoryTTF(Roboto_Regular, 30, 30.0f);
}

inline EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);

inline EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    eglQuerySurface(dpy, surface, EGL_WIDTH, &glWidth);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &glHeight);

    if (!setupimg)
    {
        SetupImgui();
        setupimg = true;
    }

    if (isSafeToDraw) {
        ImGuiIO &io = GetIO();
        
        // ======================================================
        // 🎯 INJEKSI JARUM SUNTIK PRESISI 100%
        // ======================================================
        if (touch_x >= 0.0f && touch_y >= 0.0f) {
            io.MousePos = ImVec2(touch_x, touch_y);
        }
        io.MouseDown[0] = is_touch_down;
        // ======================================================

        ImGui_ImplOpenGL3_NewFrame();
        NewFrame();

        DrawMenu(); 

        EndFrame();
        Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glDisable(GL_SCISSOR_TEST); 
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    return old_eglSwapBuffers(dpy, surface);
}

#endif //ZYGISK_MENU_TEMPLATE_MENU_H
