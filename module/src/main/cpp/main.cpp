#include <cstring>
#include <jni.h>
#include <pthread.h>
#include "hook.h"
#include "zygisk.hpp"

// =======================================================
// 📞 PANGGIL FUNGSI DARI HOOK.CPP (PENTING!)
// =======================================================
// isGame dihapus karena kita deteksi langsung di bawah
extern void *hack_thread(void *arg);

using zygisk::Api;
using zygisk::AppSpecializeArgs;
using zygisk::ServerSpecializeArgs;

class MyModule : public zygisk::ModuleBase {
public:
    void onLoad(Api *api, JNIEnv *env) override {
        env_ = env;
        api_ = api;
    }

    void preAppSpecialize(AppSpecializeArgs *args) override {
        if (!args || !args->nice_name) {
            return;
        }
        
        // Cek nama paket game (PUBG Global)
        const char *process = env_->GetStringUTFChars(args->nice_name, nullptr);
        if (process != nullptr && strstr(process, "com.tencent.ig")) {
            // Kalau kamu main versi Korea, ganti jadi "com.pubg.krmobile"
            enable_hack = true;
            // Opsi tambahan biar aman dari anticheat
            api_->setOption(zygisk::Option::FORCE_DENYLIST_UNMOUNT); 
        } else {
            enable_hack = false;
        }
        env_->ReleaseStringUTFChars(args->nice_name, process);
    }

    void postAppSpecialize(const AppSpecializeArgs *) override {
        if (enable_hack) {
            int ret;
            pthread_t ntid;
            // Menjalankan thread cheat kita yang ada di hook.cpp
            if ((ret = pthread_create(&ntid, nullptr, hack_thread, nullptr))) {
                // Gagal bikin thread
            }
        } else {
            // Bersihkan library kalau bukan target (biar HP gak berat)
            api_->setOption(zygisk::Option::DLCLOSE_MODULE_LIBRARY);
        }
    }

private:
    Api *api_{};
    JNIEnv *env_{};
    bool enable_hack = false; 
};

REGISTER_ZYGISK_MODULE(MyModule)
