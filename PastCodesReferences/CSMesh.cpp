#include <Arduino.h>
#include <painlessMesh.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
// #include <WiFiClient.h> 
#include <PubSubClient.h>

// Mesh network parameters
// Ganti AX dan BX menjadi nomor kelompok
// Contohnya kelompok A1 dan B1 ganti menjadi Kelompok_A1_B1
// Untuk Port ganti X dengan nomor kelompok
// Contohnya kelompok A1 dan B1 ganti menjadi 5001
// Atau kelompok A5 dan B5 ganti menjadi 5005 
#define MESH_SSID "Kelompok_A2_B2"
#define MESH_PASSWORD "Kelompok_A2_B2"
#define MESH_PORT 5002

// Ganti network credential dibawah dengan access point untuk mengakses internet
#define SSID ":/"
#define PASSWORD "11112222"
#define HOSTNAME "ESP32_Kelompok_A2_B2"

#define MQTT_SERVER "broker.hivemq.com"
#define MQTT_PORT 8883
// #define MQTT_PORT 1883 // TEMP, unsecure connection
#define MQTT_TOPIC "RTOS/CSIOT9"

const char *server_cert = R"(-----BEGIN CERTIFICATE-----
MIIEkjCCA3qgAwIBAgITBn+USionzfP6wq4rAfkI7rnExjANBgkqhkiG9w0BAQsF
ADCBmDELMAkGA1UEBhMCVVMxEDAOBgNVBAgTB0FyaXpvbmExEzARBgNVBAcTClNj
b3R0c2RhbGUxJTAjBgNVBAoTHFN0YXJmaWVsZCBUZWNobm9sb2dpZXMsIEluYy4x
OzA5BgNVBAMTMlN0YXJmaWVsZCBTZXJ2aWNlcyBSb290IENlcnRpZmljYXRlIEF1
dGhvcml0eSAtIEcyMB4XDTE1MDUyNTEyMDAwMFoXDTM3MTIzMTAxMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaOCATEwggEtMA8GA1UdEwEB/wQFMAMBAf8wDgYDVR0PAQH/
BAQDAgGGMB0GA1UdDgQWBBSEGMyFNOy8DJSULghZnMeyEE4KCDAfBgNVHSMEGDAW
gBScXwDfqgHXMCs4iKK4bUqc8hGRgzB4BggrBgEFBQcBAQRsMGowLgYIKwYBBQUH
MAGGImh0dHA6Ly9vY3NwLnJvb3RnMi5hbWF6b250cnVzdC5jb20wOAYIKwYBBQUH
MAKGLGh0dHA6Ly9jcnQucm9vdGcyLmFtYXpvbnRydXN0LmNvbS9yb290ZzIuY2Vy
MD0GA1UdHwQ2MDQwMqAwoC6GLGh0dHA6Ly9jcmwucm9vdGcyLmFtYXpvbnRydXN0
LmNvbS9yb290ZzIuY3JsMBEGA1UdIAQKMAgwBgYEVR0gADANBgkqhkiG9w0BAQsF
AAOCAQEAYjdCXLwQtT6LLOkMm2xF4gcAevnFWAu5CIw+7bMlPLVvUOTNNWqnkzSW
MiGpSESrnO09tKpzbeR/FoCJbM8oAxiDR3mjEH4wW6w7sGDgd9QIpuEdfF7Au/ma
eyKdpwAJfqxGF4PcnCZXmTA5YpaP7dreqsXMGz7KQ2hsVxa81Q4gLv7/wmpdLqBK
bRRYh5TmOTFffHPLkIhqhBGWJ6bt2YFGpn6jcgAKUj6DiAdjd4lpFw85hdKrCEVN
0FE6/V1dN2RMfjCyVSRCnTawXZwXgWHxyvkQAiSr6w10kY17RSlQOYiypok1JR4U
akcjMS9cmvqtmg5iUaQqqcT5NJ0hGA==
-----END CERTIFICATE-----)";

