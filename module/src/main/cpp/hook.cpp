#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <dlfcn.h>
#include <cstdlib>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android/input.h>
#include <android/log.h>

// ========== TAMBAHAN UNTUK MEMKERNEL ==========
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <cerrno>
#include <vector>

#define LOG_TAG "MemKernel"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Kode perintah untuk menulis memori (sesuaikan dengan driver MemKernel)
#define IOCTL_WRITE_MEM 0x4000

// Fungsi untuk mengirim perintah tulis ke MemKernel (Ring 0)
bool writeGameMemory(unsigned long address, void* value, size_t size) {
    int fd = open("/dev/skietm", O_RDWR);
    if (fd < 0) {
        LOGE("Gagal membuka /dev/skietm. Apakah modul sudah diinsmod? (errno: %d)", errno);
        return false;
    }
    
    // Alokasi buffer dinamis untuk data
    std::vector<char> buffer(sizeof(unsigned long) + size);
    unsigned long* addr_ptr = reinterpret_cast<unsigned long*>(buffer.data());
    *addr_ptr = address;
    std::memcpy(buffer.data() + sizeof(unsigned long), value, size);
    
    int ret = ioctl(fd, IOCTL_WRITE_MEM, buffer.data());
    close(fd);
    
    if (ret == 0) {
        LOGI("Sukses menulis ke alamat 0x%lx", address);
        return true;
    } else {
        LOGE("Gagal ioctl, ret=%d, errno=%d", ret, errno);
        return false;
    }
}
// ========== AKHIR TAMBAHAN MEMKERNEL ==========

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

// Variabel Kalibrasi Global
float g_TouchOffsetX = 0.0f;
float g_TouchOffsetY = 0.0f;

// =======================================================
// 🔌 HOOK SENTUHAN (ANTI-FC)
// =======================================================
HOOKAF(int32_t, Consume, void *thiz, void *arg1, bool arg2, long arg3, uint32_t *arg4, AInputEvent **input_event) {
    int32_t result = origConsume(thiz, arg1, arg2, arg3, arg4, input_event);
    
    // Pastikan ImGui sudah siap sebelum menerima sentuhan biar gak crash!
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
    LOGI("==== [GymFlex] MENUNGGU GAME ====");
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

    LOGI("==== [GymFlex] INJECT SUKSES, STANDBY! ====");
    
    // 🛡️ ANTI-FC: JANGAN DIHAPUS! Mencegah thread mati yang bikin game crash.
    while (true) { sleep(9999); }
    return nullptr;
}
