#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <sys/system_properties.h>
#include <dlfcn.h>
#include <cstdlib>
#include <cinttypes>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android/input.h>

#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_android.h"
#include "KittyMemory/KittyMemory.h"
#include "KittyMemory/MemoryPatch.h"
#include "KittyMemory/KittyScanner.h"
#include "KittyMemory/KittyUtils.h"
#include "Includes/Dobby/dobby.h"
#include "Include/Unity.h" 
#include "Misc.h"
#include "hook.h"
#include "Include/Roboto-Regular.h"
#include <iostream>
#include <chrono>
#include "Rect.h"
#include <limits>

#define TargetLib "libanort.so"

bool setupimg = false;         
int glHeight = 0, glWidth = 0;               
bool isSafeToDraw = true;  

// =======================================================
// 👆 VARIABEL PENAMPUNG KOORDINAT JARI
// =======================================================
float touch_x = -1.0f;
float touch_y = -1.0f;
bool is_touch_down = false;

// =======================================================
// 👆 HOOK SENTUHAN AMAN & PRESISI
// =======================================================
HOOKAF(int32_t, Consume, void *thiz, void *arg1, bool arg2, long arg3, uint32_t *arg4, AInputEvent **input_event) {
    int32_t result = origConsume(thiz, arg1, arg2, arg3, arg4, input_event);
    
    // Tangkap koordinat asli jari dan simpan ke penampung
    if (result == 0 && input_event != nullptr && *input_event != nullptr && setupimg) {
        AInputEvent* event = *input_event;
        if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
            int32_t action = AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;
            if (action == AMOTION_EVENT_ACTION_DOWN || action == AMOTION_EVENT_ACTION_MOVE) {
                touch_x = AMotionEvent_getX(event, 0);
                touch_y = AMotionEvent_getY(event, 0);
                is_touch_down = true;
            } else if (action == AMOTION_EVENT_ACTION_UP || action == AMOTION_EVENT_ACTION_CANCEL) {
                is_touch_down = false;
            }
        }
    }
    return result;
}

#include "functions.h"
#include "menu.h"

// =======================================================
// 🚀 INJEKSI UTAMA ZYGISK
// =======================================================
void *hack_thread(void *arg) {
    LOGI("==== [GymFlex-PUBG] THREAD STARTED! Menunggu Game Load... ====");
    
    while (true) {
        auto ue4_map = KittyMemory::getLibraryBaseMap(TargetLib);
        if (ue4_map.isValid()) {
            KITTY_LOGI("==== [GymFlex-PUBG] libUE4.so DITEMUKAN PADA: %p ====", (void*)(ue4_map.startAddress));
            break; 
        }
        sleep(1);
    }
    
    Pointers();
    Hooks();

    void* egl_handle = dlopen("libEGL.so", RTLD_NOW);
    void* eglSwapBuffers = nullptr;
    if (egl_handle) {
        eglSwapBuffers = dlsym(egl_handle, "eglSwapBuffers");
    }

    if (eglSwapBuffers) {
        LOGI("==== [GymFlex-PUBG] FOUND eglSwapBuffers! HOOKING... ====");
        DobbyHook(eglSwapBuffers, (void*)hook_eglSwapBuffers, (void**)&old_eglSwapBuffers);
    } else {
        LOGE("==== [GymFlex-PUBG] CANNOT FIND eglSwapBuffers! ====");
    }

    dobby_enable_near_branch_trampoline();

    void *sym_consume = DobbySymbolResolver(("/system/lib64/libinput.so"), ("_ZN7android13InputConsumer7consumeEPNS_26InputEventFactoryInterfaceEblPjPPNS_10InputEventE"));
    if(NULL != sym_consume) {
        DobbyHook(sym_consume, (void*)myConsume, (void**)&origConsume);
        LOGI("==== [GymFlex-PUBG] Touch Hook (Consume) Sukses Aktif! ====");
    } else {
        LOGE("==== [GymFlex-PUBG] Touch Hook (Consume) GAGAL DITEMUKAN ====");
    }

    dobby_disable_near_branch_trampoline();

    LOGI("==== [GymFlex-PUBG] SETUP COMPLETE! INJECT SUKSES ====");
    while (true) { sleep(9999); }
    return nullptr;
}
