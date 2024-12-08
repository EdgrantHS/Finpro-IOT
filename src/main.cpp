#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <DHT.h>
#include <BlynkSimpleEsp32.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

// Motor pins
#define MOTOR1_PIN1 27
#define MOTOR1_PIN2 26
#define MOTOR2_PIN1 25
#define MOTOR2_PIN2 33
#define MOTOR1_EN 14
#define MOTOR2_EN 32

// IR Sensor pins
#define IR_SENSOR1 34
#define IR_SENSOR2 35

// Ultrasonic Sensor pins
#define TRIG_PIN 23

// PWM Properties
#define PWM_FREQ 30000
#define PWM1_CHANNEL 0
#define PWM2_CHANNEL 1
#define PWM_RESOLUTION 8

// DHT Properties
#define DHT_PIN 4
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""
#define BLYNK_AUTH_TOKEN ""

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

void resetAllValues();
void remoteControlTask(void *pvParameters);    // Kirim queue kykny
void automaticControlTask(void *pvParameters); // kirim queue kykny
void moveMotorTask(void *pvParameters);        // pake queue sama mutex
void sendDHTDataTask(void *pvParameters);
void readSensorTask(void *pvParameters); // IR dan ultrasonik

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "";
char pass[] = "";

// Global variables
int remoteControl = 0;
int manualControl = 0;
int distance = 0;
int irSensor1 = 0;
int irSensor2 = 0;

// Queue
QueueHandle_t xQueue;

// Manual Control VP
BLYNK_WRITE(V0)
{
  manualControl = param.asInt();
  xQueueSend(xQueue, &manualControl, portMAX_DELAY);
}

// Temperature VP
BLYNK_WRITE(V1)
{
}

// Humidity VP
BLYNK_WRITE(V2)
{
}

// Direction VP
BLYNK_WRITE(V3)
{
  remoteControl = param.asInt();
  xQueueSend(xQueue, &remoteControl, portMAX_DELAY);
}

// // Manual DHT VP
BLYNK_WRITE(V3)
{
  readDHTbutton = param.asInt();
  xQueueSend(xQueue, &readDHTbutton, portMAX_DELAY);
}  

