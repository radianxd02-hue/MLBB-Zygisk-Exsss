#include "imgui.h"
#ifndef IMGUI_DISABLE
#include "imgui_impl_android.h"
#include <time.h>
#include <android/native_window.h>
#include <android/input.h>
#include <android/keycodes.h>

// Android data
static double g_Time = 0.0;
static ANativeWindow* g_Window;

// =============================
// ❌ DISABLE TOTAL INPUT ANDROID
// =============================
int32_t ImGui_ImplAndroid_HandleInputEvent(const AInputEvent* input_event)
{
    // 🔥 FIX UTAMA: MATIKAN INPUT BACKEND
    // Supaya tidak bentrok dengan hook.cpp (Consume)
    return 0;
}

// =============================
// INIT
// =============================
bool ImGui_ImplAndroid_Init(ANativeWindow* window)
{
    g_Window = window;
    g_Time = 0.0;

    ImGuiIO& io = ImGui::GetIO();
    io.BackendPlatformName = "imgui_impl_android";

    return true;
}

// =============================
// SHUTDOWN
// =============================
void ImGui_ImplAndroid_Shutdown()
{
    ImGuiIO& io = ImGui::GetIO();
    io.BackendPlatformName = nullptr;
}

// =============================
// NEW FRAME
// =============================
void ImGui_ImplAndroid_NewFrame()
{
    ImGuiIO& io = ImGui::GetIO();

    // Ambil ukuran window asli
    int32_t window_width = ANativeWindow_getWidth(g_Window);
    int32_t window_height = ANativeWindow_getHeight(g_Window);

    io.DisplaySize = ImVec2((float)window_width, (float)window_height);

    // Frame time
    struct timespec current_timespec;
    clock_gettime(CLOCK_MONOTONIC, &current_timespec);

    double current_time = (double)(current_timespec.tv_sec)
                        + (current_timespec.tv_nsec / 1000000000.0);

    io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time)
                               : (float)(1.0f / 60.0f);

    g_Time = current_time;
}

#endif // IMGUI_DISABLE
