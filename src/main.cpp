/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL69TiUly6S"
#define BLYNK_TEMPLATE_NAME "Final Project IOT Group 5"
#define BLYNK_AUTH_TOKEN "jVextDFvO-MWXlLCoVDrZ38SlrGaCeI1"

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <DHT.h>
#include <Ultrasonic.h>
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

// IR Sensor pins
#define IR_SENSOR1 34
#define IR_SENSOR2 35

// Ultrasonic Sensor pins
#define ECHO_PIN 22
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
typedef struct
{
  float temperature;
  float humidity;
} DHTData;

// Ultrasonic Sensor
Ultrasonic ultrasonic(TRIG_PIN, ECHO_PIN);

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

// Forward declarations
void remoteControlTask(void *pvParameters);
void automaticControlTask(void *pvParameters);
void moveMotorTask(void *pvParameters);
void sendDHTDataTask(void *pvParameters);
void readSensorTask(void *pvParameters);

// Your WiFi credentials.
char ssid[] = "Fz";
char pass[] = "2206814324";

// Global variables
static int remoteControl = 0;
static int manualControl = 0;
static int distance = 0;
static int irSensor1 = 0;
static int irSensor2 = 0;
static int readDHTbutton = 0;

// Queue
static QueueHandle_t xQueue;

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

// Manual DHT VP
BLYNK_WRITE(V4)
{
  readDHTbutton = param.asInt();
  xQueueSend(xQueue, &readDHTbutton, portMAX_DELAY);
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  Serial.println("Serial started");
  dht.begin();
  Serial.println("DHT started");

  // Motor
  pinMode(MOTOR1_PIN1, OUTPUT);
  pinMode(MOTOR1_PIN2, OUTPUT);
  pinMode(MOTOR2_PIN1, OUTPUT);
  pinMode(MOTOR2_PIN2, OUTPUT);
  pinMode(IR_SENSOR1, INPUT_PULLUP);
  pinMode(IR_SENSOR2, INPUT_PULLUP);
  pinMode(DHT_PIN, INPUT);

  Serial.println("Motor setup");

  Serial.println("PWM setup");

  // Blynk
  // Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  Serial.println("Blynk started");

  // Queue
  xQueue = xQueueCreate(8, sizeof(int));

  // // Task
  // xTaskCreatePinnedToCore(
  //     moveMotorTask, /* Task function. */
  //     "Move Motor",  /* name of task. */
  //     10000,         /* Stack size of task */
  //     NULL,          /* parameter of the task */
  //     24,            /* Very High priority of the task */
  //     NULL,          /* Task handle */
  //     0);            /* core 0 */

  Serial.println("Task created");

  // xTaskCreatePinnedToCore(
  //     sendDHTDataTask, /* Task function. */
  //     "Send DHT Data", /* name of task. */
  //     4096,            /* Stack size of task */
  //     NULL,            /* parameter of the task */
  //     24,              /* Very High priority of the task */
  //     NULL,            /* Task handle */
  //     0);              /* core 0 */

  Serial.println("Task created DHT");

  // xTaskCreatePinnedToCore(
  //     remoteControlTask, /* Task function. */
  //     "Remote Control",  /* name of task. */
  //     10000,             /* Stack size of task */
  //     NULL,              /* parameter of the task */
  //     24,                /* Very High priority of the task */
  //     NULL,              /* Task handle */
  //     0);                /* core 0 */

  // xTaskCreatePinnedToCore(
  //     readSensorTask, /* Task function. */
  //     "Read Sensor",  /* name of task. */
  //     1024,           /* Stack size of task */
  //     NULL,           /* parameter of the task */
  //     24,             /* Very High priority of the task */
  //     NULL,           /* Task handle */
  //     0);             /* core 0 */

  Serial.println("Task created Sensor");

  // xTaskCreatePinnedToCore(
  //     automaticControlTask, /* Task function. */
  //     "Automatic Control",  /* name of task. */
  //     10240,               /* Stack size of task */
  //     NULL,                /* parameter of the task */
  //     24,                  /* Very High priority of the task */
  //     NULL,                /* Task handle */
  //     0);                  /* core 0 */
}

