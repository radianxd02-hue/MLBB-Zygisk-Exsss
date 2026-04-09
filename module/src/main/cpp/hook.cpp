#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <dlfcn.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android/input.h>
#include <android/log.h>

#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "Includes/Dobby/dobby.h"
#include "KittyMemory/KittyMemory.h"
#include "hook.h"
#include "Include/Roboto-Regular.h"

#define TargetLib "libanort.so"

// ✅ LOG TAG
#define LOG_TAG "IMGUI_TOUCH"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

bool setupimg = false;         
int glHeight = 0, glWidth = 0;               
bool isSafeToDraw = true;  

// ================= TOUCH =================
float touch_x = -1.0f;
float touch_y = -1.0f;
bool is_touch_down = false;

// ================= HOOK TOUCH =================
HOOKAF(int32_t, Consume, void *thiz, void *arg1, bool arg2, long arg3, uint32_t *arg4, AInputEvent **input_event) {
    int32_t result = origConsume(thiz, arg1, arg2, arg3, arg4, input_event);
    
    if (result == 0 && input_event && *input_event && setupimg) {
        AInputEvent* event = *input_event;

        if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {

            int32_t action = AMotionEvent_getAction(event);
            int32_t actionMasked = action & AMOTION_EVENT_ACTION_MASK;
            int32_t pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)
                                 >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;

            switch (actionMasked) {

                case AMOTION_EVENT_ACTION_DOWN:
                case AMOTION_EVENT_ACTION_POINTER_DOWN:
                case AMOTION_EVENT_ACTION_MOVE:
                {
                    float rawX = AMotionEvent_getRawX(event, pointerIndex);
                    float rawY = AMotionEvent_getRawY(event, pointerIndex);

                    float localX = AMotionEvent_getX(event, pointerIndex);
                    float localY = AMotionEvent_getY(event, pointerIndex);

                    touch_x = rawX;
                    touch_y = rawY;

                    is_touch_down = true;

                    // 🔥 LOG TOUCH
                    LOGD("RAW: %.2f %.2f | LOCAL: %.2f %.2f | DOWN: %d",
                         rawX, rawY, localX, localY, is_touch_down);
                    break;
                }

                case AMOTION_EVENT_ACTION_UP:
                case AMOTION_EVENT_ACTION_POINTER_UP:
                case AMOTION_EVENT_ACTION_CANCEL:
                {
                    is_touch_down = false;
                    touch_x = -1.0f;
                    touch_y = -1.0f;

                    LOGD("TOUCH RELEASE");
                    break;
                }
            }
        }
    }
    return result;
}

#include "menu.h"

// ================= THREAD =================
void *hack_thread(void *arg) {

    while (true) {
        auto base = KittyMemory::getLibraryBaseMap(TargetLib);
        if (base.isValid()) break;
        sleep(1);
    }

    void* egl = dlopen("libEGL.so", RTLD_NOW);
    void* eglSwapBuffers = egl ? dlsym(egl, "eglSwapBuffers") : nullptr;

    if (eglSwapBuffers) {
        DobbyHook(eglSwapBuffers, (void*)hook_eglSwapBuffers, (void**)&old_eglSwapBuffers);
    }

    void *sym = DobbySymbolResolver(
        "/system/lib64/libinput.so",
        "_ZN7android13InputConsumer7consumeEPNS_26InputEventFactoryInterfaceEblPjPPNS_10InputEventE"
    );

    if (sym) {
        DobbyHook(sym, (void*)myConsume, (void**)&origConsume);
    }

    while (true) sleep(9999);
    return nullptr;
}
