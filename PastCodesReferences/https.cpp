#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

const char* ssid = ":/";
const char* password =  "AmamyaKokoro";
const char* host = "jsonplaceholder.typicode.com";
const int httpsPort = 443;

WiFiClientSecure client; // Menggunakan WiFiClientSecure untuk koneksi HTTPS

// SSL certificate dari jsonplaceholder.typicode.com
const char* test_root_ca = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDejCCAmKgAwIBAgIQf+UwvzMTQ77dghYQST2KGzANBgkqhkiG9w0BAQsFADBX" \
"MQswCQYDVQQGEwJCRTEZMBcGA1UEChMQR2xvYmFsU2lnbiBudi1zYTEQMA4GA1UE" \
"CxMHUm9vdCBDQTEbMBkGA1UEAxMSR2xvYmFsU2lnbiBSb290IENBMB4XDTIzMTEx" \
"NTAzNDMyMVoXDTI4MDEyODAwMDA0MlowRzELMAkGA1UEBhMCVVMxIjAgBgNVBAoT" \
"GUdvb2dsZSBUcnVzdCBTZXJ2aWNlcyBMTEMxFDASBgNVBAMTC0dUUyBSb290IFI0" \
"MHYwEAYHKoZIzj0CAQYFK4EEACIDYgAE83Rzp2iLYK5DuDXFgTB7S0md+8Fhzube" \
"Rr1r1WEYNa5A3XP3iZEwWus87oV8okB2O6nGuEfYKueSkWpz6bFyOZ8pn6KY019e" \
"WIZlD6GEZQbR3IvJx3PIjGov5cSr0R2Ko4H/MIH8MA4GA1UdDwEB/wQEAwIBhjAd" \
"BgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIwDwYDVR0TAQH/BAUwAwEB/zAd" \
"BgNVHQ4EFgQUgEzW63T/STaj1dj8tT7FavCUHYwwHwYDVR0jBBgwFoAUYHtmGkUN" \
"l8qJUC99BM00qP/8/UswNgYIKwYBBQUHAQEEKjAoMCYGCCsGAQUFBzAChhpodHRw" \
"Oi8vaS5wa2kuZ29vZy9nc3IxLmNydDAtBgNVHR8EJjAkMCKgIKAehhxodHRwOi8v" \
"Yy5wa2kuZ29vZy9yL2dzcjEuY3JsMBMGA1UdIAQMMAowCAYGZ4EMAQIBMA0GCSqG" \
"SIb3DQEBCwUAA4IBAQAYQrsPBtYDh5bjP2OBDwmkoWhIDDkic574y04tfzHpn+cJ" \
"odI2D4SseesQ6bDrarZ7C30ddLibZatoKiws3UL9xnELz4ct92vID24FfVbiI1hY" \
"+SW6FoVHkNeWIP0GCbaM4C6uVdF5dTUsMVs/ZbzNnIdCp5Gxmx5ejvEau8otR/Cs" \
"kGN+hr/W5GvT1tMBjgWKZ1i4//emhA1JG1BbPzoLJQvyEotc03lXjTaCzv8mEbep" \
"8RqZ7a2CPsgRbuvTPBwcOMBBmuFeU88+FSBX6+7iP0il8b4Z0QFqIwwMHfs/L6K1" \
"vepuoxtGzi4CZ68zJpiq1UvSqTbFJjtbD4seiMHl" \
"-----END CERTIFICATE-----\n";

void setup() {
    Serial.begin(115200);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

    client.setCACert(test_root_ca); // Mengatur SSL certificate

    if (!client.connect(host, httpsPort)) {
        Serial.println("Connection failed");
        return;
    }

    Serial.println("Connected to server");
    Serial.print("Requesting URL: ");
    Serial.println(host);

    // Send HTTP GET request
    client.println("GET /todos/1 HTTP/1.1");
    client.println("Host: jsonplaceholder.typicode.com");
    client.println("Connection: close");
    client.println();

    // Read and skip headers
    while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") {
            Serial.println("Headers received");
            break;
        }
    }

    // Read JSON body and parse it
    if (client.available()) {
        JsonDocument doc; // Adjust size if needed
        DeserializationError error = deserializeJson(doc, client);

        if (error) {
            Serial.print("deserializeJson() failed: ");
            Serial.println(error.c_str());
            return;
        }

        // Extract data from JSON
        int userId = doc["userId"];
        int id = doc["id"];
        const char* title = doc["title"];
        bool completed = doc["completed"];

        Serial.println("Response:");
        Serial.print("userId: ");
        Serial.println(userId);
        Serial.print("id: ");
        Serial.println(id);
        Serial.print("title: ");
        Serial.println(title);
        Serial.print("completed: ");
        Serial.println(completed ? "true" : "false");

        Serial.println("Closing connection");
    }

    client.stop();
}


void loop() {
    // empty
}