void loop()
{
  // Blynk.run();
  // maju
  digitalWrite(MOTOR1_PIN1, HIGH);
  digitalWrite(MOTOR1_PIN2, LOW);
  digitalWrite(MOTOR2_PIN1, HIGH);
  digitalWrite(MOTOR2_PIN2, LOW);
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  Serial.println("\nMaju\n");

  // berhenti
  digitalWrite(MOTOR1_PIN1, LOW);
  digitalWrite(MOTOR1_PIN2, LOW);
  digitalWrite(MOTOR2_PIN1, LOW);
  digitalWrite(MOTOR2_PIN2, LOW);
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  Serial.println("\nBerhenti\n");

  // mundur
  digitalWrite(MOTOR1_PIN1, LOW);
  digitalWrite(MOTOR1_PIN2, HIGH);
  digitalWrite(MOTOR2_PIN1, LOW);
  digitalWrite(MOTOR2_PIN2, HIGH);
  delay(2000);
  Serial.println("\nMundur\n");
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
    }
    else if (remoteControl == 2 && manualControl == 1)
    {
      // Mundur
      digitalWrite(MOTOR1_PIN1, LOW);
      digitalWrite(MOTOR1_PIN2, HIGH);
      digitalWrite(MOTOR2_PIN1, LOW);
      digitalWrite(MOTOR2_PIN2, HIGH);
    }
    else if (remoteControl == 3 && manualControl == 1)
    {
      // Kanan
      digitalWrite(MOTOR1_PIN1, HIGH);
      digitalWrite(MOTOR1_PIN2, LOW);
      digitalWrite(MOTOR2_PIN1, LOW);
      digitalWrite(MOTOR2_PIN2, HIGH);
    }
    else if (remoteControl == 4 && manualControl == 1)
    {
      // Kiri
      digitalWrite(MOTOR1_PIN1, LOW);
      digitalWrite(MOTOR1_PIN2, HIGH);
      digitalWrite(MOTOR2_PIN1, HIGH);
      digitalWrite(MOTOR2_PIN2, LOW);
    }
    else if (remoteControl == 5 && manualControl == 1)
    {
      // Berhenti
      digitalWrite(MOTOR1_PIN1, LOW);
      digitalWrite(MOTOR1_PIN2, LOW);
      digitalWrite(MOTOR2_PIN1, LOW);
      digitalWrite(MOTOR2_PIN2, LOW);
    }

    // delay untuk menghindari motor bergerak terlalu cepat
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}

void sendDHTDataTask(void *pvParameters)
{
  while (1)
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
      Serial.println("Temperature: " + String(temperature) + " C");
      Serial.println("Humidity: " + String(humidity) + " %");
      // Kirim data DHT ke Blynk
      Blynk.virtualWrite(V1, temperature);
      Blynk.virtualWrite(V2, humidity);
    }

    // delay 1 detik
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void remoteControlTask(void *pvParameters)
{
  while (1)
  {
    // kalau manual control tidak aktif, maka motor akan bergerak sesuai dengan inputan dari Blynk
    if (manualControl == 0)
    {
      xQueueSend(xQueue, &remoteControl, 0); // kalau gagal kirim ke queue, langsung hangus instruksinya
    }

    // delay 10 ms
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void readSensorTask(void *pvParameters)
{
  while (1)
  {
    Serial.println("Reading sensor...");
    Serial.println("Distance: " + String(distance) + " cm");
    Serial.println("IR Sensor 1: " + String(irSensor1));
    Serial.println("IR Sensor 2: " + String(irSensor2));

    // Baca sensor ultrasonik
    distance = ultrasonic.read(); // baca jarak dalam cm

    // Baca sensor IR
    irSensor1 = digitalRead(IR_SENSOR1);
    irSensor2 = digitalRead(IR_SENSOR2);

    // delay 100 ms
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void automaticControlTask(void *pvParameters)
{
  while (1)
  {
    // kalau manual control tidak aktif, maka robot akan bergerak otomatis
    if (manualControl == 0)
    {
      // Jika sensor ultrasonik mendeteksi objek di depan
      if (distance < 10)
      {
        // Berhenti
        remoteControl = 5;
        xQueueSend(xQueue, &remoteControl, 0);
      }
      else
      {
        // Jika sensor IR1 mendeteksi objek
        if (irSensor1 == 0)
        {
          // Mundur
          remoteControl = 2;
          xQueueSend(xQueue, &remoteControl, 0);
        }
        // Jika sensor IR2 mendeteksi objek
        else if (irSensor2 == 0)
        {
          // Mundur
          remoteControl = 2;
          xQueueSend(xQueue, &remoteControl, 0);
        }
        else
        {
          // Maju
          remoteControl = 1;
          xQueueSend(xQueue, &remoteControl, 0);
        }
      }
    }

    // delay 10 ms
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
