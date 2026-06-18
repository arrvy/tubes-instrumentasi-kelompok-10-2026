# Decision Log

> Kenapa pilih ini bukan itu. Biar tidak re-invent keputusan yang sama dua kali.

---

| Tanggal | Keputusan | Alternatif yang dipertimbangkan | Alasan dipilih |
|---------|-----------|--------------------------------|----------------|
| Jun 2026 | Normalisasi `r/(r+g+b)` bukan threshold absolut | Threshold absolut ADC | Threshold absolut tidak stabil jika jarak sensor berubah atau LED intensitas berubah |
| Jun 2026 | Nearest Neighbor classification | Threshold ratio sederhana | NN lebih robust, mudah ditambah warna baru tanpa ubah logic |
| Jun 2026 | Relay konfigurasi NO (Normally Open) | NC (Normally Closed) | NO = fail-safe: ESP32 mati → relay lepas → aktuator mati otomatis |
| Jun 2026 | MQTT + Mosquitto lokal, bukan Blynk | Blynk | Blynk tergantung server pihak ketiga; MQTT lebih fleksibel, bisa offline |
| Jun 2026 | PubSubClient tanpa ArduinoJson | ArduinoJson | snprintf JSON manual cukup untuk payload sederhana, hemat memory |
| Jun 2026 | Update LCD via `millis()` bukan `delay()` | delay() | delay() block seluruh loop, emergency check tidak jalan |
| Jun 2026 | Multi-servo (per warna) bukan single servo | Single servo multi-sudut | Kalibrasi mekanik lebih mudah, logika lebih simpel |
| Jun 2026 | ADC1 only untuk semua sensor | ADC2 | ADC2 conflict dengan WiFi di ESP32, tidak bisa dipakai bersamaan |
| Jun 2026 | Button NC untuk E-Stop | Button NO | NC = fail-safe: kabel putus → circuit terbuka → trigger emergency |
| Jun 2026 | [kosong — isi saat ada keputusan baru] | | |