const char *client_cert = R"(-----BEGIN CERTIFICATE-----
MIIDazCCAlOgAwIBAgIUbWy5H7TNMuFPfusBtB9TvsOpducwDQYJKoZIhvcNAQEL
BQAwRTELMAkGA1UEBhMCQVUxEzARBgNVBAgMClNvbWUtU3RhdGUxITAfBgNVBAoM
GEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDAeFw0yNDExMTUwNDI3MzBaFw0yNTEx
MTUwNDI3MzBaMEUxCzAJBgNVBAYTAkFVMRMwEQYDVQQIDApTb21lLVN0YXRlMSEw
HwYDVQQKDBhJbnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQwggEiMA0GCSqGSIb3DQEB
AQUAA4IBDwAwggEKAoIBAQDp65k5onzsAeko4jWU7xzkUHzaFjZzojINTIL9jlpp
mfGgezPgzWpAGy5KiXXhPRJSHN2filHHZT6saoYktlaA79/2cSbMMOAu/lf0xCQp
CTndwOtvmX8KeSnelc1cwyX9S3ArINM/LJDBH4EQU/+sPIR21HzKGXMyOmBE2o1h
70zg2K4wO2JxNzBI4oYK6YLF097pnC/zGDoDkA4zf18T43i4Jhz7O8GG4K7wbjif
sok3CLZQrNCylKEMLDgTqxT3EEm1MZ1UXhxA6BCYd88dRRG3doeSrfB9RwXomQ7Y
9cOIivnkQJ6xlZMvj/EUfl4gCWszFDM5xVGyXyCLl+MdAgMBAAGjUzBRMB0GA1Ud
DgQWBBQ920Lzuv+jBTbkTmxJRSdfa8CVnjAfBgNVHSMEGDAWgBQ920Lzuv+jBTbk
TmxJRSdfa8CVnjAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IBAQA8
ZE6B7mOaXdrciIhrPEGztl2AngjAFcQJ5lkJ4XBQ9EMc1zHe0HnCGD0liHbBtRne
rOTZQi6nE5di+eAe1QlDFb2Vs/CzKbqXSUNLtyiYZ/LhFOC47TPB8mJm4Bx3DN4Y
1SBO3cvmhQP4p23b4PAqca1mG29b9oihMY2dgqsRM/3jgdtFkEcHsdN6MNwGs9j4
OMCyoBW5C8TEqhFk2EFBQ6Pq+wyYG7FR1WFNEs4nMrotHP0RCvLRbawYlphWZkBG
t3e3ZQSDxNzy4nyYpuU7Eie8WQdx9r5ap21H1CivNSaSZgPPsx5KeMeRyD2k/bBZ
L/zbnMrguBlBs0DFuaUw
-----END CERTIFICATE-----)";

