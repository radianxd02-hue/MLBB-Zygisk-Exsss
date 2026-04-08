//
// Created by lbert on 2/9/2023.
// Koreksi Final by Gemini for Exsss
//
#ifndef ZYGISKPG_MISC_H
#define ZYGISKPG_MISC_H

#include "Includes/Dobby/dobby.h"
#include "Include/Unity.h"
#include "KittyMemory/KittyMemory.h"
#include "KittyMemory/KittyScanner.h"
#include "KittyMemory/MemoryPatch.h"
#include "Include/obfuscate.h"
#include <vector>
#include <string>
#include <algorithm>

using KittyMemory::ProcMap;
using KittyScanner::RegisterNativeFn;

// Gunakan 'inline' agar tidak Duplicate Symbol saat di-include banyak file
inline ProcMap g_il2cppBaseMap;

// Fungsi ini harus 'inline' karena didefinisikan di dalam header
inline void hook(void *offset, void* ptr, void **orig)
{
    DobbyHook(offset, ptr, orig);
}

// Gunakan 'inline' untuk vector global agar tidak bentrok
inline std::vector<MemoryPatch> memoryPatches;
inline std::vector<uint64_t> offsetVector;

// Patching a offset with switch.
inline void patchOffset(uint64_t offset, std::string hexBytes, bool isOn) {

    MemoryPatch patch = MemoryPatch::createWithHex(g_il2cppBaseMap, offset, hexBytes);

    // Cari apakah offset sudah ada di dalam offsetVector
    auto it = std::find(offsetVector.begin(), offsetVector.end(), offset);

    if (it != offsetVector.end()) {
        // Jika sudah ada, ambil index-nya dan gunakan patch yang sudah tersimpan
        // JANGAN gunakan 'inline' pada variabel di dalam fungsi (itr/it)
        patch = memoryPatches[std::distance(offsetVector.begin(), it)]; 
    } else {
        // Jika belum ada, masukkan ke dalam daftar
        memoryPatches.push_back(patch);
        offsetVector.push_back(offset);
    }

    if (!patch.isValid()) {
        return;
    }
    
    if (isOn && patch.get_CurrBytes() == patch.get_OrigBytes()) {
        patch.Modify();
    } else if (!isOn && patch.get_CurrBytes() != patch.get_OrigBytes()) {
        patch.Restore();
    }
}

inline uintptr_t string2Offset(const char *c) {
    int base = 16;
    static_assert(sizeof(uintptr_t) == sizeof(unsigned long)
                  || sizeof(uintptr_t) == sizeof(unsigned long long),
                  "Please add string to handle conversion for this architecture.");

    if (sizeof(uintptr_t) == sizeof(unsigned long)) {
        return strtoul(c, nullptr, base);
    }

    return strtoull(c, nullptr, base);
}

#define HOOK(offset, ptr, orig) hook((void *)(g_il2cppBaseMap.startAddress + string2Offset(OBFUSCATE(offset))), (void *)ptr, (void **)&orig)
#define PATCH(offset, hex) patchOffset(string2Offset(OBFUSCATE(offset)), OBFUSCATE(hex), true)
#define PATCH_SWITCH(offset, hex, boolean) patchOffset(string2Offset(OBFUSCATE(offset)), OBFUSCATE(hex), boolean)
#define RESTORE(offset) patchOffset(string2Offset(OBFUSCATE(offset)), "", false)

#endif //ZYGISKPG_MISC_H
