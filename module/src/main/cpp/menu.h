#ifndef ZYGISK_MENU_TEMPLATE_MENU_H
#define ZYGISK_MENU_TEMPLATE_MENU_H

#include <string>

using namespace ImGui;

extern bool isSafeToDraw;
extern bool setupimg;
extern int glWidth;
extern int glHeight;

// Variabel Penampung Kalibrasi (Penyelamat Poni HP)
static float touchOffsetX = 0.0f;
static float touchOffsetY = 0.0f;

inline void DrawMenu()
{
    static bool enableESP = false;
    static bool enableAimbot = false;
    static float recoilControl = 0.0f;
    static std::string scanStatus = "Belum di-scan";
    static uintptr_t GWorld = 0; 

    Begin("GymFlex PUBG - Zygisk Injector", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    
    TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Status: Terhubung ke libanort.so");
    Separator();

    Text("Status Peta: %s", scanStatus.c_str());
    if (GWorld != 0) {
        TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "GWorld: %p", (void*)GWorld);
    }

    if (Button("Scan Peta (GWorld)", ImVec2(200, 40))) {
        scanStatus = "Scanning memori... (Segera hadir)";
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

        // =======================================================
        // 🎯 TAB BARU: KALIBRASI SENTUHAN 
        // =======================================================
        if (BeginTabItem("⚙️ Kalibrasi")) {
            TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "Sentuhan Meleset? Sesuaikan di sini:");
            Text("Geser slider ini perlahan sampai klik-nya pas!");
            
            // Slider untuk menggeser respons sentuhan secara live
            SliderFloat("Geser Atas/Bawah", &touchOffsetY, -150.0f, 150.0f, "%.0f px");
            SliderFloat("Geser Kiri/Kanan", &touchOffsetX, -150.0f, 150.0f, "%.0f px");
            
            if (Button("Reset Kalibrasi", ImVec2(150, 35))) {
                touchOffsetX = 0.0f;
                touchOffsetY = 0.0f;
            }
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

    // Tetap pertahankan jurus pembasmi Portrait Bug!
    if (glHeight > glWidth) {
        int temp = glWidth;
        glWidth = glHeight;
        glHeight = temp;
    }

    if (!setupimg)
    {
        SetupImgui();
        setupimg = true;
    }

    if (isSafeToDraw) {
        ImGuiIO &io = GetIO();
        io.DisplaySize = ImVec2((float)glWidth, (float)glHeight);

        ImGui_ImplOpenGL3_NewFrame();
        NewFrame();

        // =======================================================
        // 💉 SUNTIKAN KALIBRASI: Nudge koordinat mouse secara ajaib
        // =======================================================
        io.MousePos.x += touchOffsetX;
        io.MousePos.y += touchOffsetY;

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
