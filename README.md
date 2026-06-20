# Sistem Pemilah Barang Berbasis Sensor Warna Diskrit
### Tugas Besar Praktikum Instrumentasi 2026

> Sistem otomatis memilah kubus berwarna menggunakan sensor warna diskrit (Photodioda + LED RGB),
> konveyor belt, servo sweeper, emergency stop berbasis sensor gas, dan monitoring IoT via MQTT + Node-RED.
> Platform: ESP32 WROOM-32 | PlatformIO + Arduino Framework

---

## Anggota Tim

| Nama | NIM | Divisi Utama | GitHub |
|------|-----|--------------|--------|
| [Nama 1] | [NIM] | Elektronik — Color Sensor & PCB | @username |
| [Nama 2] | [NIM] | Programmer — State Machine & Aktuator | @username |
| [Nama 3] | [NIM] | Programmer — IoT & Display | @username |
| [Nama 4] | [NIM] | Mekanik & Programmer — Emergency Stop | @username |
| [Nama 5] | [NIM] | Mekanik & Programmer — Emergency Stop | @username |
| [Nama 6] | [NIM] | Mekanik & Programmer — Emergency Stop | @username |
| [Nama 7] | [NIM] | Mekanik & Programmer — Emergency Stop | @username |

**Asisten:** Noor Huda Ramdani

---

## Alur Sistem

```
Kubus masuk
    │
    ▼
[Konveyor] ──► [Chamber Scan]
                     │
         ┌───────────┼────────────────┐
         │  LED R/G/B menyala         │
         │  bergantian                │
         │  Photodioda baca ADC,      |
         | IR Counter Mendeteksi Objek| 
         └───────────┬────────────────┘
                     │
              Normalisasi ADC
                     │
            Nearest Neighbor
            Classification
                     │
         ┌───────────┼───────────┐-------------┐
         ▼           ▼           ▼             ▼
      MERAH        HIJAU        BIRU      HITAM/PUTIH
         │           │           │             │
      Servo 1      Servo 1     Servo 2     Servo 2
      aktif        aktif       aktif       aktif
      sudut 1      sudut 2     sudut 1     sudut 2
         │           │           │             │
      [Bin R]     [Bin G]     [Bin B]       [Bin B/W]

Paralel sepanjang waktu:
├── LCD                  → tampilkan state + warna + counter
├── LED 3 warna          → indikator state machine
├── MQ-2/DHT11           → monitor gas, trigger EMERGENCY jika > threshold
├── Button NC            → hardware emergency stop
├── Relay                → putus daya aktuator saat EMERGENCY
└── MQTT/Blnyk/Modbus    → publish state/warna/counter/gas ke Node-RED
```

---

## Struktur Repo

```
tubes-instrumentasi/
├── README.md                       ← file ini
├── DECISION_LOG.md                 ← kenapa pilih A bukan B
├── .gitignore
│
├── dev-projects/                   ← kembangkan modul di sini dulu
│   ├── 01_color_sensor/            ← R&D photodioda + LED RGB
│   ├── 02_servo_sorter/            ← kontrol servo + logika sorting
│   ├── 03_conveyor_motor/          ← motor DC + PWM speed control
│   ├── 04_emergency_stop/          ← MQ-2 + relay + tombol NC
│   ├── 05_ui_display/             ← LCD I2C + layout per state
│   ├── 06_iot_mqtt/                ← WiFi + MQTT + Node-RED
│   └── 07_integration_test/        ← end-to-end test semua modul
│
├── firmware/                       ← kode final terintegrasi
│   ├── platformio.ini
│   └── src/
│       ├── main.cpp                ← state machine utama
│       ├── config.h                ← semua pin, threshold, konstanta
│       ├── secrets.h               ← WiFi & MQTT credentials (di .gitignore)
│       ├── color_sensor/
│       │   ├── color_sensor.h
│       │   └── color_sensor.cpp
│       ├── conveyor/
│       │   ├── conveyor.h
│       │   └── conveyor.cpp
│       ├── servo_sorter/
│       │   ├── servo_sorter.h
│       │   └── servo_sorter.cpp
│       ├── emergency/
│       │   ├── emergency.h
│       │   └── emergency.cpp
│       ├── display/
│       │   ├── display.h
│       │   └── display.cpp
│       ├── indicator/
│       │   ├── indicator.h
│       │   └── indicator.cpp
│       └── iot/
│           ├── iot.h
│           └── iot.cpp
│
├── hardware/
│   ├── schematic/                  ← file EasyEDA / KiCad
│   └── pcb/                        ← layout PCB jika memakai PCB
│   
│
│
├── mechanical/
│   ├── drawings/                   ← sketsa, foto, dimensi
│   └── 3d-design/
│
├── iot/
│   └── nodered/
│       └── flow_v1.json            ← export flow Node-RED
│
├── calibration/
│   └── data_kalibrasi.csv          ← hasil pengukuran ADC per warna
│
└── docs/
    ├── state_machine.md            ← diagram + tabel transisi state
    ├── pin_mapping.md              ← semua GPIO assignment
    ├── data_models.md              ← struct, enum, typedef
    ├── mqtt_topics.md              ← semua topic + format payload
    ├── wiring_guide.md             ← panduan wiring per subsistem
    ├── gambaran_kasar.md           ← due 18 Juni
    ├── laporan_perancangan.md      ← due 23 Juni
    ├── laporan_tugas_besar.md      ← due 29 Juni
    ├── laporan_perancangan.docx    ← due 23 Juni
    └── laporan_tugas_besar.docx    ← due 29 Juni
```

