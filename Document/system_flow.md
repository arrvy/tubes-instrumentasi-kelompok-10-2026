# System Flow & Interface Contract Antar Subsistem

> Dokumen ini menjelaskan data antar subsistem supaya tiap modul bisa dikembangkan mandiri.

---

## Block Diagram Sistem

```mermaid
graph TD
    subgraph HARDWARE_POWER["Hardware Power & Emergency"]
        STARTSTOP["START/STOP switch\nlangsung ke power supply"]
        ESTOP["Emergency NC latched\nseri dengan relay"]
    end

    subgraph INPUT
        DHT["DHT11\nGPIO32"]
        IR["IR Sensor zona scan\nGPIO35"]
    end

    subgraph SENSOR
        LED_RGB["LED RGB\nR15 G2 B4"]
        LDR["LDR / ADC\nGPIO33"]
        COLOR_MODULE["Color Sensor Module\nambient comp + normalisasi\n+ nearest neighbor"]
    end

    subgraph CONTROLLER["ESP32 - State Machine Utama"]
        SM["State Machine\nIDLE/RUNNING/SCANNING\nSORTING/EMERGENCY"]
        QUEUE["Object Queue\nColorID + timing"]
    end

    subgraph ACTUATOR
        RELAY["Relay Safety\nGPIO13"]
        MOTOR["Motor Konveyor + L298N\nENB14 IN3-27 IN4-26"]
        SERVO["Servo Sorter\nGPIO16"]
    end

    subgraph UI
        LCD["LCD I2C 16x2\nSDA21 SCL22"]
    end

    subgraph IOT
        MQTT["MQTT Broker\nMosquitto"]
        NODERED["Node-RED Dashboard"]
    end

    STARTSTOP -->|"power on/off"| SM
    ESTOP -->|"hardware cut"| RELAY
    DHT -->|"temperature threshold"| SM
    IR -->|"object trigger"| SM

    LED_RGB -->|"GPIO ON/OFF"| COLOR_MODULE
    LDR -->|"ADC value"| COLOR_MODULE
    COLOR_MODULE -->|"ColorID enum"| SM

    SM -->|"sort command"| SERVO
    SM -->|"PWM speed"| MOTOR
    SM -->|"relay ON/OFF"| RELAY

    RELAY -->|"cuts power"| MOTOR
    RELAY -->|"cuts power"| SERVO

    SM -->|"SystemState"| LCD
    SM -->|"publish"| MQTT
    MQTT -->|"subscribe reset"| SM
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

    IR->>SM: objek terdeteksi
    SM->>SM: transisi RUNNING ke SCANNING
    SM->>LCD: update state SCANNING
    SM->>CS: scan()
    CS->>CS: baca ambient dari LDR
    CS->>CS: nyalakan LED R, G, B bergantian
    CS->>CS: normalisasi + nearest neighbor
    CS-->>SM: ColorReading {r,g,b, ColorID}
    SM->>SM: simpan ke queue, hitung timing
    SM->>SM: transisi SCANNING ke SORTING
    SM->>SS: sortTo(ColorID)
    SS->>SS: gerak ke posisi, tahan, netral
    SS-->>SM: done
    SM->>SM: counter++
    SM->>IOT: publishColor(reading)
    SM->>IOT: publishCount(counter)
    SM->>LCD: update warna + counter
    SM->>SM: transisi SORTING ke RUNNING
```

---

## Interface Contract Per Modul

### Color Sensor ke State Machine

```c
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
    uint16_t adc_r;
    uint16_t adc_g;
    uint16_t adc_b;
    float    r_norm;
    float    g_norm;
    float    b_norm;
    float    nn_distance;
    ColorID  color;
    const char* colorName;
} ColorReading;

ColorReading colorSensor_scan(void);
bool         colorSensor_objectPresent(void);
```

### State Machine ke Servo Sorter

```c
void servoSorter_sortTo(ColorID color);
void servoSorter_setNeutral(void);
```

### State Machine ke IoT

```c
void iot_publishState(SystemState state);
void iot_publishColor(ColorReading* reading);
void iot_publishTemperature(float temperatureC);
void iot_publishCount(uint32_t r, uint32_t g, uint32_t b, uint32_t total);
void iot_publishEmergency(const char* trigger, float value);
```

### Emergency ke State Machine

```c
typedef struct {
    bool temperatureTriggered;
    bool sensorError;
    float temperatureC;
} EmergencyStatus;

EmergencyStatus emergency_getStatus(void);
void            emergency_clearFlags(void);
```
