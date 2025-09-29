// ---------- BLYNK (isi dengan data dari Blynk IoT) ----------
#define BLYNK_TEMPLATE_ID "TMPL6SioZ_zVj"
#define BLYNK_TEMPLATE_NAME "Smart Terrarium"
#define BLYNK_AUTH_TOKEN "ZADzqHpht5BlpH1VlvNAo0g49vdTM_rk"

// ---------- Libraries ----------
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

// ---------- WiFi ----------
char ssid[] = "Your Wifi name";
char pass[] = "Your Wifi Password";

// ---------- Pins ----------
#define DHTPIN     4
#define DHTTYPE    DHT11
#define SOIL_PIN   34
#define LDR_PIN    35
#define FAN_PIN    25
#define PUMP_PIN   26
#define LAMP_PIN   14

// ---------- Objects ----------
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
BlynkTimer timer;

// ---------- Virtual Pins ----------
const int VPIN_TEMP  = V0;
const int VPIN_HUM   = V1;
const int VPIN_SOIL  = V2;
const int VPIN_LIGHT = V3;
const int VPIN_FAN   = V4;
const int VPIN_PUMP  = V5;
const int VPIN_LAMP  = V6;
const int VPIN_MODE  = V7;

// Status virtual pins
const int VPIN_TEMP_STATUS  = V8;
const int VPIN_SOIL_STATUS  = V9;
const int VPIN_LIGHT_STATUS = V10;

// ---------- State variables ----------
bool autoMode = true;
bool manualFan = false;
bool manualPump = false;
bool manualLamp = false;
bool fanState = false;
bool pumpState = false;
bool lampState = false;
float tempAvg = NAN;
float humAvg  = NAN;
const float alpha = 0.2;

// ---------- Helper functions ----------
bool readDHTsafe(float &t, float &h) {
  for (int i = 0; i < 3; ++i) {
    t = dht.readTemperature();
    h = dht.readHumidity();
    if (!isnan(t) && !isnan(h) && t >= -10 && t <= 60 && h >= 0 && h <= 100) return true;
    delay(400);
  }
  return false;
}

int soilToPercent(int raw) {
  int p = map(raw, 4095, 0, 0, 100);
  return constrain(p, 0, 100);
}

int lightToLux(int raw) {
  int lux = map(raw, 4095, 0, 0, 1000);
  return constrain(lux, 0, 1000);
}

// ---------- Fuzzy Category ----------
String getTempCategory(float t) {
  if (t <= 26) return "Cold";
  else if (t < 28) return "Normal";
  else return "Hot";
}

String getSoilCategory(int s) {
  if (s <= 30) return "Dry";
  else if (s < 50) return "Normal";
  else return "Watery";
}

String getLightCategory(int lux) {
  if (lux <= 200) return "Dark";
  else if (lux < 400) return "Normal";
  else return "Light";
}

// ---------- Fuzzy ----------
void fuzzyControl(float temp, int soilPct, int lux) {
  String tempCat = getTempCategory(temp);
  String soilCat = getSoilCategory(soilPct);
  String lightCat = getLightCategory(lux);

  fanState = (tempCat == "Hot");

  pumpState = (soilCat == "Dry");

  lampState = (lightCat == "Dark");

  // Kirim status kategori ke Blynk
  Blynk.virtualWrite(VPIN_TEMP_STATUS, tempCat);
  Blynk.virtualWrite(VPIN_SOIL_STATUS, soilCat);
  Blynk.virtualWrite(VPIN_LIGHT_STATUS, lightCat);
}

// ---------- Blynk Handlers ----------
BLYNK_WRITE(VPIN_MODE)  { autoMode = (param.asInt() == 0); }
BLYNK_WRITE(VPIN_FAN)   { manualFan  = (param.asInt() != 0); }
BLYNK_WRITE(VPIN_PUMP)  { manualPump = (param.asInt() != 0); }
BLYNK_WRITE(VPIN_LAMP)  { manualLamp = (param.asInt() != 0); }

// ---------- Main Sensor Function ----------
int lcdPage = 0;

void sendSensorData() {
  float t, h;
  bool ok = readDHTsafe(t, h);

  if (ok) {
    if (isnan(tempAvg)) tempAvg = t; else tempAvg = (1 - alpha) * tempAvg + alpha * t;
    if (isnan(humAvg))  humAvg  = h; else humAvg  = (1 - alpha) * humAvg  + alpha * h;
  }

  int soilPct = soilToPercent(analogRead(SOIL_PIN));
  int lux     = lightToLux(analogRead(LDR_PIN));

  // Auto/Manual Control
  if (autoMode) {
    fuzzyControl(tempAvg, soilPct, lux);
  } else {
    fanState  = manualFan;
    pumpState = manualPump;
    lampState = manualLamp;
  }

  // Output ke pin (active LOW relay)
  digitalWrite(FAN_PIN,  fanState  ? LOW : HIGH);
  digitalWrite(PUMP_PIN, pumpState ? LOW : HIGH);
  digitalWrite(LAMP_PIN, lampState ? LOW : HIGH);

  // Kirim data sensor ke Blynk
  if (!isnan(tempAvg)) Blynk.virtualWrite(VPIN_TEMP, tempAvg);
  if (!isnan(humAvg))  Blynk.virtualWrite(VPIN_HUM, humAvg);
  Blynk.virtualWrite(VPIN_SOIL, soilPct);
  Blynk.virtualWrite(VPIN_LIGHT, lux);
  Blynk.virtualWrite(VPIN_FAN, fanState);
  Blynk.virtualWrite(VPIN_PUMP, pumpState);
  Blynk.virtualWrite(VPIN_LAMP, lampState);

  // LCD Display (bergantian)
  lcd.clear();
  if (lcdPage == 0) {
    lcd.setCursor(0,0);
    lcd.print("T:");
    lcd.print(tempAvg,1);
    lcd.print("C H:");
    lcd.print(humAvg,0);
    lcd.print("%");

    lcd.setCursor(0,1);
    lcd.print("L:");
    lcd.print(lux);
    lcd.print(" S:");
    lcd.print(soilPct);
    lcd.print("%");
  } else {
    lcd.setCursor(0,0);
    lcd.print("Fan:");
    lcd.print(fanState ? "On " : "Off");
    lcd.print(" Pump:");
    lcd.print(pumpState ? "On" : "Off");

    lcd.setCursor(0,1);
    lcd.print("Lamp:");
    lcd.print(lampState ? "On" : "Off");
  }

  lcdPage = (lcdPage + 1) % 2;
}

// ---------- Setup ----------
void setup() {
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  dht.begin();
  lcd.init();
  lcd.backlight();

  pinMode(FAN_PIN, OUTPUT);
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(LAMP_PIN, OUTPUT);

  digitalWrite(FAN_PIN, HIGH);
  digitalWrite(PUMP_PIN, HIGH);
  digitalWrite(LAMP_PIN, HIGH);

  timer.setInterval(2000L, sendSensorData);

  lcd.setCursor(0,0);
  lcd.print("Smart Terrarium");
  lcd.setCursor(0,1);
  lcd.print("Connecting WiFi");
}

// ---------- Loop ----------
void loop() {
  Blynk.run();
  timer.run();
}
