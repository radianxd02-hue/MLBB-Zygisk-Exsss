#include <cstring>
#include <jni.h>
#include <pthread.h>
#include "hook.h"
#include "zygisk.hpp"

// =======================================================
// 📞 PANGGIL FUNGSI DARI HOOK.CPP (PENTING!)
// =======================================================
extern int isGame(JNIEnv *env, jstring appDataDir);
extern void *hack_thread(void *arg);


using zygisk::Api;
using zygisk::AppSpecializeArgs;
using zygisk::ServerSpecializeArgs;

class MyModule : public zygisk::ModuleBase {
public:
    void onLoad(Api *api, JNIEnv *env) override {
        env_ = env;
    }

    void preAppSpecialize(AppSpecializeArgs *args) override {
        if (!args || !args->nice_name) {
            // LOGE("Skip unknown process");
            return;
        }
        // Mengecek apakah ini game MLBB
        enable_hack = isGame(env_, args->app_data_dir); 
    }

    void postAppSpecialize(const AppSpecializeArgs *) override {
        if (enable_hack) {
            int ret;
            pthread_t ntid;
            // Menjalankan thread cheat kita yang ada di hook.cpp
            if ((ret = pthread_create(&ntid, nullptr, hack_thread, nullptr))) {
                // LOGE("can't create thread: %s\n", strerror(ret));
            }
        }
    }

private:
    JNIEnv *env_{};
    bool enable_hack = false; // <-- WAJIB DITAMBAH BIAR GAK ERROR
};

REGISTER_ZYGISK_MODULE(MyModule)
