# 05 LCD Display

> Dev project untuk LCD I2C 16x2 dengan multi-mode layout per state machine.
> Kode yang sudah teruji dipindah ke `firmware/src/display/`.

**PIC:** [Nama] | **Status:** ⏳ Todo

---

## Tujuan

- [ ] Init LCD I2C, scan address otomatis
- [ ] Layout tampilan untuk setiap state sistem
- [ ] Multi-mode: beberapa tampilan bisa diswitch via tombol
- [ ] Update non-blocking — pakai `millis()`, bukan `delay()`

---

## Diagram State Machine Dasar (Pakai Mermaid)

'''

'''

---

## Layout Per State

```
STATE_IDLE:              STATE_RUNNING (mode status):
┌────────────────┐       ┌────────────────┐
│ IDLE           │       │ RUNNING        │
│ Tekan START... │       │ Warna: HIJAU   │
└────────────────┘       └────────────────┘

STATE_EMERGENCY:         MODE counter (switch tombol):
┌────────────────┐       ┌────────────────┐
│ !! EMERGENCY   │       │ R:05 G:03 B:07 │
│ GAS DETECTED   │       │ Total: 15      │
└────────────────┘       └────────────────┘

MODE sensor (switch tombol):
┌────────────────┐
│ Gas:1024 OK    │
│ T:28C H:65%    │
└────────────────┘
```

**Mode tampilan yang bisa diswitch:**
1. Status sistem (default) — state + warna terdeteksi terakhir
2. Counter — jumlah per warna + total
3. Sensor — nilai gas ADC + suhu/kelembaban

---

## Hardware

| Komponen | Keterangan |
|----------|------------|
| LCD 16x2 | Dengan I2C backpack (PCF8574) |
| I2C address | Scan dulu: biasanya `0x27` atau `0x3F` |

**Pin:** GPIO 21 (SDA), GPIO 22 (SCL)

---

## Serial Commands (115200 baud)

| Command | Fungsi |
|---------|--------|
| `scan` | Scan I2C address, print hasilnya |
| `idle` | Tampilkan layout IDLE |
| `run <warna>` | Tampilkan layout RUNNING dengan warna |
| `estop <pesan>` | Tampilkan layout EMERGENCY |
| `count <r> <g> <b>` | Tampilkan mode counter |
| `sensor <gas> <suhu>` | Tampilkan mode sensor |
| `mode <1-3>` | Switch mode tampilan |

---

## Checklist Sebelum Pindah ke Firmware

- [ ] LCD terdeteksi di I2C bus (address ditemukan)
- [ ] Semua layout state sudah ditest dan terbaca jelas
- [ ] Mode switch via tombol berfungsi
- [ ] Semua update non-blocking (tidak ada `delay()` di fungsi display)
- [ ] Kode siap dipindah ke `firmware/src/display/`