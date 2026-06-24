# IR Trigger and Counter

Library ini menyediakan subsystem deteksi objek berbasis sensor IR untuk ESP32 PlatformIO.

Fokus utamanya adalah:

- mendeteksi objek secara non-blocking,
- menghitung total objek,
- menghasilkan event trigger dan release,
- menjaga layer aplikasi tetap memegang serial parser,
- tetap mudah dipindahkan ke firmware integrasi yang lebih besar.

## Struktur

```text
src/
  main.cpp
  IRTriggerConfig.h
  IRTrigger.h
  IRTrigger.cpp
```

## Karakteristik

- C++ berbasis class
- tidak ada variabel global di module
- semua konfigurasi melalui constructor
- ada `begin()`
- ada `update()`
- ada `readTrigger()` dan `readRelease()`
- timing memakai `millis()`
- tidak memakai `delay()`
- tidak membaca `Serial` langsung dari library
- debug output memakai `Print&`
- kompatibel ESP32 Dev Module + PlatformIO + VS Code

## State Machine

```text
WAIT_OBJECT -> DEBOUNCE_TRIGGER -> OBJECT_PRESENT -> DEBOUNCE_CLEAR -> WAIT_OBJECT
```

Saat sensor aktif stabil, counter akan naik satu kali dan event trigger akan di-latch sampai dibaca layer aplikasi.

## Cara Pakai

1. Ubah konfigurasi default di `src/main.cpp`.
2. Panggil `begin()` pada `setup()`.
3. Panggil `update()` pada `loop()`.
4. Tangani event dengan `readTrigger()` dan `readRelease()`.

## Integrasi Firmware

Library hanya tahu tentang pin sensor, debounce, counter, dan state internal. Jika firmware integrasi membutuhkan parser serial, taruh di layer aplikasi, bukan di library.
