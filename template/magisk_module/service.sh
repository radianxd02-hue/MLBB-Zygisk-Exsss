#!/system/bin/sh

# ===============================================
# Script untuk memuat MemKernel dan mengatur izin
# ===============================================

# Lokasi file modul .ko (sesuaikan dengan tempat kamu menyimpannya)
MODULE_PATH="/sdcard/Download/skietm_memk.ko"
DEVICE_NODE="/dev/skietm"

# Tunggu sampai sistem benar-benar siap
sleep 10

# Cek apakah file modul ada, lalu muat
if [ -f "$MODULE_PATH" ]; then
    insmod "$MODULE_PATH" 2>/dev/null
    
    # Tunggu sebentar agar device node muncul
    sleep 2
    
    # Atur izin dan SELinux context jika node ada
    if [ -e "$DEVICE_NODE" ]; then
        chmod 0666 "$DEVICE_NODE"
        chcon u:object_r:device:s0 "$DEVICE_NODE"
    fi
fi

# Matikan SELinux (opsional, bisa dihapus nanti)
setenforce 0
