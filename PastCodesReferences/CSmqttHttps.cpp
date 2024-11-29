#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <time.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

// TODO 1 : Isi ssid dan password wifi yang digunakan, serti ketiga certificate (server_cert, client_cert, dan client_key)
const char ssid[] = ":/";
const char password[] = "AmamyaKokoro";

// const char* test_root_ca = \
// "-----BEGIN CERTIFICATE-----\n" \
// "MIIDejCCAmKgAwIBAgIQf+UwvzMTQ77dghYQST2KGzANBgkqhkiG9w0BAQsFADBX" \
// "MQswCQYDVQQGEwJCRTEZMBcGA1UEChMQR2xvYmFsU2lnbiBudi1zYTEQMA4GA1UE" \
// "CxMHUm9vdCBDQTEbMBkGA1UEAxMSR2xvYmFsU2lnbiBSb290IENBMB4XDTIzMTEx" \
// "NTAzNDMyMVoXDTI4MDEyODAwMDA0MlowRzELMAkGA1UEBhMCVVMxIjAgBgNVBAoT" \
// "GUdvb2dsZSBUcnVzdCBTZXJ2aWNlcyBMTEMxFDASBgNVBAMTC0dUUyBSb290IFI0" \
// "MHYwEAYHKoZIzj0CAQYFK4EEACIDYgAE83Rzp2iLYK5DuDXFgTB7S0md+8Fhzube" \
// "Rr1r1WEYNa5A3XP3iZEwWus87oV8okB2O6nGuEfYKueSkWpz6bFyOZ8pn6KY019e" \
// "WIZlD6GEZQbR3IvJx3PIjGov5cSr0R2Ko4H/MIH8MA4GA1UdDwEB/wQEAwIBhjAd" \
// "BgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIwDwYDVR0TAQH/BAUwAwEB/zAd" \
// "BgNVHQ4EFgQUgEzW63T/STaj1dj8tT7FavCUHYwwHwYDVR0jBBgwFoAUYHtmGkUN" \
// "l8qJUC99BM00qP/8/UswNgYIKwYBBQUHAQEEKjAoMCYGCCsGAQUFBzAChhpodHRw" \
// "Oi8vaS5wa2kuZ29vZy9nc3IxLmNydDAtBgNVHR8EJjAkMCKgIKAehhxodHRwOi8v" \
// "Yy5wa2kuZ29vZy9yL2dzcjEuY3JsMBMGA1UdIAQMMAowCAYGZ4EMAQIBMA0GCSqG" \
// "SIb3DQEBCwUAA4IBAQAYQrsPBtYDh5bjP2OBDwmkoWhIDDkic574y04tfzHpn+cJ" \
// "odI2D4SseesQ6bDrarZ7C30ddLibZatoKiws3UL9xnELz4ct92vID24FfVbiI1hY" \
// "+SW6FoVHkNeWIP0GCbaM4C6uVdF5dTUsMVs/ZbzNnIdCp5Gxmx5ejvEau8otR/Cs" \
// "kGN+hr/W5GvT1tMBjgWKZ1i4//emhA1JG1BbPzoLJQvyEotc03lXjTaCzv8mEbep" \
// "8RqZ7a2CPsgRbuvTPBwcOMBBmuFeU88+FSBX6+7iP0il8b4Z0QFqIwwMHfs/L6K1" \
// "vepuoxtGzi4CZ68zJpiq1UvSqTbFJjtbD4seiMHl" \

// "-----END CERTIFICATE-----\n";
// sementara pakai insecure
// const char *server_cert = R"(-----BEGIN CERTIFICATE-----
// MASUKAN SERTIFIKAT ANDA DISINI 
// server_cert didapat dari command : 

// openssl s_client -connect broker.hivemq.com:8883 -showcerts

// terus ambil certificate paling akhir
// -----END CERTIFICATE-----)";

// const char *client_cert = R"(-----BEGIN CERTIFICATE-----
// MASUKAN SERTIFIKAT ANDA DISINI 
// client_cert dan client_key = public key dan private key dari key yang kita generate sendiri. 
// caranya dengan menggunakan command : 

// openssl req -newkey rsa:2048 -nodes -keyout client_key.pem -x509 -days 365 -out client_cert.pem 

// command di atas akan menghasilkan 2 file, client_cert.pem dan client_key.pem
// isi client_cert dengan isi file client_cert.pem
// -----END CERTIFICATE-----)";

// const char *client_key = R"(-----BEGIN PRIVATE KEY-----
// MASUKAN SERTIFIKAT ANDA DISINI 
// isi client_key dengan isi file client_key.pem
// -----END PRIVATE KEY-----)";


// mqtt
const char mqttServer[] = "broker.hivemq.com";
const int mqttPort = 8883;
const char mqttTopic[] = "RTOS-IoT/Modul7";

// base url dari api yang akan di-hit
const char serverName[] = "https://jsonplaceholder.typicode.com";

