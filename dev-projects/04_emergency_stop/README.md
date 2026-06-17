# 04 — Emergency Stop

> Dev project untuk sistem keamanan: sensor gas MQ-2, relay, dan tombol NC fisik.
> Kode yang sudah teruji dipindah ke `firmware/src/emergency/`.

**PIC:** [Nama] | **Status:** ⏳ Todo

---

## Tujuan

- [ ] Baca ADC MQ-2/DHT11 periodik, deteksi jika melampaui threshold
- [ ] Deteksi tombol NC fisik (circuit terbuka = ditekan = bahaya)
- [ ] Saat trigger: nonaktifkan relay → semua aktuator mati total
- [ ] Kirim sinyal ke state machine bahwa EMERGENCY aktif
- [ ] Sistem tidak bisa resume otomatis — harus reset manual

---

## Arsitektur Safety

```
                    ┌─── GPIO27 (Relay Control) ──► Koil Relay
ESP32               │
                    └─── GPIO39 (Feedback) ◄───── Status jalur daya

Jalur daya aktuator:
PSU → [Relay NO contact] → [Button NC] → Aktuator (Motor + Servo)
         ↑                      ↑
    Dikontrol GPIO27       Hardware cut
    (software estop)       (hardware estop)

Jika salah satu terbuka → aktuator mati.
ESP32 deteksi via GPIO feedback → pindah ke state EMERGENCY.
```

> Relay NO: normally open → relay harus aktif agar aktuator menyala.
> Keuntungan: ESP32 mati/crash → relay lepas → aktuator otomatis mati (fail-safe).

---

## Hardware

| Komponen | Keterangan |Note|
|----------|------------|---|
| MQ-2 | Sensor gas LPG/asap, output analog ke ADC. Warm-up 30 detik |
| DHT-11 | Sensor Suhu dan Kelembapan, Output digital dengan library khusus. refresh rate 1 Hz |
| Relay | Relay 5V (contoh: SRD-05VDC-SL-C), konfigurasi NO |
| Button E-Stop | NC push button, warna merah, ukuran besar, mudah dijangkau |
| Button Reset | NO push button untuk clear state EMERGENCY | Bisa tidak diperlukan |
| DHT22 | Opsional — sensor suhu, trigger tambahan jika suhu > threshold |

---

## Pin (Project Ini)

| GPIO | Fungsi | Catatan |
|------|--------|---------|
| 34 | MQ-2 AO → ADC | ADC1, input-only GPIO |
| 35 | DHT22 data (opsional) | Input, 10kΩ pull-up |
| 27 | Relay coil control | OUTPUT — HIGH = relay aktif = aktuator ON |
| 36 | Button E-Stop NC | INPUT_PULLUP — LOW normal, HIGH saat ditekan |
| 39 | Button Reset | INPUT_PULLUP — LOW normal, HIGH saat ditekan |
| 5 | LED Merah indikator | OUTPUT — menyala saat EMERGENCY |
| 15 | Buzzer | OUTPUT PWM |

---

## Serial Commands (115200 baud)

| Command | Fungsi |
|---------|--------|
| `status` | Print nilai gas ADC, suhu, dan state relay |
| `cal` | Print gas ADC tiap 1 detik untuk kalibrasi baseline |
| `trig` | Simulasi trigger EMERGENCY dari serial |
| `rst` | Reset EMERGENCY state (untuk testing) |
| `relay on` | Aktifkan relay manual |
| `relay off` | Nonaktifkan relay manual |

---

## Kalibrasi MQ-2

Lakukan setelah warm-up 30 detik:

| Kondisi | ADC Value (0–4095) |
|---------|--------------------|
| Udara bersih — baseline | |
| Dekat korek api (gas referensi ringan) | |
| Asap rokok dekat sensor | |
| **Threshold yang ditetapkan** | |

Threshold dicatat ke `GAS_THRESHOLD_ADC` di `config.h`.

---

## Checklist Sebelum Pindah ke Firmware

- [ ] MQ-2/DHT11 bisa deteksi gas di atas threshold yang wajar
- [ ] NC button: terbuka (ditekan) = trigger EMERGENCY
- [ ] Relay mati saat EMERGENCY aktif
- [ ] State tidak bisa clear tanpa reset manual
- [ ] Feedback GPIO ESP32 bisa deteksi kondisi relay
- [ ] LED merah + buzzer aktif saat EMERGENCY
- [ ] Kode siap dipindah ke `firmware/src/emergency/`