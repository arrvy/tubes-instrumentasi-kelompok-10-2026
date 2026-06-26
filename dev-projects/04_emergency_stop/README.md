# 04 - Emergency Stop

> Dev project untuk sistem keamanan otomatis berbasis DHT11 dan relay.
> Emergency manual memakai button NC latched secara hardware seri dengan relay, tidak dibaca oleh kode.

**PIC:** [Nama] | **Status:** Todo

---

## Tujuan

- [ ] Baca suhu DHT11 periodik.
- [ ] Trigger emergency otomatis jika suhu melewati threshold.
- [ ] Saat trigger: nonaktifkan relay sehingga aktuator mati total.
- [ ] Kirim sinyal ke state machine bahwa `EMERGENCY` aktif.
- [ ] Sistem tidak resume otomatis sebelum kondisi aman dan reset software dilakukan.

---

## Arsitektur Safety

```text
ESP32 GPIO13 -> Relay control
ESP32 GPIO32 <-> DHT11 data

Jalur daya / kontrol aktuator:
Power/relay path -> Button NC latched -> Relay -> Aktuator

Emergency otomatis:
DHT11 suhu >= threshold -> ESP32 mematikan relay

Emergency manual:
Button NC latched ditekan -> jalur relay terbuka secara hardware
```

---

## Hardware

| Komponen | Keterangan |
|----------|------------|
| DHT11 | Sensor suhu dan kelembapan, data digital, refresh sekitar 1 Hz |
| Relay | Relay 5V, HIGH = relay aktif sesuai default kode |
| Button E-Stop NC Latched | Dipasang seri dengan relay sebagai saklar emergency manual hardware |

---

## Pin (Project Ini)

| GPIO | Fungsi | Catatan |
|------|--------|---------|
| 32 | DHT11 data | Input data DHT11 |
| 13 | Relay coil control | OUTPUT, HIGH = relay aktif |

`estopPin` dan `feedbackPin` pada kode default bernilai `255`, artinya tidak dipakai. Emergency button manual ditangani hardware.

---

## Serial Commands (115200 baud)

| Command | Fungsi |
|---------|--------|
| `status` | Print suhu DHT11 dan state relay |
| `rst` | Reset emergency state saat kondisi sudah aman |
| `relay on` | Aktifkan relay manual untuk testing |
| `relay off` | Nonaktifkan relay manual untuk testing |

---

## Checklist Sebelum Pindah ke Firmware

- [ ] DHT11 terbaca stabil.
- [ ] Threshold suhu sesuai kondisi demo.
- [ ] Relay mati saat emergency otomatis aktif.
- [ ] Button NC latched terbukti memutus relay secara hardware.
- [ ] Kode siap dipindah ke `firmware/src/emergency/`.