// server NTP 
const char ntpServer[] = "id.pool.ntp.org";

// Function prototypes
void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info);
void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info);
void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);
void updateTimeTask(void *pvParameters);
void httpTask(void *pvParameters);
void mqttTask(void *pvParameters);
String getPost();
void createPost();
void reconnectMQTT();
void callback(char* topic, byte* payload, unsigned int length);
void replaceNewlineWithSpace(char *str);

WiFiClientSecure wifiClient;
PubSubClient mqttClient(mqttServer, mqttPort, callback, wifiClient);

// queue untuk kirim data dari httpTask ke mqttTask
QueueHandle_t httpQueue; 

// variabel untuk menyimpan waktu dari NTP server
String currentTime;

// Task handlers
TaskHandle_t timeUpdateTaskHandle;
TaskHandle_t httpTaskHandle;
TaskHandle_t mqttTaskHandle;

// struct yang akan digunakan untuk mengirim data via Queue
struct Post{
    char title[100];
    char body[1000];
};

void setup() {
    Serial.begin(115200);

    Serial.println("Connecting to WiFi...");
    // connecting to wifi
    WiFi.begin(ssid, password);

    // TODO 5 : Attach function - function WiFi Event yang telah didefinisikan ke WiFi dengan function WiFi.onEvent
    WiFi.onEvent(WiFiStationConnected, ARDUINO_EVENT_WIFI_STA_CONNECTED);
    WiFi.onEvent(WiFiGotIP, ARDUINO_EVENT_WIFI_STA_GOT_IP);
    WiFi.onEvent(WiFiStationDisconnected, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

    // // TODO 6 : Set server NTP yang digunakan mengunakan configTime
    configTime(7 * 3600, 0, ntpServer);


    // // TODO 8 : Buatlah task menggunakan xTaskCreate untuk menjalankan function updateTimeTask
    xTaskCreate(updateTimeTask, "updateTimeTask", 8192, NULL, 1, &timeUpdateTaskHandle);

    // TODO 9 : Buatlah queue menggunakan xQueueCreate untuk mengirim response HTTP GET ke mqtt task dalam bentuk struct Post
    httpQueue = xQueueCreate(5, sizeof(Post));

    // TODO 13 : Buatlah task menggunakan xTaskCreate untuk menjalankan function httpTask
    xTaskCreate(httpTask, "httpTask", 8192, NULL, 1, &httpTaskHandle);

    // TODO 15 : Set ketiga certificate pada wifiClient dan connect wifi dengan WiFi.begin
    // wifiClient.setCACert(server_cert);
    wifiClient.setInsecure();

    if (!wifiClient.connect("jsonplaceholder.typicode.com", 443)) {
        Serial.println("Connection failed");
        return;
    }

    // TODO 16 : Buatlah task menggunakan xTaskCreate untuk menjalankan function mqttTask
    // xTaskCreate(mqttTask, "mqttTask", 8192, NULL, 1, &mqttTaskHandle);
    // ini tidak jadi, mqttTask jadinya task yang bunuh diri saat dipanggil httpTask



}

void loop() {
    if (!mqttClient.connected()) {
        reconnectMQTT();
    }
    else {
        mqttClient.loop();
    }
}

// TODO 2 : Definisikan function WiFi Event untuk mem-print informasi bahwa koneksi WiFi berhasil
void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info){
    Serial.println("Connected to WiFi");
}

// TODO 3 : Definisikan function WiFi Event untuk mem-print local IP Address yang didapatkan
void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

// TODO 4 : Definisikan function WiFi Event untuk mem-print bahwa koneksi WiFi terputus, lalu print alasan koneksi terputus, lalu coba reconnect ke ssid dan password yang sama
void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
    Serial.print("WiFi Disconnected | ");
    Serial.print("Reason: ");
    Serial.println(info.wifi_sta_disconnected.reason);

    Serial.println("Reconnecting to WiFi...");
    WiFi.begin(ssid, password);    
}

