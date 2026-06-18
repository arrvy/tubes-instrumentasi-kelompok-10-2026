# MQTT Topics & Payload Format

---

## Broker

```
Host   : 192.168.x.x  ← IP laptop/RPi yang running Mosquitto (update sebelum demo)
Port   : 1883
Client : esp32-tubes-01
```

---

## Semua Topics

### Publish — ESP32 → Broker

```
instru/system/state
instru/system/emergency
instru/sensor/color
instru/sensor/gas
instru/conveyor/count
```

### Subscribe — Broker → ESP32

```
instru/control/reset
```

---

## Format Payload Detail

### `instru/system/state`
```
Tipe    : string
QoS     : 0
Retain  : false
Nilai   : "IDLE" | "STARTING" | "RUNNING" | "SCANNING" | "SORTING" | "EMERGENCY"
Contoh  : RUNNING
Trigger : setiap transisi state
```

### `instru/system/emergency`
```
Tipe    : JSON
QoS     : 1
Retain  : true   ← retain agar dashboard tahu bahkan setelah reconnect
Format  : {"trigger":"GAS","value":2800,"timestamp":123456}
Trigger : saat masuk state EMERGENCY
```

### `instru/sensor/color`
```
Tipe    : JSON
QoS     : 0
Retain  : false
Format  : {"r":2800,"g":400,"b":300,"r_norm":0.82,"g_norm":0.12,"b_norm":0.09,"color":"MERAH"}
Trigger : setelah setiap scan berhasil (state SORTING)
```

### `instru/sensor/gas`
```
Tipe    : JSON
QoS     : 0
Retain  : false
Format  : {"adc":1200,"threshold":2500,"safe":true}
Trigger : periodik setiap MQTT_PUBLISH_INTERVAL_MS
```

### `instru/conveyor/count`
```
Tipe    : JSON
QoS     : 0
Retain  : false
Format  : {"red":5,"green":3,"blue":7,"unknown":1,"total":16}
Trigger : setiap counter berubah (setelah sorting)
```

### `instru/control/reset` (subscribe)
```
Tipe    : string
Format  : "1"
Aksi    : trigger reset dari EMERGENCY ke IDLE (jika kondisi aman)
```

---

## Node-RED Flow Map

| MQTT Topic | Node-RED | Tampilan Dashboard |
|------------|----------|--------------------|
| `instru/system/state` | text node | Badge state saat ini |
| `instru/sensor/color` | JSON → text | Warna terdeteksi terakhir |
| `instru/sensor/gas` | JSON → gauge | Gauge 0–4095, merah jika > threshold |
| `instru/conveyor/count` | JSON → chart | Bar chart per warna |
| `instru/system/emergency` | function → notification | Alert merah + audio |
| — | button → MQTT out | Tombol Reset remote |