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

### Integration
Semua komponen terhubung ke ESP32 untuk memastikan pengumpulan data sensor dan kontrol motor berjalan lancar. Robot ini dirancang agar mampu bekerja di medan berbahaya dengan daya baterai 12V.

---

## Software Implementation Details

### Platform
- **Blynk**: Untuk kontrol manual dan pemantauan data sensor.  
- **FreeRTOS**: Mengelola multitasking, seperti membaca sensor, mengontrol motor, dan komunikasi data.

### Features
- **Kontrol Manual**: Menggunakan aplikasi Blynk untuk menggerakkan robot (maju, mundur, kiri, kanan, berhenti).
- **Navigasi Otomatis**: Berdasarkan data sensor untuk menghindari rintangan.
- **Pengumpulan Data**: Suhu dan kelembaban lingkungan dikirim ke Blynk secara real-time.

### Code Highlights
- **Task-based System**: Menggunakan FreeRTOS untuk menjalankan tugas seperti:
  - Penggerakan motor
  - Pengiriman data DHT ke Blynk
  - Pembacaan sensor ultrasonik dan inframerah

---

## Test Results and Performance Evaluation

### Test Setup
- Program diuji menggunakan PlatformIO di Visual Studio Code.  
- Robot dikontrol secara manual melalui aplikasi Blynk untuk menguji semua fitur.

### Results
- **Navigasi Otomatis**: Robot berhasil menghindari rintangan dan menyesuaikan arah.
- **Pengumpulan Data**: Data suhu dan kelembaban terbaca dengan akurat, namun memerlukan aktivasi melalui aplikasi Blynk.
- **Kendala**: Daya baterai 9V tidak mencukupi untuk mendukung operasi motor. Baterai 12V diperlukan untuk performa optimal.

---

## Conclusion and Future Work

### Conclusion
Proyek KOSEKI berhasil memenuhi kriteria yang ditentukan:
- Navigasi otomatis dan kontrol manual bekerja dengan baik.
- Data sensor dikirim ke Blynk secara real-time.
- Robot berfungsi sesuai target untuk eksplorasi di area berbahaya.

### Future Work
- **Optimisasi daya**: Menggunakan sumber daya yang lebih efisien untuk meningkatkan durasi operasi.  
- **Peningkatan fitur**: Menambahkan kamera untuk visualisasi lingkungan.  
- **Integrasi AI**: Memungkinkan pengambilan keputusan yang lebih cerdas dalam navigasi otomatis.

---

> **Dibuat oleh Grup 9 - Universitas Indonesia**  
> Adam Bintang Arafah Poernomo, Edgrant Henderson Suryajaya, Fairuz Muhammad, Reiki Putra Darmawan