---

## Dev Projects — Alur Kerja

```
dev-projects/0X_nama/   →   tested & stabil   →   firmware/src/modul/
```

Setiap dev project adalah PlatformIO project mandiri.
Kembangkan, kalibrasi, dan test di sini dulu sebelum integrasi ke firmware.

| No | Folder | Tujuan | PIC | Status |
|----|--------|--------|-----|--------|
| 01 | `01_color_sensor` | R&D sensor warna diskrit | [Nama] | 🔄 |
| 02 | `02_servo_sorter` | Kontrol servo + logika sorting | [Nama] | ⏳ |
| 03 | `03_conveyor_motor` | Motor DC + PWM speed | [Nama] | ⏳ |
| 04 | `04_emergency_stop` | Sensor gas MQ-2 + tombol NC + relay | [Nama] | ⏳ |
| 05 | `05_lcd_display` | LCD I2C 16x2 + layout state | [Nama] | ⏳ |
| 06 | `06_iot_mqtt` | WiFi + MQTT + Node-RED | [Nama] | ⏳ |
| 07 | `07_integration_test` | End-to-end test | Semua | ⏳ |

---

## State Machine

```
              ┌─────────────────────────────────────────┐
              │  Dari state MANAPUN, kapan saja:        │
              │  MQ2 > threshold  → EMERGENCY           │
              │  Button NC terbuka → EMERGENCY          │
              └─────────────────────────────────────────┘

  [IDLE] ──── START ditekan ────► [STARTING]
                                      │
                              init 2 detik
                                      │
                                      ▼
                    ┌────────────[RUNNING] ◄──────────────────┐
                    │            konveyor jalan                │
                    │            tunggu objek                  │
                    │                                          │
              objek terdeteksi                                 │
              (IR / timing)                                    │
                    │                                          │
                    ▼                                          │
               [SCANNING]                                      │
               baca ambient                                    │
               nyalakan LED R/G/B                              │
               baca ADC                                        │
               normalisasi                                     │
               nearest neighbor                                │
                    │                                          │
              warna diketahui                                  │
                    │                                          │
                    ▼                                          │
               [SORTING]                                       │
               aktifkan servo sesuai warna                     │
               tahan SERVO_HOLD_MS                             │
               servo kembali netral ─────────────────────────►┘
                    
  [EMERGENCY] ◄── trigger kapan saja
       │
       │  motor stop
       │  servo netral
       │  relay OFF → aktuator mati total
       │  LED merah + buzzer ON
       │  MQTT publish emergency
       │  LCD tampilkan pesan
       │
       └── reset manual (tekan button reset) ──► [IDLE]
```

Detail lengkap: [`docs/state_machine.md`](docs/state_machine.md)

---

## Pin Mapping (Ringkasan) (BELUM FIX)

> Detail lengkap + catatan: [`docs/pin_mapping.md`](docs/pin_mapping.md)
> Nilai `#define`: [`firmware/src/config.h`](firmware/src/config.h)

| GPIO | Fungsi | Komponen | Catatan |
|------|--------|----------|---------|
| 32 | ADC Photodioda | Color sensor | ADC1 ch4 — aman dengan WiFi |
| 33 | LED Merah sensor | RGB LED | OUTPUT, R=220Ω |
| 25 | LED Hijau sensor | RGB LED | OUTPUT, R=150Ω |
| 26 | LED Biru sensor | RGB LED | OUTPUT, R=100Ω |
| 18 | Servo 1 | Sweeper | PWM 50Hz |
| 19 | Servo 2 | Sweeper | PWM 50Hz |
| 16 | Motor IN1 | L298N | Arah konveyor |
| 17 | Motor IN2 | L298N | Arah konveyor |
| 23 | Motor ENA | L298N | PWM speed |
| 21 | SDA | LCD I2C | I2C bus |
| 22 | SCL | LCD I2C | I2C bus |
| 34 | ADC Gas | MQ-2 | ADC1, input-only |
| 27 | Relay control | Relay safety | OUTPUT |
| 36 | Button Start | Push button NO | INPUT_PULLUP |
| 39 | Button E-Stop | Push button NC | INPUT_PULLUP |
| 2 | LED Hijau indikator | State LED | Running |
| 4 | LED Kuning indikator | State LED | Idle/standby |
| 5 | LED Merah indikator | State LED | Emergency |
| 15 | Buzzer | Passive buzzer | PWM tone |

