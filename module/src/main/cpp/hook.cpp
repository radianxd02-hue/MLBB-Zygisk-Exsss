#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <dlfcn.h>
#include <cstdlib>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android/input.h>

#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_android.h"
#include "KittyMemory/KittyMemory.h"
#include "KittyMemory/KittyScanner.h"
#include "KittyMemory/KittyUtils.h"
#include "Includes/Dobby/dobby.h"
#include "Include/Unity.h" 
#include "Misc.h"
#include "hook.h"
#include "Include/Roboto-Regular.h"

#define TargetLib "libanort.so"

// Variabel Global Esensial
bool setupimg = false;         
int glHeight = 0, glWidth = 0;               
bool isSafeToDraw = true;  

// =======================================================
// 👆 HOOK SENTUHAN (BYPASS TOTAL KE IMGUI)
// =======================================================
HOOKAF(int32_t, Consume, void *thiz, void *arg1, bool arg2, long arg3, uint32_t *arg4, AInputEvent **input_event) {
    int32_t result = origConsume(thiz, arg1, arg2, arg3, arg4, input_event);
    
    // Jangan manipulasi apapun! Biarkan ImGui asli yang memprosesnya
    if (result == 0 && input_event != nullptr && *input_event != nullptr && setupimg) {
        ImGui_ImplAndroid_HandleInputEvent(*input_event);
    }
    return result;
}

#include "functions.h"
#include "menu.h"

// =======================================================
// 🚀 THREAD UTAMA ZYGISK
// =======================================================
void *hack_thread(void *arg) {
    LOGI("==== [GymFlex] MENUNGGU GAME ====");
    
    while (true) {
        if (KittyMemory::getLibraryBaseMap(TargetLib).isValid()) { break; }
        sleep(1);
    }
    
    Pointers();
    Hooks();

    void* egl_handle = dlopen("libEGL.so", RTLD_NOW);
    void* eglSwapBuffers = egl_handle ? dlsym(egl_handle, "eglSwapBuffers") : nullptr;

    if (eglSwapBuffers) {
        DobbyHook(eglSwapBuffers, (void*)hook_eglSwapBuffers, (void**)&old_eglSwapBuffers);
    }

    dobby_enable_near_branch_trampoline();
    void *sym_consume = DobbySymbolResolver(("/system/lib64/libinput.so"), ("_ZN7android13InputConsumer7consumeEPNS_26InputEventFactoryInterfaceEblPjPPNS_10InputEventE"));
    if(sym_consume) {
        DobbyHook(sym_consume, (void*)myConsume, (void**)&origConsume);
    }
    dobby_disable_near_branch_trampoline();

    LOGI("==== [GymFlex] INJECT SUKSES ====");
    while (true) { sleep(9999); }
    return nullptr;
}
