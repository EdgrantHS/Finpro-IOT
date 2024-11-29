#include <WiFi.h>

const char* ssid = ":/";
const char* password = "AmamyaKokoro";

// Fungsi yang akan dipanggil ketika ESP32 terhubung ke jaringan WiFi
void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
    Serial.println("Connected to WiFi network");
}

// Fungsi yang akan dipanggil ketika ESP32 terputus dari jaringan WiFi
void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
    Serial.println("Disconnected from WiFi network");
    Serial.println("Reconnecting to WiFi network...");
    WiFi.begin(ssid, password);
}

void setup() {
    Serial.begin(115200);

    // Mengatur WiFi Event handler untuk koneksi dan diskoneksi
    WiFi.onEvent(WiFiStationConnected, ARDUINO_EVENT_WIFI_STA_CONNECTED);
    WiFi.onEvent(WiFiStationDisconnected, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

    // Menghubungkan ESP32 ke jaringan WiFi
    WiFi.begin(ssid, password);
}

void loop() {
    // empty
}
