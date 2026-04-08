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
#include "Include/Quaternion.h"
#include "Rect.h"
#include <limits>

#define GamePackageName "com.mobile.legends"

// =======================================================
// 🛡️ VARIABEL MANDIRI IMGUI
// =======================================================
bool setupimg = false;         
int glHeight = 0, glWidth = 0;               
bool isSafeToDraw = true;  // <--- KITA SET TRUE (PAKSA NYALA) UNTUK TEST

// Kita matikan dulu fungsi saklar otomatis agar tidak ganggu testing
void (*old_LeaveRoom)();
void hook_LeaveRoom() { if (old_LeaveRoom) old_LeaveRoom(); }
void (*old_joinRoom)(void* thiz, void* callback, int iUpdate, int iRoomType, int sceneType);
void hook_joinRoom(void* thiz, void* callback, int iUpdate, int iRoomType, int sceneType) { if (old_joinRoom) old_joinRoom(thiz, callback, iUpdate, iRoomType, sceneType); }
void (*old_CreateRoomDataAll)(void* thiz);
void hook_CreateRoomDataAll(void* thiz) { if (old_CreateRoomDataAll) old_CreateRoomDataAll(thiz); }

// =======================================================
// ⚙️ FUNGSI DETEKSI GAME
// =======================================================
int isGame(JNIEnv *env, jstring appDataDir) {
    if (!appDataDir) return 0;
    const char *app_data_dir = env->GetStringUTFChars(appDataDir, nullptr);
    if (strstr(app_data_dir, GamePackageName)) {
        LOGI("==== [Zygisk-Exsss] GAME DETECTED! ====");
        env->ReleaseStringUTFChars(appDataDir, app_data_dir);
        return 1;
    }
    env->ReleaseStringUTFChars(appDataDir, app_data_dir);
    return 0;
}

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

#include "functions.h"
#include "menu.h"

// =======================================================
// 🚀 INJEKSI UTAMA
// =======================================================
void *hack_thread(void *arg) {
    LOGI("==== [Zygisk-Exsss] THREAD STARTED! WAITING 20S... ====");
    sleep(20);

    // Cari base address
    do {
        g_il2cppBaseMap = KittyMemory::getLibraryBaseMap("libil2cpp.so");
        sleep(1);
    } while (!g_il2cppBaseMap.isValid());
    
    KITTY_LOGI("il2cpp base: %p", (void*)(g_il2cppBaseMap.startAddress));
    
    // Aktifkan Cheat
    Pointers();
    Hooks();

    // 🛡️ HOOK RENDER ENGINE (PINDAH KE libEGL.so BIAR PASTI KETEMU)
    void* eglSwapBuffers = dlsym(RTLD_DEFAULT, "eglSwapBuffers");
    if (eglSwapBuffers) {
        LOGI("==== [Zygisk-Exsss] FOUND eglSwapBuffers! HOOKING... ====");
        DobbyHook(eglSwapBuffers, (void*)hook_eglSwapBuffers, (void**)&old_eglSwapBuffers);
    } else {
        LOGE("==== [Zygisk-Exsss] CANNOT FIND eglSwapBuffers! ====");
    }

    // Hook Sentuhan
    void *sym_input = DobbySymbolResolver(("/system/lib/libinput.so"), ("_ZN7android13InputConsumer21initializeMotionEventEPNS_11MotionEventEPKNS_12InputMessageE"));
    if (NULL != sym_input) {
        DobbyHook(sym_input, (void*)myInput, (void**)&origInput);
    } else {
        sym_input = DobbySymbolResolver(("/system/lib/libinput.so"), ("_ZN7android13InputConsumer7consumeEPNS_26InputEventFactoryInterfaceEblPjPPNS_10InputEventE"));
        if(NULL != sym_input) {
            DobbyHook(sym_input, (void*)myConsume, (void**)&origConsume);
        }
    }
    
    LOGI("==== [Zygisk-Exsss] SETUP COMPLETE! ====");
    while (true) { sleep(9999); }
    return nullptr;
}
