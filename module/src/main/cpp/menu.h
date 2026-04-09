#ifndef ZYGISK_MENU_TEMPLATE_MENU_H
#define ZYGISK_MENU_TEMPLATE_MENU_H

#include <string>
using namespace ImGui;

extern bool isSafeToDraw;
extern bool setupimg;
extern int glWidth;
extern int glHeight;

inline void DrawMenu()
{
    static bool enableESP = false;
    static float recoilControl = 0.0f;

    Begin("GymFlex PUBG - Zygisk Injector", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    
    TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Status: Terhubung ke libanort.so");
    Separator();

    Checkbox("Enable ESP Box", &enableESP);
    SliderFloat("Less Recoil", &recoilControl, 0.0f, 100.0f, "%.0f%%");
    
    End();
}

inline void SetupImgui() {
    IMGUI_CHECKVERSION();
    CreateContext();
    ImGuiIO &io = GetIO();
    
    ImGui_ImplOpenGL3_Init("#version 100");
    StyleColorsDark();
    GetStyle().ScaleAllSizes(3.0f); 
    io.Fonts->AddFontFromMemoryTTF(Roboto_Regular, 30, 30.0f);
}

inline EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);

inline EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    eglQuerySurface(dpy, surface, EGL_WIDTH, &glWidth);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &glHeight);

    // Tukar jika terbaca Portrait
    if (glHeight > glWidth) {
        int temp = glWidth; glWidth = glHeight; glHeight = temp;
    }

    if (!setupimg) {
        SetupImgui();
        setupimg = true;
    }

    if (isSafeToDraw) {
        ImGuiIO &io = GetIO();
        
        // 1. Set Resolusi Layar
        io.DisplaySize = ImVec2((float)glWidth, (float)glHeight);

        // 🔥 KITA TIDAK LAGI MENGISI io.MousePos SECARA MANUAL DI SINI! 🔥
        // Semua sudah diurus otomatis oleh hook.cpp

        // 2. Mulai Frame Baru
        ImGui_ImplOpenGL3_NewFrame();
        NewFrame();

        // 3. Gambar Menu
        DrawMenu(); 

        // 4. Render
        EndFrame();
        Render();
        
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glDisable(GL_SCISSOR_TEST); 
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    return old_eglSwapBuffers(dpy, surface);
}

#endif //ZYGISK_MENU_TEMPLATE_MENU_H
