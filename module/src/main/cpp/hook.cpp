#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <sys/system_properties.h>
#include <dlfcn.h>
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
#include <fstream>
#include <limits>
#define GamePackageName "com.mobile.legends" // define the game package name here please

int glHeight, glWidth;

int isGame(JNIEnv *env, jstring appDataDir)
{
    if (!appDataDir)
        return 0;
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

bool setupimg;

HOOKAF(void, Input, void *thiz, void *ex_ab, void *ex_ac)
{
    origInput(thiz, ex_ab, ex_ac);
    ImGui_ImplAndroid_HandleInputEvent((AInputEvent *)thiz);
    return;
}

HOOKAF(int32_t, Consume, void *thiz, void *arg1, bool arg2, long arg3, uint32_t *arg4, AInputEvent **input_event)
{
    auto result = origConsume(thiz, arg1, arg2, arg3, arg4, input_event);
    if(result != 0 || *input_event == nullptr) return result;
    ImGui_ImplAndroid_HandleInputEvent(*input_event);
    return result;
}

#include "functions.h"
#include "menu.h"

void *hack_thread(void *arg) {
    LOGI("==== [Zygisk-Exsss] MEMULAI INJEKSI MODUL ====");

    // 1. Menunggu Game & Library IL2CPP
    do {
        LOGI("==== [Zygisk-Exsss] Sedang mencari libil2cpp.so... (Status: Menunggu) ====");
        sleep(20); // Sesuai permintaanmu tetap 20 detik
        g_il2cppBaseMap = KittyMemory::getLibraryBaseMap("libil2cpp.so");
    } while (!g_il2cppBaseMap.isValid());

    LOGI("==== [Zygisk-Exsss] libil2cpp TERDETEKSI! Base: %p ====", (void*)(g_il2cppBaseMap.startAddress));

    // 2. Inisialisasi Fitur
    Pointers();
    LOGI("==== [Zygisk-Exsss] Pointers Selesai Dipasang ====");
    
    Hooks();
    LOGI("==== [Zygisk-Exsss] Hooks Selesai Dipasang ====");

    // 3. Hook Rendering (EGL)
    LOGI("==== [Zygisk-Exsss] Mencoba Hook libunity.so (EGLSwapBuffers) ====");
    auto eglhandle = dlopen("libunity.so", RTLD_LAZY);
    if (eglhandle) {
        auto eglSwapBuffers = dlsym(eglhandle, "eglSwapBuffers");
        if (eglSwapBuffers) {
            DobbyHook((void*)eglSwapBuffers, (void*)hook_eglSwapBuffers, (void**)&old_eglSwapBuffers);
            LOGI("==== [Zygisk-Exsss] Hook Rendering BERHASIL! ====");
        } else {
            LOGE("==== [Zygisk-Exsss] ERROR: eglSwapBuffers TIDAK DITEMUKAN! ====");
        }
    } else {
        LOGE("==== [Zygisk-Exsss] ERROR: Gagal load libunity.so! ====");
    }

    // 4. Hook Input (Touch)
    LOGI("==== [Zygisk-Exsss] Mencoba Hook Input (Sentuhan) ====");
    void *sym_input = DobbySymbolResolver(("/system/lib/libinput.so"), ("_ZN7android13InputConsumer21initializeMotionEventEPNS_11MotionEventEPKNS_12InputMessageE"));
    if (NULL != sym_input) {
        LOGI("==== [Zygisk-Exsss] Menggunakan Metode Input A ====");
        DobbyHook(sym_input, (void*)myInput, (void**)&origInput);
    } else {
        LOGW("==== [Zygisk-Exsss] Metode A Gagal, Mencoba Metode B... ====");
        sym_input = DobbySymbolResolver(("/system/lib/libinput.so"), ("_ZN7android13InputConsumer7consumeEPNS_26InputEventFactoryInterfaceEblPjPPNS_10InputEventE"));
        if (NULL != sym_input) {
            DobbyHook(sym_input, (void *) myConsume, (void **) &origConsume);
            LOGI("==== [Zygisk-Exsss] Metode B BERHASIL! ====");
        } else {
            LOGE("==== [Zygisk-Exsss] SEMUA METODE INPUT GAGAL! ====");
        }
    }

    LOGI("==== [Zygisk-Exsss] === SELURUH PROSES SELESAI TANPA CRASH ====");
    LOGI("Draw Done!");
    return nullptr;
}
