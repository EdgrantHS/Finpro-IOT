#include <WiFi.h>
#include <WebServer.h>

// Replace with your network credentials
const char* ssid = ":/";
const char* password = "AmamyaKokoro";

// Create a web server object on port 80
WebServer server(80);

// HTML and CSS content as a single raw string
const char* webpage = R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Web Server</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            text-align: center;
            background-color: #f4f4f9;
            color: #333;
        }
        h1 {
            color: #0066cc;
        }
        p {
            font-size: 1.2em;
        }
        .content {
            border: 1px solid #ccc;
            padding: 20px;
            margin-top: 20px;
            border-radius: 5px;
            background-color: #fff;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
        }
    </style>
</head>
<body>
    <h1>Welcome to the ESP32 Web Server!</h1>
    <div class="content">
        <p>This page is served by your ESP32 with embedded CSS and HTML.</p>
    </div>
</body>
</html>
)";

// Handles the root URL ("/") and serves the HTML page
void handleRoot() {
    server.send(200, "text/html", webpage);
}

void setup() {
    // Start serial communication
    Serial.begin(115200);

    // Connect to Wi-Fi network
    WiFi.begin(ssid, password);
    Serial.print("Connecting to Wi-Fi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println(" Connected!");

    // Print the ESP32 IP address
    Serial.println("ESP32 IP Address: ");
    Serial.println(WiFi.localIP());

    // Define the root URL
    server.on("/", handleRoot);

    // Start the server
    server.begin();
    Serial.println("Web server started.");
}

void loop() {
    // Handle client requests
    server.handleClient();
}
