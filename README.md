# Smart Terrarium IoT (ESP32 + Blynk)

Proyek ini adalah sistem **Smart Terrarium** berbasis **ESP32 + Blynk IoT**.  
Sistem mampu memonitor dan mengendalikan **suhu, kelembapan, kelembapan tanah, dan intensitas cahaya** secara otomatis maupun manual.

---

## Fitur
- Monitoring **Temperature, Humidity, Soil Moisture, Light Intensity**
- Kontrol otomatis/manual untuk:
  -  Fan
  -  Pump
  -  Lamp
- Tampilan data di **LCD 16x2 I2C**
- Integrasi dengan **Blynk IoT** (monitoring & remote control via smartphone)

---

## Hardware yang Dibutuhkan
- ESP32 Dev Board
- Sensor DHT11 (Temperature & Humidity)
- Sensor Soil Moisture
- LDR (Light Sensor)
- Relay Module 5V (3 channel)
- Fan 12V DC
- Water Pump Mini
- Grow Lamp LED
- LCD 16x2 I2C

---

## Wiring (ESP32 ke Sensor/Actuator)
| Komponen         | ESP32 Pin |
|------------------|-----------|
| DHT11 (Data)     | GPIO 4    |
| Soil Sensor (A0) | GPIO 34   |
| LDR (A0)         | GPIO 35   |
| Fan Relay        | GPIO 25   |
| Pump Relay       | GPIO 26   |
| Lamp Relay       | GPIO 14   |
| LCD I2C (SDA)    | GPIO 21   |
| LCD I2C (SCL)    | GPIO 22   |

 Jangan lupa sambungkan **VCC ke 3.3V/5V** sesuai modul sensor & **GND ke GND ESP32**.

---

## Instalasi Software
1. Install [Arduino IDE](https://www.arduino.cc/en/software)  
2. Tambahkan **ESP32 Board** ke Arduino IDE:
   - File > Preferences > Additional Boards URL:  
     ```
     https://dl.espressif.com/dl/package_esp32_index.json
     ```
   - Lalu buka **Boards Manager** dan install **esp32 by Espressif Systems**.
3. Install Library yang dibutuhkan:
   - `Blynk`  
   - `WiFi` (sudah ada default)  
   - `DHT sensor library`  
   - `LiquidCrystal_I2C`  

---

## Setup Blynk IoT
1. Buat project baru di **Blynk IoT**.  
2. Tambahkan widget:
   - Gauge / Value Display untuk Temp, Humidity, Soil, Light
   - Button untuk Fan, Pump, Lamp
   - Segmented Switch untuk Mode (Auto/Manual)
3. Salin **BLYNK_TEMPLATE_ID**, **BLYNK_TEMPLATE_NAME**, dan **BLYNK_AUTH_TOKEN** ke dalam kode:
   ```cpp
   #define BLYNK_TEMPLATE_ID   "YOUR_TEMPLATE_ID"
   #define BLYNK_TEMPLATE_NAME "Smart Terrarium"
   #define BLYNK_AUTH_TOKEN    "YOUR_AUTH_TOKEN"
