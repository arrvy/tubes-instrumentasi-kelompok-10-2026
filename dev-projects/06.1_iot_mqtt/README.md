# 06 IoT MQTT

> Dev project untuk koneksi WiFi, MQTT publish/subscribe, dan Node-RED dashboard.
> Kode yang sudah teruji dipindah ke `firmware/src/iot/`.

**PIC:** [Nama] | **Status:** ⏳ Todo

---

## Tujuan

- [ ] Koneksi WiFi dengan auto-reconnect
- [ ] MQTT publish semua data: state, warna, gas, counter, emergency
- [ ] MQTT subscribe untuk remote reset dari dashboard
- [ ] Seluruh operasi non-blocking — tidak boleh block loop utama
- [ ] Node-RED dashboard menampilkan semua data real-time

---

## Library

```ini
; tambahkan di platformio.ini
lib_deps =
    knolleary/PubSubClient @ ^2.8
```

---

## Konfigurasi

Buat file `src/secrets.h` — **ada di `.gitignore`, jangan di-commit:**

```cpp
#define WIFI_SSID      "nama_wifi"
#define WIFI_PASSWORD  "password"
#define MQTT_HOST      "192.168.x.x"   // IP laptop yang running Mosquitto
#define MQTT_PORT      1883
#define MQTT_CLIENT_ID "esp32-tubes-01"
```

---

## Topics yang Ditest

| Topic | Arah | Contoh Payload |
|-------|------|----------------|
| `instru/system/state` | Pub | `"RUNNING"` |
| `instru/sensor/color` | Pub | `{"r":2800,"g":400,"b":300,"color":"MERAH"}` |
| `instru/sensor/gas` | Pub | `{"adc":1200}` |
| `instru/conveyor/count` | Pub | `{"red":5,"green":3,"blue":7,"total":15}` |
| `instru/system/emergency` | Pub | `{"trigger":"GAS","value":2800}` |
| `instru/control/reset` | Sub | `"1"` |

---

## Serial Commands (115200 baud)

| Command | Fungsi |
|---------|--------|
| `status` | Print WiFi status + MQTT status + IP |
| `pub <topic> <msg>` | Test publish manual ke topic tertentu |
| `reconnect` | Force disconnect + reconnect WiFi dan MQTT |

---

## Node-RED Flows yang Dibuat

1. `instru/system/state` → Dashboard text node (tampilkan state)
2. `instru/sensor/color` → JSON parse → text node (warna) + chart history warna
3. `instru/sensor/gas` → JSON parse → gauge node (nilai gas)
4. `instru/conveyor/count` → JSON parse → bar chart per warna
5. `instru/system/emergency` → Function node → notification merah + alert audio
6. Button "RESET" di dashboard → MQTT out ke `instru/control/reset`

Setelah selesai, export flow ke `../../iot/nodered/flow_v1.json`.

---

## Checklist Sebelum Pindah ke Firmware

- [ ] WiFi auto-reconnect saat putus
- [ ] MQTT reconnect otomatis saat broker tidak terjangkau
- [ ] Semua topic publish terverifikasi di Mosquitto subscriber
- [ ] Subscribe remote reset berfungsi dan trigger aksi di ESP32
- [ ] Node-RED dashboard menampilkan semua data dengan benar
- [ ] `secrets.h` sudah masuk `.gitignore` dan tidak ter-commit
- [ ] Kode siap dipindah ke `firmware/src/iot/`