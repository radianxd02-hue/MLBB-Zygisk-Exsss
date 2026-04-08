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
// 🛡️ VARIABEL MANDIRI IMGUI (ANTI UNDEFINED) 🛡️
// =======================================================
// Hapus kata 'extern' karena variabel ini sekarang resmi milik hook.cpp
bool setupimg = false;         
int glHeight = 0;              
int glWidth = 0;               
bool isSafeToDraw = false;     // Saklar Room Filter

// =======================================================
// ⚙️ FUNGSI SAKLAR OTOMATIS (ROOM FILTER) ⚙️
// =======================================================

// 1. Saklar OFF (Keluar Room / Kembali ke Lobi)
void (*old_LeaveRoom)();
void hook_LeaveRoom() {
    isSafeToDraw = false; // Matikan ImGui
    LOGI("==== [Zygisk-Exsss] KELUAR ROOM: MENU DISAKUIN! ====");
    if (old_LeaveRoom) old_LeaveRoom();
}

// 2. Saklar ON 1 (Gabung ke Room)
void (*old_joinRoom)(void* thiz, void* callback, int iUpdate, int iRoomType, int sceneType);
void hook_joinRoom(void* thiz, void* callback, int iUpdate, int iRoomType, int sceneType) {
    isSafeToDraw = true; // Nyalakan ImGui
    LOGI("==== [Zygisk-Exsss] JOIN ROOM: MENU DIBUKA! ====");
    if (old_joinRoom) old_joinRoom(thiz, callback, iUpdate, iRoomType, sceneType);
}

// 3. Saklar ON 2 (Bikin Room Sendiri)
void (*old_CreateRoomDataAll)(void* thiz);
void hook_CreateRoomDataAll(void* thiz) {
    isSafeToDraw = true; // Nyalakan ImGui
    LOGI("==== [Zygisk-Exsss] CREATE ROOM: MENU DIBUKA! ====");
    if (old_CreateRoomDataAll) old_CreateRoomDataAll(thiz);
}

// =======================================================
// ⚙️ FUNGSI BAWAAN GAME ⚙️
// =======================================================

// Deklarasi game_data_dir (Biasanya ada di header, tapi kita pastikan aman di sini)
char* game_data_dir = nullptr;

int isGame(JNIEnv *env, jstring appDataDir) {
    if (!appDataDir) return 0;
    const char *app_data_dir = env->GetStringUTFChars(appDataDir, nullptr);
    int user = 0;
    static char package_name[256];
    if (sscanf(app_data_dir, "/data/%*[^/]/%d/%s", &user, package_name) != 2) {
        if (sscanf(app_data_dir, "/data/%*[^/]/%s", package_name) != 1) {
            package_name[0] = '\0';
            LOGW(OBFUSCATE("can't parse %s"), app_data_dir);
            return 0;
        }
    }
    if (strcmp(package_name, GamePackageName) == 0) {
        LOGI(OBFUSCATE("detect game: %s"), package_name);
        game_data_dir = new char[strlen(app_data_dir) + 1];
        strcpy(game_data_dir, app_data_dir);
        env->ReleaseStringUTFChars(appDataDir, app_data_dir);
        return 1;
    } else {
        env->ReleaseStringUTFChars(appDataDir, app_data_dir);
        return 0;
    }
}

HOOKAF(void, Input, void *thiz, void *ex_ab, void *ex_ac) {
    origInput(thiz, ex_ab, ex_ac);
    ImGui_ImplAndroid_HandleInputEvent((AInputEvent *)thiz);
    return;
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
// 🚀 INJEKSI UTAMA (THE THREAD) 🚀
// =======================================================
void *hack_thread(void *arg) {
    // 1. Waktu tunggu original Bos agar MLBB loading
    do {
        sleep(20);
        g_il2cppBaseMap = KittyMemory::getLibraryBaseMap("libil2cpp.so");
    } while (!g_il2cppBaseMap.isValid());
    
    KITTY_LOGI("il2cpp base: %p", (void*)(g_il2cppBaseMap.startAddress));
    
    // 2. Pasang cheat pointers & hooks asli
    Pointers();
    Hooks();

    // 3. PASANG SAKLAR OTOMATIS (ROOM FILTER)
    LOGI("==== [Zygisk-Exsss] MEMASANG SAKLAR OTOMATIS... ====");
    DobbyHook((void*)(g_il2cppBaseMap.startAddress + 0x81960D8), (void*)hook_LeaveRoom, (void**)&old_LeaveRoom);
    DobbyHook((void*)(g_il2cppBaseMap.startAddress + 0x6F834D4), (void*)hook_joinRoom, (void**)&old_joinRoom);
    DobbyHook((void*)(g_il2cppBaseMap.startAddress + 0x7EFAB58), (void*)hook_CreateRoomDataAll, (void**)&old_CreateRoomDataAll);

    // 4. Hook Render Engine (libunity.so)
    auto eglhandle = dlopen("libunity.so", RTLD_LAZY);
    if (eglhandle) {
        auto eglSwapBuffers = dlsym(eglhandle, "eglSwapBuffers");
        if (eglSwapBuffers) {
            DobbyHook((void*)eglSwapBuffers, (void*)hook_eglSwapBuffers, (void**)&old_eglSwapBuffers);
        }
    }

    // 5. Hook Sentuhan (libinput.so)
    void *sym_input = DobbySymbolResolver(("/system/lib/libinput.so"), ("_ZN7android13InputConsumer21initializeMotionEventEPNS_11MotionEventEPKNS_12InputMessageE"));
    if (NULL != sym_input) {
        DobbyHook(sym_input, (void*)myInput, (void**)&origInput);
    } else {
        sym_input = DobbySymbolResolver(("/system/lib/libinput.so"), ("_ZN7android13InputConsumer7consumeEPNS_26InputEventFactoryInterfaceEblPjPPNS_10InputEventE"));
        if(NULL != sym_input) {
            DobbyHook(sym_input, (void*)myConsume, (void**)&origConsume);
        }
    }
    
    LOGI("==== [Zygisk-Exsss] DRAW DONE! THREAD STANDBY! ====");

    // 6. Thread Abadi (Anti Crash)
    while (true) {
        sleep(9999);
    }
    
    return nullptr;
}