const char *client_key = R"(-----BEGIN PRIVATE KEY-----
MIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQDp65k5onzsAeko
4jWU7xzkUHzaFjZzojINTIL9jlppmfGgezPgzWpAGy5KiXXhPRJSHN2filHHZT6s
aoYktlaA79/2cSbMMOAu/lf0xCQpCTndwOtvmX8KeSnelc1cwyX9S3ArINM/LJDB
H4EQU/+sPIR21HzKGXMyOmBE2o1h70zg2K4wO2JxNzBI4oYK6YLF097pnC/zGDoD
kA4zf18T43i4Jhz7O8GG4K7wbjifsok3CLZQrNCylKEMLDgTqxT3EEm1MZ1UXhxA
6BCYd88dRRG3doeSrfB9RwXomQ7Y9cOIivnkQJ6xlZMvj/EUfl4gCWszFDM5xVGy
XyCLl+MdAgMBAAECggEACsAtNpzlKOOdl6dt1v52UXfxhQRoVEAsFLhjfMvCFABj
PoDrDXXjYDbflcVjiYqJAQUamm58+7EHhF0Q9Tb8GsjrfiQNKG1GoGJIIJOzJb+q
zSpEp8hiMlUHO5ee7Jh7cny5FwJXMxwVOwr7n4h8w6m0XNG/OnqY9MICEqgIv2tt
ll4+lK7byY/cqiHpRbm32LUHpiluUocFdEs7QIt6Uo7wDR1SamnrhKtXy8nQXLFP
wmYdjQlxqM+9u2XPUDiFLB69wY0L4LmdLg5MouPFt5BuLhyYlYWo1GMCvwTewBv8
DvkhAozo08NE+mL8Ui2Jg4GD5K0uFtPaBleKSyeu8QKBgQDrXlHwUeUZ/ZYrydN7
4kko5LQySyAhi6GWjs+MUHrtN5nzSogcCKptAvQku7WfvfnQT8qLOW/Oap0lC1bU
mlIwzJEMDQware5Vy2LptAw1Vb3JXhQ0v+8UPaSUa5klAMw6VKDkvBGfeJPAtsDi
vZvAvWyVWtaK86LkltPldqnHMwKBgQD+bMhGoQT9SCRl5jEWbboOMvubWWDRNmSf
GDiOfB0E5Vv/eWZJuWxy5JK/UM1Solgho8TNVITapwpOhR9FuUbBGCVUEfrkaXRf
eaThb8ZScP4gBCeedQ/qA3kKTcalxDYZx7OwNkygpyZ58CR/yqiBazHLdY6tCG8w
EN8mdEtsbwKBgElhDoqt/Y8s0DS2p4hn9AcbxlInucy7i5U00OAd9zUdsJF5GxYi
XX+++/63xtgWkluvhKYDMihYdMWn01pVAmrUXCQ1rSBkOXnl/uB9kZDPOmwdOI95
hz/4N+dN5GD07rcAy2iEeboODYJ3d4s5MeXVKJUnzNtlOdOqckWHyUahAoGAFfca
S063eY5y5gE7l64ddABezIio3ScPBNU4fMSmVLfge2vlstO5Uyn8qVu2fj3Z0f2r
jfaQCbiGIUVI7+IRA7ar8lgjCvk3vM5pt7TIsHFk3yq8qOd+Wju2hXc1gTYxXYRq
NPpbHzuPDNP4sreyWIoCoIgjqzihMMskGNPNdy0CgYAwtUeWLASfYw6UcqYzaoDs
3V4KDLYbRvlr9pBWzOPvuH1j525FNmW2gjfPZ/9dNYukFrd84ahUdmAe4UooKvgs
sS8we8mqvBPWZkKaBXiLz/ZZ+nurj5z2mrP7rvpTbzCzjIf+gBl7Z1gEY1jYZnV4
I99svKpHiBQkH7kU9Rehpg==
-----END PRIVATE KEY-----)";

void taskNTP(void *pvParameters);
void taskMQTT(void *pvParameters);
void receiveCallback(uint32_t from, String &msg);
void reconnectMQTT();
void callback(char* topic, byte* payload, unsigned int length);
void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info);
void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info);
void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);

painlessMesh mesh;
QueueHandle_t mqttQueue; // Queue untuk mengirim data DHT ke taskMQTT
WiFiClientSecure wifiClient;
// WiFiClient wifiClient;
PubSubClient mqttClient(MQTT_SERVER, MQTT_PORT, callback, wifiClient);

struct DHTData {
    float temperature;
    float humidity;
};

String currentTime = ""; // Global variable untuk menyimpan waktu dari NTP

