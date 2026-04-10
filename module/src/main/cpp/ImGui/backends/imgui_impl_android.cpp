#include "imgui.h"
#include "imgui_impl_android.h"
#include <time.h>
#include <android/native_window.h>
#include <android/input.h>

static double g_Time = 0.0;

// Variabel untuk fitur Kalibrasi Slider
extern float g_TouchOffsetX;
extern float g_TouchOffsetY;

int32_t ImGui_ImplAndroid_HandleInputEvent(const AInputEvent* input_event)
{
    ImGuiIO& io = ImGui::GetIO();
    int32_t event_type = AInputEvent_getType(input_event);

    if (event_type == AINPUT_EVENT_TYPE_MOTION)
    {
        int32_t event_action = AMotionEvent_getAction(input_event);
        int32_t event_pointer_index = (event_action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
        event_action &= AMOTION_EVENT_ACTION_MASK;

        switch (event_action)
        {
            case AMOTION_EVENT_ACTION_DOWN:
            case AMOTION_EVENT_ACTION_POINTER_DOWN:
            case AMOTION_EVENT_ACTION_MOVE:
                // 🚀 Titik temu sentuhan dan slider kalibrasi
                io.MousePos = ImVec2(
                    AMotionEvent_getX(input_event, event_pointer_index) + g_TouchOffsetX, 
                    AMotionEvent_getY(input_event, event_pointer_index) + g_TouchOffsetY
                );
                if (event_action != AMOTION_EVENT_ACTION_MOVE) io.MouseDown[0] = true;
                return 1;
            case AMOTION_EVENT_ACTION_UP:
            case AMOTION_EVENT_ACTION_POINTER_UP:
                io.MouseDown[0] = false;
                return 1;
        }
    }
    return 0;
}

// 🛡️ ANTI-FC: Bypass inisialisasi window bawaan pabrik yang sering bikin crash
bool ImGui_ImplAndroid_Init(ANativeWindow* window)
{
    g_Time = 0.0;
    ImGuiIO& io = ImGui::GetIO();
    io.BackendPlatformName = "imgui_impl_android_zygisk";
    return true;
}

void ImGui_ImplAndroid_Shutdown()
{
    ImGuiIO& io = ImGui::GetIO();
    io.BackendPlatformName = nullptr;
}

void ImGui_ImplAndroid_NewFrame()
{
    ImGuiIO& io = ImGui::GetIO();
    struct timespec current_timespec;
    clock_gettime(CLOCK_MONOTONIC, &current_timespec);
    double current_time = (double)(current_timespec.tv_sec) + (current_timespec.tv_nsec / 1000000000.0);
    io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f / 60.0f);
    g_Time = current_time;
}
