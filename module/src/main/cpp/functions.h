#ifndef ZYCHEATS_MLBB_FUNCTIONS_H
#define ZYCHEATS_MLBB_FUNCTIONS_H

// Variabel untuk Menu UI (Dibiarkan agar menu.h tidak error)
bool addCurrency, freeItems, everythingUnlocked, showAllItems, addSkins;

// Fungsi helper pembuat string IL2CPP (Aman dibiarkan, tapi offsetnya nanti diganti)
monoString *CreateIl2cppString(const char *str) {
    // KITA MATIKAN DULU BIAR GAK CRASH DI MLBB
    // monoString *(*String_CreateString)(void *instance, const char *str) = (monoString*(*)(void*, const char*)) (g_il2cppBaseMap.startAddress + string2Offset(OBFUSCATE("0x2596B20")));
    // return String_CreateString(NULL, str);
    return NULL; 
}

void Pointers() {
    // KOSONGKAN DULU! Nanti diisi dengan Offset asli Mobile Legends
    LOGI("==== [Zygisk-Exsss] POINTERS STANDBY ====");
}

void Patches() {
    // KOSONGKAN DULU! Jangan pasang Patch game lain ke MLBB
    LOGI("==== [Zygisk-Exsss] PATCHES STANDBY ====");
}

void Hooks() {
    // KOSONGKAN DULU! Jangan pasang Hook game lain ke MLBB
    LOGI("==== [Zygisk-Exsss] HOOKS STANDBY ====");
}

#endif //ZYCHEATS_MLBB_FUNCTIONS_H
