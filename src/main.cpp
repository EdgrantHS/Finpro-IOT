#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""
#define BLYNK_AUTH_TOKEN ""

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

void resetAllValues();
void remoteControlTask(void *pvParameters); // Kirim queue kykny
void automaticControlTask(void *pvParameters); // kirim queue kykny
void moveMotorTask(void *pvParameters); // pake queue sama mutex
void sendDHTDataTask(void *pvParameters);
void readSensorTask(void *pvParameters); // IR dan ultrasonik

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "";
char pass[] = "";

// Remote Control 1, di sini set global variabel aja
BLYNK_WRITE(V1)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable

}

// Remote Control 2, di sini set global variabel aja
BLYNK_WRITE(V2)
{
  int time = param.asInt(); // assigning incoming value from pin V2 to a variable

}

void setup()
{
  // Debug console
  Serial.begin(115200);

  // Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  resetAllValues();

  // FreeRTOS
}

void loop()
{
  Blynk.run();
}
