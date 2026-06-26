/**
 * ============================================================
 * ColorSensor.h
 * ============================================================
 * Custom RGB Photodiode Color Sensor Library
 * Platform : ESP32
 * Author   : TUBES Instrumentasi
 * ============================================================
 */

#pragma once

#include <Arduino.h>

class ColorSensor
{
public:

    enum class State
    {
        IDLE,
        READ_AMBIENT,
        READ_RED,
        READ_GREEN,
        READ_BLUE,
        PROCESS,
        DONE
    };

    enum class ColorID
    {
        UNKNOWN,
        RED,
        GREEN,
        BLUE,
        WHITE,
        BLACK
    };

    struct RGBRaw
    {
        uint16_t ambient;
        uint16_t red;
        uint16_t green;
        uint16_t blue;
    };

    /**
    * @brief Data warna hasil normalisasi.
    *
    * Nilai ADC mentah sangat dipengaruhi oleh:
    *
    * - Intensitas LED
    * - Jarak sensor
    * - Tegangan supply
    * - Sensitivitas photodiode
    *
    * Oleh karena itu proses klasifikasi menggunakan data
    * hasil normalisasi agar lebih robust terhadap perubahan
    * kondisi pengukuran.
    *
    * Nilai setiap kanal berada pada rentang 0.0 - 1.0.
    */
    struct RGBNormalized
    {
        float r;
        float g;
        float b;
    };

    struct ColorReference
    {
        ColorID id;
        float r;
        float g;
        float b;
    };

    struct Config
    {
        uint8_t pinLedR;
        uint8_t pinLedG;
        uint8_t pinLedB;

        uint8_t pinPhotodiode;

        uint32_t sampleDelayMs;
    };

    /**
     * @brief Create the default demo configuration.
     */
    static Config defaultConfig();

public:

  /**
 * @brief Membuat instance ColorSensor.
 *
 * Seluruh konfigurasi hardware dan parameter runtime
 * diberikan melalui Config agar library tidak memiliki
 * dependency terhadap variabel global.
 *
 * Pendekatan ini mempermudah integrasi ke proyek besar,
 * unit testing, dan pemindahan library antar platform.
 *
 * @param config Struktur konfigurasi sensor.
 */
    explicit ColorSensor(const Config& config);

/**
 * @brief Inisialisasi hardware sensor warna.
 *
 * Function ini harus dipanggil satu kali pada setup().
 *
 * Tugas utama:
 * - Mengatur mode pin LED RGB.
 * - Memastikan seluruh LED berada pada kondisi OFF.
 * - Menyiapkan state machine internal.
 *
 * Function ini tidak melakukan proses pembacaan warna.
 */
    void begin();

/**
 * @brief Menjalankan state machine sensor warna.
 *
 * Function ini dirancang non-blocking dan harus dipanggil
 * secara periodik pada loop utama.
 *
 * Library tidak menggunakan delay() sehingga seluruh
 * tahapan pembacaan sensor dijalankan secara bertahap
 * menggunakan timing berbasis millis().
 *
 * Urutan proses:
 *
 * IDLE
 *   ↓
 * READ_AMBIENT
 *   ↓
 * READ_RED
 *   ↓
 * READ_GREEN
 *   ↓
 * READ_BLUE
 *   ↓
 * PROCESS
 *   ↓
 * DONE
 *
 * Ketika state mencapai DONE maka hasil klasifikasi
 * warna dapat diambil menggunakan getColor().
 */
    void update();

    bool readColor();

    ColorID getColor() const;

    RGBRaw getRawData() const;
    RGBNormalized getNormalizedData() const;

    State getState() const;

    const char* colorToString(ColorID color) const;

private:

    void setAllLedOff();

    /**
     * @brief Melakukan klasifikasi warna objek.
     *
     * Tahapan:
     *
     * 1. Menghilangkan pengaruh ambient light.
     * 2. Melakukan normalisasi RGB.
     * 3. Menghitung jarak terhadap seluruh warna referensi.
     * 4. Memilih warna dengan jarak terkecil.
     *
     * Algoritma yang digunakan adalah Euclidean Nearest
     * Neighbor karena ringan dan cocok untuk mikrokontroler.
     */
    void processClassification();

    float calculateDistance(
        const RGBNormalized& sample,
        const ColorReference& ref);

private:

    Config _config;

    State _state;

    RGBRaw _raw;
    RGBNormalized _normalized;

    ColorID _detectedColor;

    uint32_t _previousMillis;

    static constexpr uint8_t COLOR_COUNT = 5;

    /**
     * Database warna referensi.
     *
     * Nilai referensi diperoleh dari proses kalibrasi
     * menggunakan sampel warna yang diketahui.
     *
     * Pada implementasi awal database disimpan sebagai
     * static array untuk mengurangi kompleksitas sistem.
     *
     * Pengembangan berikutnya dapat memindahkan database
     * ke EEPROM, Preferences (NVS), SD Card, atau server IoT.
     */
    static const ColorReference _database[COLOR_COUNT];
};
