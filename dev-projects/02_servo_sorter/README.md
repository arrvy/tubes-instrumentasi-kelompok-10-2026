# 02 Servo Sorter

> Dev project untuk kontrol servo dan logika pemilahan berdasarkan warna.
> Kode yang sudah teruji dipindah ke `firmware/src/servo_sorter/`.

**PIC:** [Nama] | **Status:** ⏳ Todo

---

## Tujuan

- [ ] Gerakkan servo ke posisi sudut spesifik secara presisi
- [ ] Mapping: warna → sudut servo yang sesuai
- [ ] Sequence: gerak ke posisi → tahan `SERVO_HOLD_MS` → kembali netral
- [ ] Tidak ada jitter pada posisi hold
- [ ] Test timing: kapan servo harus aktif setelah objek dideteksi di zona scan

---

## Prinsip Kerja

```
Input: enum DetectedColor (RED / GREEN / BLUE / UNKNOWN)
   │
   ├── RED   → Servo1: 30°,  Servo2: 90° (netral)
   ├── GREEN → Servo1: 150°, Servo2: 90° (netral)
   ├── BLUE  → Servo1: 90°,  Servo2: 30°
   └── UNKNOWN → semua netral (barang jatuh ke bin default)
   
Setelah SERVO_HOLD_MS milliseconds → semua servo kembali ke 90°
```

> Posisi sudut di atas adalah estimasi — kalibrasi sesuai mekanik aktual.

---

## Hardware

| Komponen | Spesifikasi | Catatan |
|----------|-------------|---------|
| Servo 1 | SG90 / MG90S | Signal → GPIO18 |
| Servo 2 | SG90 / MG90S | Signal → GPIO19 |
| PSU 5V | Min 2A | Power VCC servo — JANGAN dari pin ESP32 |

> ⚠️ Common ground antara PSU servo dan ESP32 wajib dihubungkan.

---

## Pin (Project Ini)

| GPIO | Fungsi |
|------|--------|
| 18 | Servo 1 Signal (PWM 50Hz) |
| 19 | Servo 2 Signal (PWM 50Hz) |

---

## Serial Commands (115200 baud)

| Command | Fungsi |
|---------|--------|
| `r` | Sort ke bin Merah lalu kembali netral |
| `g` | Sort ke bin Hijau lalu kembali netral |
| `b` | Sort ke bin Biru lalu kembali netral |
| `n` | Paksa semua servo ke posisi netral |
| `p1 <0-180>` | Set servo 1 ke sudut tertentu (kalibrasi) |
| `p2 <0-180>` | Set servo 2 ke sudut tertentu (kalibrasi) |
| `sweep` | Sweep servo 1 dari 0° ke 180° perlahan |

---

## Tabel Kalibrasi Posisi

Isi setelah uji mekanik aktual:

| Kondisi | Servo 1 (°) | Servo 2 (°) |
|---------|-------------|-------------|
| Netral | 90 | 90 |
| Bin Merah | ___ | 90 |
| Bin Hijau | ___ | 90 |
| Bin Biru | 90 | ___ |

---

## Checklist Sebelum Pindah ke Firmware

- [ ] Semua posisi bin terkalibrasi dan dicatat
- [ ] Servo kembali netral setelah `SERVO_HOLD_MS`
- [ ] Tidak ada jitter saat posisi hold
- [ ] Timing delay sudah ditest dengan objek nyata di konveyor
- [ ] Kode siap dipindah ke `firmware/src/servo_sorter/`