> ⚠️ **ADC2 (GPIO 0,2,4,12-15,25-27) tidak bisa dipakai bersamaan dengan WiFi aktif — pakai ADC1 untuk semua sensor.**
> ⚠️ **GPIO 6–11 terhubung ke flash internal — jangan dipakai.**
> ⚠️ **GPIO 34, 35, 36, 39 adalah input-only.**

---

## MQTT Topics (Ringkasan)

> Detail lengkap + format payload: [`docs/mqtt_topics.md`](docs/mqtt_topics.md)

| Topic | Arah | Format Payload |
|-------|------|----------------|
| `instru/system/state` | ESP32 → Broker | `"RUNNING"` |
| `instru/system/emergency` | ESP32 → Broker | `{"trigger":"GAS","value":2800}` |
| `instru/sensor/color` | ESP32 → Broker | `{"r":2800,"g":400,"b":300,"color":"MERAH"}` |
| `instru/sensor/gas` | ESP32 → Broker | `{"adc":1200}` |
| `instru/conveyor/count` | ESP32 → Broker | `{"red":5,"green":3,"blue":7,"total":15}` |
| `instru/control/reset` | Broker → ESP32 | `"1"` |

---

## Setup Cepat

```bash
# 1. Clone repo
git clone https://github.com/arrvy/tubes-instrumentasi-kelompok-10-2026.git
cd tubes-instrumentasi-kelompok-10-2026

# 2. Buka dev project di PlatformIO (contoh: color sensor)
cd dev-projects/01_color_sensor
code .

# 3. Buka firmware final
cd firmware
code .

# 4. Build + upload + monitor
pio run --target upload
pio device monitor --baud 115200
```

Buat `firmware/src/secrets.h` — **jangan di-commit, sudah ada di `.gitignore`:**

```cpp
#define WIFI_SSID      "nama_wifi_kamu"
#define WIFI_PASSWORD  "password_wifi"
#define MQTT_HOST      "192.168.x.x"
#define MQTT_PORT      1883
#define MQTT_CLIENT_ID "esp32-tubes-01"
```

---

## Setup MQTT Broker (Mosquitto)

```bash
# Ubuntu / WSL
sudo apt install mosquitto mosquitto-clients
sudo systemctl start mosquitto

# Test subscribe semua topic
mosquitto_sub -h localhost -t "instru/#" -v

# Test publish manual
mosquitto_pub -h localhost -t "instru/test" -m "hello"
```

---

## Setup Node-RED

```bash
npm install -g node-red
node-red
# Buka: http://localhost:1880
# Import flow dari: iot/nodered/flow_v1.json
```

---

## Branching Strategy

```
main        ← stable, hanya merge menjelang demo/responsi
dev         ← integrasi semua fitur, up di remote
feature/xx  ← pengembangan per fitur, merge ke dev via PR, buat di local saja
```

```bash
# Mulai fitur baru
git checkout dev && git pull origin dev
git checkout -b feature/nama-fitur

# Selesai, push dan buat PR ke dev
git add .
git commit -m "feat(color): tambah mode kalibrasi serial"
git push origin feature/nama-fitur
```

**Format commit message:**

```
feat(modul)  : tambah fitur baru
fix(modul)   : perbaiki bug
docs         : update dokumentasi / README
hw           : update skematik atau PCB
test         : tambah atau update test case
refactor     : refactor kode tanpa ubah fungsi
```

---

## Timeline

| Milestone | Deadline | Status |
|-----------|----------|--------|
| Technical Meeting | 13 Jun 2026 | ✅ Done |
| Gambaran kasar ke asisten | 18 Jun 2026 | 🔄 In Progress |
| Laporan perancangan (dastek + langkah) | 23 Jun 2026 | ⏳ Todo |
| Responsi + laporan tugas besar | 29 Jun 2026 | ⏳ Todo |

## Contributors
<a href="https://github.com/arrvy/tubes-instrumentasi-kelompok-10-2026/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=arrvy/tubes-instrumentasi-kelompok-10-2026" />
</a>
