#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <dlfcn.h>
#include <cstdlib>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android/input.h>

// Header ImGui Terbaru
#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_android.h"

#include "KittyMemory/KittyMemory.h"
#include "Includes/Dobby/dobby.h"
#include "hook.h"
#include "Include/Roboto-Regular.h"

#define TargetLib "libanort.so"

bool setupimg = false;         
bool isSafeToDraw = true;  

// =======================================================
// 🔌 HOOK SENTUHAN (BYPASS KE MESIN TERBARU)
// =======================================================
HOOKAF(int32_t, Consume, void *thiz, void *arg1, bool arg2, long arg3, uint32_t *arg4, AInputEvent **input_event) {
    int32_t result = origConsume(thiz, arg1, arg2, arg3, arg4, input_event);
    
    // 🛡️ ANTI-FC: Pastikan Context ImGui sudah nyala sebelum terima sentuhan
    if (result == 0 && input_event != nullptr && *input_event != nullptr) {
        if (setupimg && ImGui::GetCurrentContext() != nullptr) {
            ImGui_ImplAndroid_HandleInputEvent(*input_event);
        }
    }
    return result;
}


#include "functions.h"
#include "menu.h"

void *hack_thread(void *arg) {
    while (true) {
        if (KittyMemory::getLibraryBaseMap(TargetLib).isValid()) break; 
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

    return nullptr;
}
