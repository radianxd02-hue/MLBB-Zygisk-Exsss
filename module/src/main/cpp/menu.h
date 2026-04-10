#ifndef ZYGISK_MENU_TEMPLATE_MENU_H
#define ZYGISK_MENU_TEMPLATE_MENU_H

#include <string>
using namespace ImGui;

extern bool isSafeToDraw;
extern bool setupimg;

inline void DrawMenu()
{
    SetNextWindowSize(ImVec2(500, 350), ImGuiCond_FirstUseEver);
    if (Begin("GYMFLEX - RAW GAME SIZE", nullptr)) 
    {
        TextColored(ImVec4(1, 0, 0, 1), "Persetan dengan poni!");
        Text("Menu ini murni 100% ngikut ukuran game.");
        Separator();
        
        static bool dummy = false;
        Checkbox("Tes Klik ESP", &dummy);
        
        if (Button("Tutup Menu", ImVec2(-1, 50))) {
            isSafeToDraw = false;
        }
        End();
    }
}

inline void SetupImgui() {
    IMGUI_CHECKVERSION();
    CreateContext();
    ImGui_ImplAndroid_Init(nullptr); 
    ImGui_ImplOpenGL3_Init("#version 300 es"); // Mode aman
    
    StyleColorsDark();
    GetStyle().ScaleAllSizes(3.5f); // Besarin UI biar enak dipencet
}

inline EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);

inline EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    // 1. Todong mesin game: "Lagi gambar di ukuran berapa lu sekarang?"
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    // 2. Kalau game belum siap gambar, jangan nongol dulu
    if (viewport[2] <= 0 || viewport[3] <= 0) {
        return old_eglSwapBuffers(dpy, surface);
    }

    if (!setupimg) {
        SetupImgui();
        setupimg = true;
    }

    if (isSafeToDraw) {
        ImGuiIO &io = GetIO();
        
        // 3. JEPLAK MURNI UKURAN GAME (Nggak peduli hardware)
        io.DisplaySize = ImVec2((float)viewport[2], (float)viewport[3]);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplAndroid_NewFrame();
        NewFrame();

        DrawMenu(); 

        EndFrame();
        Render();
        
        glDisable(GL_SCISSOR_TEST); 
        
        // 4. Paksa ImGui menggambar di area yang SAMA PERSIS dengan game
        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]); 
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    return old_eglSwapBuffers(dpy, surface);
}
#endif
