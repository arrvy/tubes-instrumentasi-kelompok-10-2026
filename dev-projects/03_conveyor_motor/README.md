# 03 Conveyor Motor

> Dev project untuk kontrol motor DC konveyor via PWM + L298N.
> Kode yang sudah teruji dipindah ke `firmware/src/conveyor/`.

**PIC:** [Nama] | **Status:** ⏳ Todo

---

## Tujuan

- [ ] Start / stop motor dari GPIO
- [ ] Set kecepatan via PWM duty cycle (0–255)
- [ ] Kecepatan stabil dan repeatable
- [ ] Tidak ada brownout ESP32 saat motor start/stop
- [ ] Tentukan PWM value optimal agar objek punya waktu scan > 500ms

---

## Kalkulasi Target Kecepatan

```
Panjang zona scan       = ___ mm
Waktu scan minimal      = 500 ms
Kecepatan belt maks     = panjang / waktu = ___ mm/s

Jarak zona scan ke zona sweeper = ___ mm
Waktu tempuh            = jarak / kecepatan = ___ ms
→ ini jadi nilai CONVEYOR_SORT_DELAY_MS di config.h
```

---

## Hardware

| Komponen | Keterangan |
|----------|------------|
| Motor DC | 12V + gearbox, cukup torsi untuk belt + beban kubus |
| L298N | Driver motor, supply motor dari 12V terpisah |
| PSU 12V | Terpisah dari supply ESP32 dan servo |

### Wiring L298N

```
ESP32 GPIO16 ── L298N IN1
ESP32 GPIO17 ── L298N IN2
ESP32 GPIO23 ── L298N ENA  (PWM)
PSU 12V (+)  ── L298N 12V
GND          ── L298N GND + ESP32 GND (common ground!)
L298N OUT1/2 ── Motor DC terminal
```

---

## Pin (Project Ini)

| GPIO | Fungsi |
|------|--------|
| 16 | L298N IN1 (arah) |
| 17 | L298N IN2 (arah) |
| 23 | L298N ENA (PWM speed) |

---

## Serial Commands (115200 baud)

| Command | Fungsi |
|---------|--------|
| `start` | Jalankan motor kecepatan default |
| `stop` | Hentikan motor |
| `s <0-255>` | Set kecepatan (PWM duty cycle) |
| `ramp` | Test ramp-up perlahan lalu ramp-down |

---

## Tabel Kecepatan

Isi dari hasil pengukuran aktual:

| PWM Value | Kecepatan Belt (cm/s) | Waktu di Zona Scan (ms) | Layak? |
|-----------|----------------------|------------------------|--------|
| 100 | | | |
| 150 | | | |
| 200 | | | |
| 255 | | | |

**PWM optimal yang dipilih:** ___ → dicatat ke `MOTOR_SPEED_DEFAULT` di `config.h`

---

## Checklist Sebelum Pindah ke Firmware

- [ ] Motor stabil di kecepatan target
- [ ] Tidak ada brownout saat start/stop
- [ ] Waktu objek di zona scan > 500ms pada PWM yang dipilih
- [ ] `CONVEYOR_SORT_DELAY_MS` sudah dihitung dan dicatat
- [ ] Kode siap dipindah ke `firmware/src/conveyor/`