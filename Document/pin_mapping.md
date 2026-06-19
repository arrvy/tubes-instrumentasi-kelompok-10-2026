# Pin Mapping — ESP32 WROOM-32

> Single source of truth untuk semua GPIO assignment.
> Jika ada perubahan, update di sini DAN di `firmware/src/config.h`.

---

## Aturan Wajib

- **ADC2** (GPIO 0,2,4,12–15,25–27) tidak bisa dipakai bersamaan dengan WiFi → semua sensor ADC wajib pakai **ADC1**
- **GPIO 6–11** internal flash — jangan dipakai sama sekali
- **GPIO 34,35,36,39** input-only — tidak bisa jadi output
- Servo dan motor power dari sumber eksternal, **bukan** dari pin ESP32

---

## Tabel Pin Lengkap

| GPIO | `#define` di config.h | Arah | Komponen | Keterangan |
|------|----------------------|------|----------|------------|
| **Color Sensor** |
| 32 | `PIN_PHOTODIODE` | ADC1 IN | Photodioda | Ch4, aman saat WiFi aktif |
| 33 | `PIN_LED_RED` | OUT | LED Merah sensor | R=220Ω |
| 25 | `PIN_LED_GREEN` | OUT | LED Hijau sensor | R=150Ω |
| 26 | `PIN_LED_BLUE` | OUT | LED Biru sensor | R=100Ω |
| **Servo** |
| 18 | `PIN_SERVO_1` | OUT PWM | Servo 1 | 50Hz, ledc channel 0 |
| 19 | `PIN_SERVO_2` | OUT PWM | Servo 2 | 50Hz, ledc channel 1 |
| **Motor Konveyor** |
| 16 | `PIN_MOTOR_IN1` | OUT | L298N IN1 | Arah |
| 17 | `PIN_MOTOR_IN2` | OUT | L298N IN2 | Arah |
| 23 | `PIN_MOTOR_ENA` | OUT PWM | L298N ENA | Speed, ledc channel 2 |
| **LCD I2C** |
| 21 | — | I2C SDA | LCD 16x2 | Shared bus |
| 22 | — | I2C SCL | LCD 16x2 | Shared bus |
| **Safety & Emergency** |
| 34 | `PIN_GAS_SENSOR` | ADC1 IN | MQ-2 AO | Input-only GPIO |
| 35 | `PIN_TEMP_SENSOR` | IN | DHT22 | Opsional, 10kΩ pull-up |
| 27 | `PIN_RELAY` | OUT | Koil relay | HIGH = relay aktif = aktuator ON |
| 13 | `PIN_RELAY_FEEDBACK` | IN | Feedback relay | Baca status jalur daya aktuator |
| **Button & Input** |
| 36 | `PIN_BTN_START` | IN | Button START | INPUT_PULLUP, NO, input-only |
| 39 | `PIN_BTN_ESTOP` | IN | Button E-Stop | INPUT_PULLUP, NC, input-only |
| 14 | `PIN_BTN_RESET` | IN | Button RESET | INPUT_PULLUP, NO |
| 12 | `PIN_BTN_MODE` | IN | Button mode LCD | INPUT_PULLUP, NO |
| **LED Indikator** |
| 2 | `PIN_LED_GREEN_IND` | OUT | LED Hijau | State RUNNING |
| 4 | `PIN_LED_YELLOW_IND` | OUT | LED Kuning | State IDLE |
| 5 | `PIN_LED_RED_IND` | OUT | LED Merah | State EMERGENCY |
| **Audio** |
| 15 | `PIN_BUZZER` | OUT PWM | Passive buzzer | tone() / ledc |
| **IR Sensor (opsional)** |
| 3 | `PIN_IR_TRIGGER` | IN | IR receiver | Interrupt, objek masuk zona scan |

---

## Diagram Blok Wiring

```
                    ┌─────────────────────┐
     3.3V ──────────┤ 3V3            GND  ├──── GND (common)
                    │                     │
     LED_R ─[220Ω]─┤ GPIO33       GPIO32 ├──── [10kΩ] ── Photodioda ── GND
     LED_G ─[150Ω]─┤ GPIO25       GPIO34 ├──── MQ-2 AO
     LED_B ─[100Ω]─┤ GPIO26       GPIO36 ├──── BTN_START ── GND
                    │                     │
     Servo1 sig ────┤ GPIO18       GPIO39 ├──── BTN_ESTOP ── GND
     Servo2 sig ────┤ GPIO19       GPIO27 ├──── Relay coil
                    │                     │
     L298N IN1 ─────┤ GPIO16       GPIO21 ├──── LCD SDA
     L298N IN2 ─────┤ GPIO17       GPIO22 ├──── LCD SCL
     L298N ENA ─────┤ GPIO23              │
                    │                     │
     LED_GRN ─[Ω]──┤ GPIO2        GPIO2  │
     LED_YLW ─[Ω]──┤ GPIO4               │
     LED_RED ─[Ω]──┤ GPIO5               │
     Buzzer ────────┤ GPIO15              │
                    └─────────────────────┘
                           ESP32
```