void setup()
{
  // Debug console
  Serial.begin(115200);
  dht.begin();

  // Motor
  pinMode(MOTOR1_PIN1, OUTPUT);
  pinMode(MOTOR1_PIN2, OUTPUT);
  pinMode(MOTOR2_PIN1, OUTPUT);
  pinMode(MOTOR2_PIN2, OUTPUT);
  pinMode(MOTOR1_EN, OUTPUT);
  pinMode(MOTOR2_EN, OUTPUT);
  pinMode(IR_SENSOR1, INPUT_PULLUP);
  pinMode(IR_SENSOR2, INPUT_PULLUP);
  pinMode(DHT_PIN, INPUT);

  // DHT
  dhtQueue = xQueueCreate(5, sizeof(DHTData));

  // PWM
  ledcSetup(PWM1_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(MOTOR1_EN, PWM1_CHANNEL);
  ledcSetup(PWM2_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(MOTOR2_EN, PWM2_CHANNEL);

  // Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  resetAllValues();

  // FreeRTOS
  // Setup Queue dengan ukuran 8 sebagai buffer
  xQueue = xQueueCreate(8, sizeof(int));

  // Task
  xTaskCreatePinnedToCore(
      moveMotorTask, /* Task function. */
      "Move Motor",  /* name of task. */
      10000,         /* Stack size of task */
      NULL,          /* parameter of the task */
      24,            /* Very High priority of the task */
      NULL,          /* Task handle to keep track of created task */
      0);            /* core 0, nanti coba multicore pake xTaskCreate */

  xTaskCreatePinnedToCore(
      sendDHTDataTask, /* Task function. */
      "Send DHT Data", /* name of task. */
      10000,           /* Stack size of task */
      NULL,            /* parameter of the task */
      24,              /* Very High priority of the task */
      NULL,            /* Task handle to keep track of created task */
      0);              /* core 0, nanti coba multicore pake xTaskCreate */
}

void loop()
{
  Blynk.run();
}

void moveMotorTask(void *pvParameters)
{
  while (1)
  {
    // saat mendapat queue, motor akan bergerak sesuai dengan queue yang diterima
    xQueueReceive(xQueue, &remoteControl, portMAX_DELAY);
    xQueueReceive(xQueue, &manualControl, portMAX_DELAY);

    if (remoteControl == 1 && manualControl == 1)
    {
      // Maju
      digitalWrite(MOTOR1_PIN1, HIGH);
      digitalWrite(MOTOR1_PIN2, LOW);
      digitalWrite(MOTOR2_PIN1, HIGH);
      digitalWrite(MOTOR2_PIN2, LOW);
      ledcWrite(PWM1_CHANNEL, 255);
      ledcWrite(PWM2_CHANNEL, 255);
    }
    else if (remoteControl == 2 && manualControl == 1)
    {
      // Mundur
      digitalWrite(MOTOR1_PIN1, LOW);
      digitalWrite(MOTOR1_PIN2, HIGH);
      digitalWrite(MOTOR2_PIN1, LOW);
      digitalWrite(MOTOR2_PIN2, HIGH);
      ledcWrite(PWM1_CHANNEL, 255);
      ledcWrite(PWM2_CHANNEL, 255);
    }
    else if (remoteControl == 3 && manualControl == 1)
    {
      // Kanan
      digitalWrite(MOTOR1_PIN1, HIGH);
      digitalWrite(MOTOR1_PIN2, LOW);
      digitalWrite(MOTOR2_PIN1, LOW);
      digitalWrite(MOTOR2_PIN2, HIGH);
      ledcWrite(PWM1_CHANNEL, 255);
      ledcWrite(PWM2_CHANNEL, 255);
    }
    else if (remoteControl == 4 && manualControl == 1)
    {
      // Kiri
      digitalWrite(MOTOR1_PIN1, LOW);
      digitalWrite(MOTOR1_PIN2, HIGH);
      digitalWrite(MOTOR2_PIN1, HIGH);
      digitalWrite(MOTOR2_PIN2, LOW);
      ledcWrite(PWM1_CHANNEL, 255);
      ledcWrite(PWM2_CHANNEL, 255);
    }
    else if (remoteControl == 5 && manualControl == 1)
    {
      // Berhenti
      digitalWrite(MOTOR1_PIN1, LOW);
      digitalWrite(MOTOR1_PIN2, LOW);
      digitalWrite(MOTOR2_PIN1, LOW);
      digitalWrite(MOTOR2_PIN2, LOW);
      ledcWrite(PWM1_CHANNEL, 0);
      ledcWrite(PWM2_CHANNEL, 0);
    }

    // delay untuk menghindari motor bergerak terlalu cepat
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}

void sendDHTDataTask(void *pvParameters)
{
  while (1)
  {
    xQueueReceive(xQueue, &manualControl, portMAX_DELAY);
    xQueueReceive(xQueue, &readDHTbutton, portMAX_DELAY);

    // Baca DHT otomatis jika manual control tidak aktif
    if (manualControl == 0)
    {
      float temperature = dht.readTemperature();
      float humidity = dht.readHumidity();

      // Cek pembacaan
      if (isnan(temperature) || isnan(humidity))
      {
        Serial.println("Failed to read from DHT sensor!");
      }
      else
      {
        // Kirim data DHT ke Blynk
        Blynk.virtualWrite(V1, temperature);
        Blynk.virtualWrite(V2, humidity);
      }

      // delay 1 detik
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    // Baca DHT jika manual control aktif
    else if (manualControl == 1 && readDHTbutton == 1)
    {
      // Baca data dari sensor DHT
      float temperature = dht.readTemperature();
      float humidity = dht.readHumidity();

      // Cek pembacaan
      if (isnan(temperature) || isnan(humidity))
      {
        Serial.println("Failed to read from DHT sensor!");
      }
      else
      {
        // Kirim data DHT ke Blynk
        Blynk.virtualWrite(V1, temperature);
        Blynk.virtualWrite(V2, humidity);
      }
    }
  }
}