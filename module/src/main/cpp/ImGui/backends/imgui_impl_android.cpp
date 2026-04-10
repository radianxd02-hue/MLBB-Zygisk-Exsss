#include "imgui.h"
#include "imgui_impl_android.h"
#include <time.h>
#include <android/native_window.h>
#include <android/input.h>

static double g_Time = 0.0;

// Variabel resolusi otomatis dari menu.h
extern float g_GameW;
extern float g_GameH;
extern float g_HardwareW;
extern float g_HardwareH;

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
            {
                // 1. Tangkap kordinat mentah dari Hardware HP
                float raw_x = AMotionEvent_getX(input_event, event_pointer_index);
                float raw_y = AMotionEvent_getY(input_event, event_pointer_index);

                // 2. 🔥 RUMUS KONVERSI OTOMATIS 🔥
                // Mengubah kordinat Hardware (misal 2400px) ke kordinat Game (misal 1600px)
                if (g_HardwareW > 0 && g_HardwareH > 0) {
                    io.MousePos.x = (raw_x / g_HardwareW) * g_GameW;
                    io.MousePos.y = (raw_y / g_HardwareH) * g_GameH;
                }

                if (event_action != AMOTION_EVENT_ACTION_MOVE) io.MouseDown[0] = true;
                return 1;
            }
            case AMOTION_EVENT_ACTION_UP:
            case AMOTION_EVENT_ACTION_POINTER_UP:
                io.MouseDown[0] = false;
                return 1;
        }
    }
    return 0;
}

bool ImGui_ImplAndroid_Init(ANativeWindow* window) { 
    g_Time = 0.0; 
    return true; 
}

void ImGui_ImplAndroid_Shutdown() {}

void ImGui_ImplAndroid_NewFrame() {
    ImGuiIO& io = ImGui::GetIO();
    struct timespec ts; 
    clock_gettime(CLOCK_MONOTONIC, &ts);
    double cur_time = (double)ts.tv_sec + (ts.tv_nsec / 1000000000.0);
    io.DeltaTime = g_Time > 0.0 ? (float)(cur_time - g_Time) : (float)(1.0f / 60.0f);
    g_Time = cur_time;
}
