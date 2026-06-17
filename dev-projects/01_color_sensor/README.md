# 01 Color Sensor R&D

> Dev project untuk penelitian dan karakterisasi sensor warna diskrit: Photodioda + LED RGB.
> Kode yang sudah teruji di sini dipindah ke `firmware/src/color_sensor/`.

**PIC:** [Nama] | **Status:** 🔄 In Progress

---

## Tujuan

- [ ] Baca ADC photodioda dengan 3 LED (R, G, B) menyala bergantian
- [ ] Lakukan ambient compensation sebelum baca
- [ ] Normalisasi hasil ADC: `r_norm = adc_r / (adc_r + adc_g + adc_b)`
- [ ] Klasifikasi nearest neighbor: bandingkan dengan database referensi
- [ ] Bedakan minimal 3 warna primer + hitam + putih
- [ ] Hasilkan threshold/database siap pakai untuk `config.h`

---

## Prinsip Kerja

```
Matikan semua LED → baca ambient (ADC_ambient)
LED Merah ON  → baca ADC → ADC_R_raw
LED Hijau ON  → baca ADC → ADC_G_raw
LED Biru ON   → baca ADC → ADC_B_raw

Kompensasi: ADC_R = ADC_R_raw - ADC_ambient  (dst)

Normalisasi:
  sum = ADC_R + ADC_G + ADC_B
  r_norm = ADC_R / sum
  g_norm = ADC_G / sum
  b_norm = ADC_B / sum

Nearest Neighbor:
  Hitung Euclidean distance ke setiap entry database
  Pilih warna dengan distance terkecil
  Jika distance > UNKNOWN_THRESHOLD → return UNKNOWN
```

> Housing/chamber wajib untuk isolasi cahaya ambient dari luar.

---

## Hardware

| Komponen | Spesifikasi | Catatan |
|----------|-------------|---------|
| ESP32 WROOM-32 | — | Pakai ADC1 saja (ADC2 conflict WiFi) |
| LED Merah | 3mm, ~625nm | R = 220Ω ke GPIO |
| LED Hijau | 3mm, ~525nm | R = 150Ω ke GPIO |
| LED Biru | 3mm, ~470nm | R = 100Ω ke GPIO |
| Photodioda | BPW34 atau sejenis | Reverse-bias, pull-up 10kΩ |
| Op-Amp LM358 | Opsional | Pakai jika sinyal ADC < 500 pada LED menyala |
| Housing | Pipa PVC hitam / kardus | Tutup semua sisi kecuali celah objek |

### Wiring Breadboard

```
GPIO33 ──[220Ω]── LED Merah (+)  ──► LED Merah (-)── GND
GPIO25 ──[150Ω]── LED Hijau (+)  ──► LED Hijau (-)── GND
GPIO26 ──[100Ω]── LED Biru (+)   ──► LED Biru  (-)── GND

3.3V ──[10kΩ]── GPIO32 (ADC) ──── Photodioda Cathode
                                   Photodioda Anode ── GND
```

> Cahaya makin kuat → resistansi photodioda turun → tegangan di GPIO32 turun → ADC value turun.
> Artinya: **ADC rendah = pantulan kuat = warna cocok dengan LED yang menyala.**

---

## Pin (Project Ini)

| GPIO | Fungsi |
|------|--------|
| 33 | LED Merah |
| 25 | LED Hijau |
| 26 | LED Biru |
| 32 | ADC Photodioda (ADC1 ch4) |

---

## Serial Commands (115200 baud)

| Command | Fungsi |
|---------|--------|
| `r` | Baca sekali: print R/G/B raw + normalized + klasifikasi |
| `c` | Continuous read tiap 500ms |
| `s` | Stop continuous |
| `raw` | Print raw ADC ketiga channel tanpa klasifikasi |
| `amb` | Baca dan print nilai ambient saja |
| `h` | Tampilkan help |

---

## Data Kalibrasi

Isi tabel ini dari hasil pengukuran. Salin ke `calibration/data_kalibrasi.csv`.

**Kondisi pengukuran:** jarak objek ke sensor: ___mm | housing: ya/tidak | ambient: gelap/indoor/terang

| Warna Objek | ADC_R | ADC_G | ADC_B | r_norm | g_norm | b_norm | Klasifikasi |
|-------------|-------|-------|-------|--------|--------|--------|-------------|
| Merah | | | | | | | MERAH ✓ |
| Hijau | | | | | | | HIJAU ✓ |
| Biru | | | | | | | BIRU ✓ |
| Putih | | | | | | | PUTIH ✓ |
| Hitam | | | | | | | HITAM ✓ |

**Database referensi yang dipakai di `config.h`:**

```cpp
// {r_norm, g_norm, b_norm}
#define DB_RED    {0.60f, 0.25f, 0.15f}   // ← isi dari hasil kalibrasi
#define DB_GREEN  {0.20f, 0.60f, 0.20f}
#define DB_BLUE   {0.15f, 0.25f, 0.60f}
#define DB_WHITE  {0.33f, 0.33f, 0.34f}
#define DB_BLACK  {0.33f, 0.33f, 0.34f}   // mirip white, bedakan dari ADC magnitude
#define UNKNOWN_THRESHOLD  0.15f           // jika distance > ini → UNKNOWN
```

---

## Checklist Sebelum Pindah ke Firmware

- [ ] Akurasi klasifikasi 3 warna primer > 90% dalam kondisi housing tertutup
- [ ] Ambient compensation terbukti membantu stabilitas
- [ ] Pembacaan stabil: variasi `r_norm` < 0.05 antara 10 sample berulang
- [ ] Database referensi sudah dicatat dan di-commit ke `calibration/`
- [ ] Kode sudah direfactor, siap dipindah ke `firmware/src/color_sensor/`