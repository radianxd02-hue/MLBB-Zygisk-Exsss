#ifndef ZYGISK_MENU_TEMPLATE_MENU_H
#define ZYGISK_MENU_TEMPLATE_MENU_H

#include <string> // Wajib ditambah buat teks string

using namespace ImGui;

// =======================================================
// 📞 PANGGIL VARIABEL DARI HOOK.CPP (SANGAT PENTING)
// =======================================================
extern bool isSafeToDraw;
extern bool setupimg;
extern int glWidth;
extern int glHeight;

// =======================================================
// 🎨 DESAIN MENU GYMFLEX PUBG
// =======================================================
inline void DrawMenu()
{
    // Variabel statis untuk nampung status tombol (sementara sebelum disambung fungsi asli)
    static bool enableESP = false;
    static bool enableAimbot = false;
    static float recoilControl = 0.0f;
    static std::string scanStatus = "Belum di-scan";
    static uintptr_t GWorld = 0; // Dummy nilai GWorld

    // Warna background menu
    static ImVec4 clear_color = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);

    {
        // Ganti judul menu jadi GymFlex
        Begin("GymFlex PUBG - Zygisk Injector", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        
        TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Status: Terhubung ke libUE4.so");
        Separator();

        // --- BAGIAN SCANNER PETA ---
        Text("Status Peta: %s", scanStatus.c_str());
        if (GWorld != 0) {
            TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "GWorld: 0x%lX", GWorld);
        }

        if (Button("Scan Peta (GWorld)", ImVec2(200, 40))) {
            scanStatus = "Scanning memori... (Fitur segera hadir)";
            // Nanti kodingan KittyScanner buat nyari GWorld masuknya di sini
        }
        
        Separator();

        // --- BAGIAN FITUR CHEAT ---
        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_FittingPolicyResizeDown;
        if (BeginTabBar("MenuTabs", tab_bar_flags)) {
            
            // Tab 1: Visual (ESP)
            if (BeginTabItem("Visuals")) {
                Checkbox("Enable ESP Box", &enableESP);
                if (enableESP) {
                    TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "  -> Butuh GWorld Offset!");
                }
                EndTabItem();
            }

            // Tab 2: Aimbot & Gun
            if (BeginTabItem("Combat")) {
                Checkbox("Aimbot (Safe Mode)", &enableAimbot);
                SliderFloat("Less Recoil", &recoilControl, 0.0f, 100.0f, "%.0f%%");
                EndTabItem();
            }
            
            EndTabBar();
        }
        End();
    }
}

// =======================================================
// ⚙️ SETUP RENDERER IMGUI
// =======================================================
inline void SetupImgui() {
    IMGUI_CHECKVERSION();
    CreateContext();
    ImGuiIO &io = GetIO();
    io.DisplaySize = ImVec2((float) glWidth, (float) glHeight);
    ImGui_ImplOpenGL3_Init("#version 100");
    StyleColorsDark();
    
    // Scale menu biar gak kekecilan/kebesaran di layar HP
    GetStyle().ScaleAllSizes(3.0f); // Diturunin dari 7.0f biar gak menuhin layar
    
    // Pastikan font Roboto_Regular beneran ada di file 'Include/Roboto-Regular.h'
    io.Fonts->AddFontFromMemoryTTF(Roboto_Regular, 30, 30.0f);
}

// =======================================================
// 🖥️ HOOK EGLSWAPBUFFERS (JANTUNG NYA VISUAL)
// =======================================================
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
        ImGui_ImplOpenGL3_NewFrame();
        NewFrame();

        DrawMenu(); // Manggil desain menu GymFlex

        EndFrame();
        Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        
        glDisable(GL_SCISSOR_TEST); 
        
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    return old_eglSwapBuffers(dpy, surface);
}

#endif //ZYGISK_MENU_TEMPLATE_MENU_H
