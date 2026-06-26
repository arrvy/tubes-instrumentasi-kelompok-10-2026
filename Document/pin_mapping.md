# Pin Mapping - ESP32 WROOM-32

> Single source of truth untuk semua GPIO assignment.
> Jika ada perubahan, update di sini dan di konfigurasi firmware/dev project terkait.

---

## Aturan Wajib

- **ADC2** (GPIO 0,2,4,12-15,25-27) tidak bisa dipakai bersamaan dengan WiFi, jadi semua sensor analog wajib pakai **ADC1**.
- **GPIO 6-11** internal flash, jangan dipakai.
- **GPIO 34,35,36,39** input-only, tidak bisa jadi output.
- Servo dan motor memakai power eksternal, bukan dari pin ESP32.
- Tombol START dan STOP tidak dibaca kode. Keduanya dipasang langsung secara hardware ke power supply untuk menyalakan/mematikan sistem keseluruhan.
- Emergency button NC latched tidak dibaca kode. Tombol ini dipasang hardware seri dengan relay sebagai saklar emergency manual.
- Emergency otomatis dari kode memakai threshold suhu DHT11.

---

## Tabel Pin Lengkap

| GPIO | Nama Konfigurasi | Arah | Komponen | Keterangan |
|------|------------------|------|----------|------------|
| **Color Sensor** |
| 33 | `PIN_LDR` / `pinPhotodiode` | ADC1 IN | LDR | Aman saat WiFi aktif |
| 15 | `PIN_LED_RED` / `pinLedR` | OUT | LED R | LED merah sensor warna |
| 2 | `PIN_LED_GREEN` / `pinLedG` | OUT | LED G | LED hijau sensor warna |
| 4 | `PIN_LED_BLUE` / `pinLedB` | OUT | LED B | LED biru sensor warna |
| **IR Sensor** |
| 35 | `PIN_IR_TRIGGER` | IN | IR sensor | Deteksi objek dan counter |
| **DHT11** |
| 32 | `PIN_DHT` / `dhtPin` | IN | DHT11 | Emergency otomatis berbasis suhu |
| **Servo** |
| 16 | `PIN_SERVO` / `servoPin` | OUT PWM | Servo sorter | 50 Hz |
| **Relay** |
| 13 | `PIN_RELAY` / `relayPin` | OUT | Koil relay | HIGH = relay aktif |
| **Motor Konveyor** |
| 14 | `PIN_MOTOR_ENB` / `enPin` | OUT PWM | L298N ENB | Speed |
| 27 | `PIN_MOTOR_IN3` / `in1Pin` | OUT | L298N IN3 | Arah |
| 26 | `PIN_MOTOR_IN4` / `in2Pin` | OUT | L298N IN4 | Arah |
| **LCD I2C** |
| 21 | - | I2C SDA | LCD 16x2 | Shared I2C bus |
| 22 | - | I2C SCL | LCD 16x2 | Shared I2C bus |

---

## Diagram Blok Wiring

```text
ESP32 GPIO35  <- IR sensor output
ESP32 GPIO32  <-> DHT11 data
ESP32 GPIO33  <- LDR analog divider
ESP32 GPIO16  -> Servo signal
ESP32 GPIO13  -> Relay control
ESP32 GPIO21  <-> LCD SDA
ESP32 GPIO22  -> LCD SCL
ESP32 GPIO14  -> L298N ENB PWM
ESP32 GPIO27  -> L298N IN3
ESP32 GPIO26  -> L298N IN4
ESP32 GPIO15  -> LED R
ESP32 GPIO2   -> LED G
ESP32 GPIO4   -> LED B

START/STOP hardware:
Power supply -> START/STOP switch -> system power input

Manual emergency hardware:
Relay supply/control path -> NC latched emergency button -> relay
```