// TODO 7 : Definisikan function yang mengupdate waktu yang diambil dari NTP setiap 5 detik (disimpan di string currentTime)
// Simpan dalam format seperti ini : Tuesday, 29 October 2024 23:07:02
void updateTimeTask(void *pvParameters) {
    while (true) {
        // get current time
        time_t now;
        struct tm timeinfo;
        if (!getLocalTime(&timeinfo)) {
            Serial.println("Failed to obtain time");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        // print current time
        char strftime_buf[64];
        strftime(strftime_buf, sizeof(strftime_buf), "%A, %d %B %Y %H:%M:%S", &timeinfo);
        Serial.println(strftime_buf);

        // update currentTime
        currentTime = strftime_buf;

        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

/*
TODO 12 : Definisikan function yang akan menerima input dari serial monitor

call function createPost jika mendapatkan string "POST" dari serial input

call function getPost jika mendapatkan string "GET" dari serial input, 
parse (deserializeJson) response yang didapat menggunakan ArduinoJSON, 
lalu kirim struct Post pada queue yang berisi title dan body 
*/ 
void httpTask(void *pvParameters) {
    while (true) {
        if (Serial.available() > 0) {
            String input = Serial.readStringUntil('\n');
            input.trim();

            if (input == "POST") {
                createPost();
            } else if (input == "GET") {
                String response = getPost();
                if (response == "") {
                    continue;
                }

                // parse response
                JsonDocument doc;
                DeserializationError error = deserializeJson(doc, response);

                if (error) {
                    Serial.print("deserializeJson() failed: ");
                    Serial.println(error.c_str());
                    continue;
                }

                // extract data
                Post post;
                strcpy(post.title, doc["title"]);
                strcpy(post.body, doc["body"]);

                Serial.println("Sending to QUEUE");
                Serial.print("Sent");
                Serial.println(post.title);
                Serial.println(post.body);
                // // send to queue
                // xQueueSend(httpQueue, &post, portMAX_DELAY);

                // Serial.println("Data sent to QUEUE");

                // Call mqttTask
                mqttTask(&post);
            }
        }
    }
}

/*
TODO 10 : Definisikan function yang akan melakukan GET request ke endpoint /posts/[angka random dalam range 1 sampai 100], contoh : 
/posts/1
/posts/15
/posts/34
/posts/83

angka random dapat di-generate dengan function random(1, 100)

API yang akan kita hit adalah "https://jsonplaceholder.typicode.com"

jadi nanti hitnya https://jsonplaceholder.typicode.com/posts/1 dan sebagainya

return value dari function ini adalah reponse yang belum di-parsing (gunakan method getString() dari instance HTTPClient)
*/ 
String getPost() {
    HTTPClient http;
    http.begin(String(serverName) + String("/posts/") + String(random(1, 100)));
    int getResponse = http.GET();
    String response = http.getString();

    //error handling
    if (getResponse <= 0) {
        Serial.println("Error on HTTP request");
        Serial.println(http.errorToString(getResponse).c_str());
        return "";
    }
    
    Serial.println("HTTP GET Request Success");

    http.end();

    Serial.print("Response: ");
    Serial.println(response);

    return response;
}

/*
TODO 11 : Definisikan function yang akan melakukan POST request ke endpoint /posts 

payload yang dikirim dalam bentuk JSON yang kalian serialize dengan ArduinoJSON (https://arduinojson.org/v7/assistant)

payload yang dikirim format berikut : 

{
    "title": "bebas mau isi apa",
    "body": "bebas mau isi apa",
    "author": "bebas mau isi apa",
    "userId": 1
}

Jika request sukses, print response-nya (tidak perlu di-parsing)
*/ 
void createPost() {
    HTTPClient http;
    http.begin(String(serverName) + "/posts");
    http.addHeader("Content-Type", "application/json");

    // serialize JSON
    JsonDocument doc;
    doc["title"] = "bebas mau isi apa";
    doc["body"] = "bebas mau isi apa";
    doc["author"] = "bebas mau isi apa";
    doc["userId"] = 1;

    String output;
    serializeJson(doc, output);

    int postResponse = http.POST(output);
    String response = http.getString();

    //error handling
    if (postResponse <= 0) {
        Serial.println("Error on HTTP request");
        Serial.println(http.errorToString(postResponse).c_str());
        return;
    }
    
    Serial.println("HTTP POST Request Success");

    http.end();

    Serial.print("Response: ");
    Serial.println(response);
}

/*
TODO 14 : Definisikan function untuk publish MQTT message setiap kali ada response pada httpQueue.

payload yang dikirim adalah dalam format berikut 

{NAMA ANDA} sent at {Tanggal dan jam sekarang} | Title : {Title post dari struct Post yang diterima dari queue}

subscribe juga ke topic yang sama dengan topic publish agar function callback bisa jalan dan menerima message
*/ 
void mqttTask(void *pvParameters) {
    Post post;

    // Receive data from parameter
    post = *((Post *)pvParameters);

    Serial.print("Received: ");
    Serial.println(post.title);
    Serial.println(post.body);

    String message = "Edgrant Henderson Suryajaya - 2206025016 sent at ";
    // message += currentTime;
    message += "20:20";
    message += " | Title : ";
    message += post.title;

    // publish message
    if (!mqttClient.connected()) {
        reconnectMQTT();
    }

    mqttClient.publish(mqttTopic, message.c_str());
    Serial.println("Message sent to MQTT broker");
    Serial.print("Published topic: ");
    Serial.println(mqttTopic);
    Serial.print("Published message: ");
    Serial.println(message);

    // Subscribe to topic
    mqttClient.subscribe(mqttTopic);
}

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

// Function untuk callback 
void callback(char* topic, byte* payload, unsigned int length) {
    Serial.println("Message Received: ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}