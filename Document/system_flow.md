# System Flow & Interface Contract Antar Subsistem

> Dokumen ini menjawab: subsistem A menghasilkan data apa, formatnya bagaimana,
> dan siapa yang mengkonsumsinya.
> Ini yang memungkinkan tiap orang coding bagiannya secara independen.

---

## Block Diagram Sistem

```mermaid
graph TD
    subgraph INPUT
        BTN_START[Button START/STOP]
        BTN_ESTOP[Button E-Stop NC]
        MQ2[Sensor Gas MQ-2]
        IR[IR Sensor zona scan]
    end

    subgraph SENSOR
        LED_RGB[LED RGB]
        PHOTO[Photodioda + ADC]
        COLOR_MODULE[Color Sensor Module\nambient comp + normalisasi\n+ nearest neighbor]
    end

    subgraph CONTROLLER["ESP32 — State Machine Utama"]
        SM[State Machine\nIDLE/RUNNING/SCANNING\nSORTING/EMERGENCY]
        QUEUE[Object Queue\nColorID + timing]
    end

    subgraph ACTUATOR
        RELAY[Relay Safety]
        MOTOR[Motor Konveyor\n+ L298N Driver]
        SERVO1[Servo 1]
        SERVO2[Servo 2]
    end

    subgraph UI
        LCD[LCD I2C 16x2]
        LED_IND[LED Indikator\nHijau/Kuning/Merah]
        BUZZER[Buzzer]
    end

    subgraph IOT
        MQTT[MQTT Broker\nMosquitto]
        NODERED[Node-RED Dashboard]
    end

    BTN_START -->|digitalRead| SM
    BTN_ESTOP -->|interrupt| SM
    MQ2 -->|ADC| SM
    IR -->|interrupt| SM

    LED_RGB -->|GPIO ON/OFF| COLOR_MODULE
    PHOTO -->|ADC value| COLOR_MODULE
    COLOR_MODULE -->|ColorID enum| SM

    SM -->|ServoCommand| SERVO1
    SM -->|ServoCommand| SERVO2
    SM -->|PWM speed| MOTOR
    SM -->|relay ON/OFF| RELAY

    RELAY -->|cuts power| MOTOR
    RELAY -->|cuts power| SERVO1
    RELAY -->|cuts power| SERVO2

    SM -->|SystemState| LCD
    SM -->|SystemState| LED_IND
    SM -->|alert trigger| BUZZER

    SM -->|publish| MQTT
    MQTT -->|subscribe reset| SM
    MQTT --> NODERED
```

---

## Data Flow Detail

```mermaid
sequenceDiagram
    participant IR as IR Sensor
    participant SM as State Machine
    participant CS as Color Sensor
    participant SS as Servo Sorter
    participant IOT as IoT/MQTT
    participant LCD as Display

    IR->>SM: objek terdeteksi (interrupt)
    SM->>SM: transisi RUNNING → SCANNING
    SM->>LCD: update state SCANNING
    SM->>CS: scan()
    CS->>CS: baca ambient
    CS->>CS: baca R, G, B ADC
    CS->>CS: normalisasi + nearest neighbor
    CS-->>SM: ColorReading {r,g,b, ColorID}
    SM->>SM: simpan ke queue, hitung timing
    SM->>SM: transisi SCANNING → SORTING
    SM->>SS: sortTo(ColorID)
    SS->>SS: gerak ke posisi, tahan, netral
    SS-->>SM: done
    SM->>SM: counter++
    SM->>IOT: publishColor(reading)
    SM->>IOT: publishCount(counter)
    SM->>LCD: update warna + counter
    SM->>SM: transisi SORTING → RUNNING
```

---

## Interface Contract Per Modul

### Color Sensor → State Machine

```c
// Output dari color_sensor.h
typedef enum {
    COLOR_UNKNOWN    = 0,
    COLOR_RED        = 1,
    COLOR_GREEN      = 2,
    COLOR_BLUE       = 3,
    COLOR_WHITE      = 4,
    COLOR_BLACK      = 5,
    COLOR_NO_OBJECT  = 6
} ColorID;

typedef struct {
    uint16_t adc_r;       // raw ADC setelah kompensasi ambient
    uint16_t adc_g;
    uint16_t adc_b;
    float    r_norm;      // r / (r+g+b)
    float    g_norm;
    float    b_norm;
    float    nn_distance; // jarak ke database terdekat
    ColorID  color;
    const char* colorName;
} ColorReading;

// Fungsi yang dipanggil state machine:
ColorReading colorSensor_scan(void);
bool         colorSensor_objectPresent(void);
```

### State Machine → Servo Sorter

```c
// Input ke servo_sorter.h
void servoSorter_sortTo(ColorID color);
// Internally maps:
//   COLOR_RED   → servo1: 30°,  servo2: 90°
//   COLOR_GREEN → servo1: 150°, servo2: 90°
//   COLOR_BLUE  → servo1: 90°,  servo2: 30°
//   default     → servo1: 90°,  servo2: 90° (netral)
void servoSorter_setNeutral(void);
```

### State Machine → IoT

```c
// Data yang dipublish ke MQTT:
void iot_publishState(SystemState state);
void iot_publishColor(ColorReading* reading);
void iot_publishGas(uint16_t adc);
void iot_publishCount(uint32_t r, uint32_t g, uint32_t b, uint32_t total);
void iot_publishEmergency(const char* trigger, uint16_t value);
```

### Emergency → State Machine (feedback)

```c
// Emergency module mendeteksi dan SET FLAG — state machine baca flag ini
typedef struct {
    bool gasTriggered;
    bool buttonTriggered;
    bool relayFeedbackLost;
    uint16_t gasADC;
} EmergencyStatus;

EmergencyStatus emergency_getStatus(void);
void            emergency_clearFlags(void);   // hanya dipanggil saat reset
```