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

// =======================================================
// 🎯 TARGET PUBG
// =======================================================
#define TargetLib "libanort.so"

// =======================================================
// 🛡️ VARIABEL MANDIRI IMGUI
// =======================================================
bool setupimg = false;         
int glHeight = 0, glWidth = 0;               
bool isSafeToDraw = true;  

// Fungsi isGame() dan Hook abal-abal MLBB sudah dibuang ke tong sampah!

// =======================================================
// 👆 HOOK SENTUHAN (IM-GUI TOUCH)
// =======================================================
HOOKAF(void, Input, void *thiz, void *ex_ab, void *ex_ac) {
    origInput(thiz, ex_ab, ex_ac);
    ImGui_ImplAndroid_HandleInputEvent((AInputEvent *)thiz);
}

HOOKAF(int32_t, Consume, void *thiz, void *arg1, bool arg2, long arg3, uint32_t *arg4, AInputEvent **input_event) {
    auto result = origConsume(thiz, arg1, arg2, arg3, arg4, input_event);
    if(result != 0 || *input_event == nullptr) return result;
    ImGui_ImplAndroid_HandleInputEvent(*input_event);
    return result;
}

// PERHATIAN BREE: eglSwapBuffers harus ada di dalam salah satu file ini!
#include "functions.h"
#include "menu.h"

// =======================================================
// 🚀 INJEKSI UTAMA ZYGISK
// =======================================================
void *hack_thread(void *arg) {
    LOGI("==== [GymFlex-PUBG] THREAD STARTED! Menunggu Game Load... ====");
    
    // Looping pintar pakai 'auto' biar kompiler yang nebak sendiri tipe datanya
    while (true) {
        auto ue4_map = KittyMemory::getLibraryBaseMap(TargetLib);
        if (ue4_map.isValid()) {
            KITTY_LOGI("==== [GymFlex-PUBG] libUE4.so DITEMUKAN PADA: %p ====", (void*)(ue4_map.startAddress));
            break; // Stop looping kalau library udah ketemu
        }
        sleep(1);
    }
    
    // Panggil fungsi inisialisasi yang ada di menu.h / functions.h
    Pointers();
    Hooks();

    // 🛡️ HOOK RENDER ENGINE EGL
    void* egl_handle = dlopen("libEGL.so", RTLD_NOW);
    void* eglSwapBuffers = nullptr;
    if (egl_handle) {
        eglSwapBuffers = dlsym(egl_handle, "eglSwapBuffers");
    }
        LOGI("==== [GymFlex-PUBG] FOUND eglSwapBuffers! HOOKING... ====");
        DobbyHook(eglSwapBuffers, (void*)hook_eglSwapBuffers, (void**)&old_eglSwapBuffers);
    } else {
        LOGE("==== [GymFlex-PUBG] CANNOT FIND eglSwapBuffers! ====");
    }

        // 👆 HOOK SENTUHAN (KITA MATIKAN SEMENTARA BIAR GAK FC)
    void *sym_input = DobbySymbolResolver(("/system/lib64/libinput.so"), ("_ZN7android13InputConsumer21initializeMotionEventEPNS_11MotionEventEPKNS_12InputMessageE"));
    if (NULL != sym_input) {
        // DobbyHook(sym_input, (void*)myInput, (void**)&origInput); <--- MATIKAN INI
        LOGI("==== [GymFlex-PUBG] Touch Hook (Input) Dimatikan Sementara! ====");
    } else {
        sym_input = DobbySymbolResolver(("/system/lib64/libinput.so"), ("_ZN7android13InputConsumer7consumeEPNS_26InputEventFactoryInterfaceEblPjPPNS_10InputEventE"));
        if(NULL != sym_input) {
            // DobbyHook(sym_input, (void*)myConsume, (void**)&origConsume); <--- MATIKAN INI
            LOGI("==== [GymFlex-PUBG] Touch Hook (Consume) Dimatikan Sementara! ====");
        } else {
            LOGE("==== [GymFlex-PUBG] Touch Hook GAGAL DITEMUKAN ====");
        }
    }

    
    LOGI("==== [GymFlex-PUBG] SETUP COMPLETE! INJECT SUKSES ====");
    while (true) { sleep(9999); }
    return nullptr;
}
