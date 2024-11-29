/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "Censored"
#define BLYNK_TEMPLATE_NAME "TP9"
#define BLYNK_AUTH_TOKEN "EPHWYt1Zb0tTZMbxJdaRrvTkNRtEZ3JC"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#define LED_PIN 4

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Kamal’s iPhone";
char pass[] = "katobestgirl";

static int speedGlobal = 0;
static int timeGlobal = 0;

void handleLEDwithTimer(int value, bool onOff){
  if (onOff) {
    Serial.print("LED ON | ");
    Serial.println(value);
    analogWrite(LED_PIN, value * 255 / 100);
  }
  else {
    Serial.println("LED OFF");
    analogWrite(LED_PIN, 0);
  }
}

void resetAllValues() {
  Blynk.virtualWrite(V0, 0);
  Blynk.virtualWrite(V1, 0);
  Blynk.virtualWrite(V2, 0);
}

//Log V0 to serial
BLYNK_WRITE(V0)
{
  Serial.println(param.asStr());
}

// This function will be called every time Slider Widget
// The slider uses V1 to send percantage for the LED brightness
BLYNK_WRITE(V1)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable

  // process received value
  Serial.print("Slider: ");
  Serial.println(pinValue);

  speedGlobal = pinValue;
}

// This function will be called every time the timer is changed
BLYNK_WRITE(V2)
{
  int time = param.asInt(); // assigning incoming value from pin V2 to a variable

  Serial.print("Timer: ");
  Serial.println(time);

  timeGlobal = time;
}

// Compare time and brightness function and LED on
void compareTimeAndBrightness() {
  if (timeGlobal > 0) {
    handleLEDwithTimer(speedGlobal, true);
    timeGlobal--;
    Blynk.virtualWrite(V2, timeGlobal);
  } else {
    handleLEDwithTimer(speedGlobal, false);
  }
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void setup()
{
  // Debug console
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  resetAllValues();
}

void loop()
{
  Blynk.run();
  compareTimeAndBrightness();
}
