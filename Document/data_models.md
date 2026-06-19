# Data Models — Struct, Enum, Typedef

> Semua tipe data yang dipakai lintas modul didefinisikan di sini.
> File aktualnya ada di `firmware/src/config.h` dan header masing-masing modul.
> Jika ada perubahan struct, update di sini dulu lalu propagate ke kode.

---

## Enumerasi Utama

```c
// State machine
typedef enum {
    STATE_IDLE          = 0,
    STATE_STARTING      = 1,
    STATE_RUNNING       = 2,
    STATE_SCANNING      = 3,
    STATE_SORTING       = 4,
    STATE_EMERGENCY     = 5
} SystemState;

// Hasil klasifikasi warna
typedef enum {
    COLOR_UNKNOWN   = 0,
    COLOR_RED       = 1,
    COLOR_GREEN     = 2,
    COLOR_BLUE      = 3,
    COLOR_WHITE     = 4,
    COLOR_BLACK     = 5,
    COLOR_NO_OBJECT = 6
} ColorID;

// Trigger penyebab emergency
typedef enum {
    EMERGENCY_NONE          = 0,
    EMERGENCY_GAS_SENSOR    = 1,
    EMERGENCY_BUTTON_NC     = 2,
    EMERGENCY_RELAY_LOST    = 3
} EmergencyTrigger;
```

---

## Struct Data

```c
// Hasil pembacaan sensor warna (output dari color_sensor module)
typedef struct {
    uint16_t  adc_r;        // raw ADC LED merah (setelah ambient compensation)
    uint16_t  adc_g;        // raw ADC LED hijau
    uint16_t  adc_b;        // raw ADC LED biru
    uint16_t  adc_ambient;  // nilai ambient (semua LED mati)
    float     r_norm;       // r / (r+g+b)
    float     g_norm;
    float     b_norm;
    float     nn_distance;  // Euclidean distance ke database terdekat
    ColorID   color;
    const char* colorName;
} ColorReading;

// Entry database warna referensi (disimpan di config.h)
typedef struct {
    float    r_norm;
    float    g_norm;
    float    b_norm;
    ColorID  color;
    const char* name;
} ColorReference;

// Counter objek per sesi
typedef struct {
    uint32_t red;
    uint32_t green;
    uint32_t blue;
    uint32_t white;
    uint32_t black;
    uint32_t unknown;
    uint32_t total;
} SortCounter;

// Status emergency
typedef struct {
    bool             active;
    EmergencyTrigger trigger;
    uint16_t         gasADC;       // nilai gas saat trigger
    uint32_t         timestamp;    // millis() saat trigger
} EmergencyStatus;

// Perintah servo (internal servo_sorter module)
typedef struct {
    uint8_t servo1_deg;   // target sudut servo 1 (0-180)
    uint8_t servo2_deg;   // target sudut servo 2 (0-180)
    uint16_t hold_ms;     // durasi tahan sebelum kembali netral
} ServoCommand;
```

---

## Database Warna Referensi

Format array of struct, hardcode di `config.h`, diisi dari hasil kalibrasi:

```c
// Di config.h — isi dari hasil data_kalibrasi.csv
static const ColorReference COLOR_DB[] = {
    // {r_norm, g_norm, b_norm, ColorID, name}
    { 0.60f, 0.25f, 0.15f, COLOR_RED,   "MERAH" },
    { 0.20f, 0.60f, 0.20f, COLOR_GREEN, "HIJAU" },
    { 0.15f, 0.25f, 0.60f, COLOR_BLUE,  "BIRU"  },
    { 0.33f, 0.33f, 0.34f, COLOR_WHITE, "PUTIH" },
    // Hitam: bedakan dari putih berdasarkan magnitude ADC total, bukan rasio
};
#define COLOR_DB_SIZE   4
#define UNKNOWN_THRESHOLD  0.15f   // jika nn_distance > ini → UNKNOWN
```

---

## Nearest Neighbor — Formula

```c
// Euclidean distance antara pembacaan dan satu entry database
float nn_distance(ColorReading* reading, ColorReference* ref) {
    float dr = reading->r_norm - ref->r_norm;
    float dg = reading->g_norm - ref->g_norm;
    float db = reading->b_norm - ref->b_norm;
    return sqrtf(dr*dr + dg*dg + db*db);
}

// Klasifikasi: cari distance terkecil
// Jika distance terkecil > UNKNOWN_THRESHOLD → return COLOR_UNKNOWN
```

---

## Servo Position Map

```c
// Di config.h
#define SERVO_NEUTRAL    90

static const ServoCommand SERVO_MAP[] = {
    // {servo1_deg, servo2_deg, hold_ms}
    [COLOR_UNKNOWN]   = { 90,  90, 0    },  // netral, tidak sort
    [COLOR_RED]       = { 30,  90, 1000 },
    [COLOR_GREEN]     = { 150, 90, 1000 },
    [COLOR_BLUE]      = { 90,  30, 1000 },
    [COLOR_NO_OBJECT] = { 90,  90, 0    },
};
```