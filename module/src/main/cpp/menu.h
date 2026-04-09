#ifndef ZYGISK_MENU_TEMPLATE_MENU_H
#define ZYGISK_MENU_TEMPLATE_MENU_H

#include <string>

using namespace ImGui;

extern bool isSafeToDraw;
extern bool setupimg;
extern int glWidth;
extern int glHeight;

// =======================================================
// 🎨 THEME & STYLING CUSTOM (DARK PREMIUM)
// =======================================================
inline void ApplyPremiumStyle() {
    ImGuiStyle& style = GetStyle();
    
    style.WindowRounding = 8.0f;
    style.ChildRounding = 6.0f;
    style.FrameRounding = 5.0f;
    style.PopupRounding = 6.0f;
    style.ScrollbarRounding = 9.0f;
    style.GrabRounding = 5.0f;
    style.TabRounding = 6.0f;

    style.WindowPadding = ImVec2(15, 15);
    style.FramePadding = ImVec2(10, 8);
    style.ItemSpacing = ImVec2(12, 10);

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.09f, 0.94f);
    colors[ImGuiCol_Header] = ImVec4(0.15f, 0.75f, 0.15f, 0.40f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.15f, 0.75f, 0.15f, 0.70f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.15f, 0.75f, 0.15f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.13f, 0.13f, 0.14f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.75f, 0.15f, 0.80f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.16f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.15f, 0.75f, 0.15f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.15f, 0.75f, 0.15f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.20f, 0.85f, 0.20f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.10f, 0.10f, 0.11f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.15f, 0.75f, 0.15f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.15f, 0.75f, 0.15f, 0.60f);
    colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
}

inline void DrawMenu()
{
    static bool enableESP = false;
    static bool enableLines = false;
    static bool enableAimbot = false;
    static float aimRange = 100.0f;
    static float recoilControl = 0.0f;
    static std::string scanStatus = "Standby";
    static uintptr_t GWorld = 0; 

    SetNextWindowSize(ImVec2(550, 420), ImGuiCond_FirstUseEver);

    if (Begin("GYMFLEX PREMIER - PUBG MOBILE", nullptr, ImGuiWindowFlags_NoCollapse)) 
    {
        TextColored(ImVec4(0.15f, 0.75f, 0.15f, 1.00f), "Injected: libanort.so (UE4 Engine)");
        SameLine(GetWindowWidth() - 120);
        TextDisabled("Build: v2.6.x");
        
        Separator();
        Spacing();

        if (BeginTabBar("MainTabs", ImGuiTabBarFlags_None)) 
        {
            if (BeginTabItem(" VISUALS ")) {
                Spacing();
                Checkbox("Enable ESP Box", &enableESP);
                Checkbox("Enable ESP Lines", &enableLines);
                
                Spacing();
                Separator();
                TextDisabled("Status GWorld:");
                if (GWorld == 0) {
                    TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Offline - Please Scan First");
                } else {
                    TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "Active: %p", (void*)GWorld);
                }

                if (Button("SCAN GWORLD OFFSET", ImVec2(-1, 45))) {
                    scanStatus = "Scanning...";
                }
                EndTabItem();
            }

            if (BeginTabItem(" COMBAT ")) {
                Spacing();
                Checkbox("Auto Aimbot", &enableAimbot);
                SliderFloat("Aim FOV", &aimRange, 10.0f, 360.0f, "%.1f");
                
                Spacing();
                Separator();
                Text("Weapon Modification");
                SliderFloat("No Recoil", &recoilControl, 0.0f, 100.0f, "%.0f%%");
                EndTabItem();
            }

            if (BeginTabItem(" MISC ")) {
                Spacing();
                if (Button("Unload Module", ImVec2(-1, 40))) {
                    isSafeToDraw = false;
                }
                TextDisabled("Device Info: Android 64-bit");
                TextDisabled("Mode: Zygisk Injection");
                EndTabItem();
            }
            EndTabBar();
        }
        
        Spacing();
        Separator();
        TextDisabled("GymFlex Apparel Project @ 2026");
        
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
        
        // ============================================================
        // 🎯 KUNCI PRESISI: AMBIL UKURAN LAYAR ASLI DARI GAME!
        // ============================================================
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        
        // viewport[2] = Lebar game asli, viewport[3] = Tinggi game asli
        io.DisplaySize = ImVec2((float)viewport[2], (float)viewport[3]);

        ImGui_ImplOpenGL3_NewFrame();
        NewFrame();

        DrawMenu(); 

        EndFrame();
        Render();
        
        // 🚫 DILARANG KERAS MENGUBAH GLVIEWPORT DI SINI! 🚫
        // glViewport(0, 0, ...) -> Kodingan ini yang dulu bikin meleset!
        
        glDisable(GL_SCISSOR_TEST); 
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    return old_eglSwapBuffers(dpy, surface);
}

#endif
