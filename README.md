# KOSEKI (Kontrol/Otomatis Suhu/Kelembaban Kar Intelligent)

**Proyek Sistem Embedded dan IoT**

## Pendahuluan

Proyek ini bertujuan untuk merancang sistem robot mobil berbasis IoT bernama KOSEKI yang dapat digunakan untuk eksplorasi di area sulit dijangkau manusia, seperti zona bencana atau gua. KOSEKI mengintegrasikan berbagai sensor dan teknologi untuk navigasi otomatis serta kontrol manual melalui aplikasi Blynk.

## Fitur Utama

1. **Navigasi Otomatis**: Menggunakan sensor ultrasonik dan inframerah untuk menghindari rintangan.
2. **Kontrol Manual**: Dikendalikan melalui aplikasi Blynk menggunakan WiFi.
3. **Pemantauan Lingkungan**: Mengukur suhu dan kelembaban menggunakan sensor DHT22 secara real-time.
4. **Integrasi IoT**: Data dikirim dan dapat diakses melalui aplikasi Blynk.

## Teknologi yang Digunakan

- **Hardware**:
  - ESP32 sebagai mikrokontroler.
  - Sensor DHT22, ultrasonik, dan inframerah.
  - Motor DC dengan driver motor L298N/L293D.
- **Software**:
  - Platform: Arduino IDE dengan PlatformIO.
  - Framework: FreeRTOS untuk multitasking.
  - Aplikasi Blynk untuk kontrol manual.

### Desain Perangkat Keras
- **ESP32**: Mengolah data sensor dan mengontrol motor.
- **Sensor DHT22**: Mengukur suhu dan kelembaban.
- **Sensor Ultrasonik & Inframerah**: Mendukung navigasi otomatis.
- **Motor DC**: Memberikan mobilitas pada robot.

### Pengembangan Perangkat Lunak
Kode dikembangkan menggunakan FreeRTOS untuk menjalankan beberapa tugas paralel, seperti:
- **Task untuk kontrol motor**: Mengatur gerakan robot.
- **Task untuk membaca data sensor**: Mengolah dan mengirimkan data ke aplikasi Blynk.
- **Task untuk navigasi otomatis**: Mengontrol pergerakan berdasarkan data sensor.

### Rangkaian

![image](https://github.com/user-attachments/assets/516286ab-29dd-4d3a-a77c-f657ad225040)