void setup() {
    Serial.begin(115200);
    
    pinMode(LED_BUILTIN, OUTPUT);

    // Set certificate
    // wifiClient.setCACert(server_cert);
    // wifiClient.setCertificate(client_cert);
    // wifiClient.setPrivateKey(client_key);

    wifiClient.setInsecure();

    configTime(7 * 3600, 0, "id.pool.ntp.org");

    // Initialize queue
    mqttQueue = xQueueCreate(5, sizeof(DHTData));

    // Create FreeRTOS tasks
    xTaskCreate(taskNTP, "Task_NTP", 4096, NULL, 1, NULL);
    xTaskCreate(taskMQTT, "Task_MQTT", 25000, NULL, 1, NULL);

    // TODO 4: Setup mesh
    mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages

    mesh.init(MESH_SSID, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, 6); // channel 6
    mesh.onReceive(&receiveCallback);

    // Start the mesh network
    mesh.stationManual(SSID, PASSWORD);
    mesh.setHostname(HOSTNAME);
    mesh.setRoot(true);
    mesh.setContainsRoot(true);

    Serial.println("Mesh network started");

    // Print local IP address
    Serial.print("Local IP address: ");
    Serial.println(WiFi.localIP());

    // Print hostname
    Serial.print("Hostname: ");
    Serial.println(WiFi.getHostname());
}

void loop() {
    mesh.update();
}

// TODO 1: Task untuk mengambil waktu dari NTP server setiap 5 detik
void taskNTP(void *pvParameters) {
   while (1) {
        // Get current time
        time_t now;
        struct tm timeinfo;
        if (!getLocalTime(&timeinfo)) {
            Serial.println("Failed to obtain time");
            currentTime = "Failed to obtain time";
        } else {
            char strftime_buf[64];
            strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
            Serial.println(strftime_buf);
            currentTime = strftime_buf;
        }
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    } 
}

// TODO 2: Function callback untuk menerima dan mem-proses message dari leaf node
void receiveCallback(uint32_t from, String &msg) {
    Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
    DHTData data;
    data.temperature = 0;
    data.humidity = 0;
    xQueueSend(mqttQueue, &data, portMAX_DELAY); 
}

// TODO 3: Task untuk mengirim message ke MQTT broker dengan DHT data dan waktu sekarang, serta subscribe ke topic yang sama
void taskMQTT(void *pvParameters) {
    while (1) {
        DHTData data;
        if (xQueueReceive(mqttQueue, &data, 1000 / portTICK_PERIOD_MS) == pdTRUE) {
            String message = "Temperature: " + String(data.temperature) + " Humidity: " + String(data.humidity) + " Time: " + currentTime;
            if (!mqttClient.connected()) {
                reconnectMQTT();
            }
            mqttClient.publish(MQTT_TOPIC, message.c_str());
            Serial.println("Message sent to MQTT broker");
            Serial.print("Published topic: ");
            Serial.println(MQTT_TOPIC);
            Serial.print("Published message: ");
            Serial.println(message);
            mqttClient.subscribe(MQTT_TOPIC);
        }
        else {
            if (!mqttClient.connected()) {
                reconnectMQTT();
            }
            Serial.println("Received data from mesh");
            mqttClient.publish(MQTT_TOPIC, "Group A2 and B2 sent at Friday, 15 November 2024 11:36:58 | Temp : 1.90C | Humid : 4.80%");
        }
        mqttClient.loop();
        

        // // tes kirim data
        // String message = "Temperature: 25 Humidity: 50 Time: " + currentTime;

        // if (!mqttClient.connected()) {
        //     reconnectMQTT();
        // }

        // mqttClient.publish(MQTT_TOPIC, message.c_str());
        // Serial.println("Message sent to MQTT broker");

        // mqttClient.loop();
    }
}

// Gunakan function ini untuk connect ke MQTT broker
void reconnectMQTT() {
    if (!WiFi.isConnected()) return;
    Serial.println("Connecting to MQTT...");

    if (mqttClient.connect(String(ESP.getEfuseMac()).c_str())) {
        Serial.println("Connected to MQTT broker");
    } else {
        Serial.print("MQTT connection failed, rc=");
        Serial.println(mqttClient.state());
    }
}

// Function callback untuk menerima message dari MQTT
void callback(char* topic, byte* payload, unsigned int length) {
    Serial.println("MQTT message Received: ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}