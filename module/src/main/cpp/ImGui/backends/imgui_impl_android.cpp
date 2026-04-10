#include "imgui.h"
#include "imgui_impl_android.h"
#include <time.h>
#include <android/input.h>
#include <android/native_window.h>

static double g_Time = 0.0;

// Tarik data ukuran game dari menu.h
extern float g_GameW;
extern float g_GameH;

// 🔥 RESOLUSI HARDCODE POCO X3 PRO 🔥
const float HP_PHYSICAL_WIDTH = 2400.0f;  
const float HP_PHYSICAL_HEIGHT = 1080.0f; 

int32_t ImGui_ImplAndroid_HandleInputEvent(const AInputEvent* input_event)
{
    ImGuiIO& io = ImGui::GetIO();
    if (AInputEvent_getType(input_event) != AINPUT_EVENT_TYPE_MOTION) return 0;

    int32_t action = AMotionEvent_getAction(input_event);
    int32_t index = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
    action &= AMOTION_EVENT_ACTION_MASK;

    if (action == AMOTION_EVENT_ACTION_DOWN || action == AMOTION_EVENT_ACTION_MOVE || action == AMOTION_EVENT_ACTION_POINTER_DOWN)
    {
        // 1. Ambil kordinat murni dari jari
        float raw_x = AMotionEvent_getX(input_event, index);
        float raw_y = AMotionEvent_getY(input_event, index);

        // 2. RUMUS SILANG PASTI (HARDCODE MAPPING)
        io.MousePos.x = (raw_x / HP_PHYSICAL_WIDTH) * g_GameW;
        io.MousePos.y = (raw_y / HP_PHYSICAL_HEIGHT) * g_GameH;

        if (action != AMOTION_EVENT_ACTION_MOVE) io.MouseDown[0] = true;
        return 1;
    }
    else if (action == AMOTION_EVENT_ACTION_UP || action == AMOTION_EVENT_ACTION_POINTER_UP || action == AMOTION_EVENT_ACTION_CANCEL)
    {
        io.MouseDown[0] = false;
        return 1;
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
