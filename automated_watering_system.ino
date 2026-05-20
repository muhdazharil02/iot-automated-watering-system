#define BLYNK_TEMPLATE_ID "TMPL6Xze6M1cE"
#define BLYNK_TEMPLATE_NAME "Watering"
#define BLYNK_AUTH_TOKEN "5fIQpxo9y_NUASx8y_g-MHG77mPgsV6W"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <time.h>      

/*************** TIMER ***************/
BlynkTimer timer;

/*************** I2C MULTI-BUS ***************/
#include <Wire.h>
TwoWire I2C_BME = TwoWire(0);   
TwoWire I2C_OLED = TwoWire(1);  

/*************** BME680 ***************/
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>

#define BME680_SDA 27
#define BME680_SCL 26
Adafruit_BME680 bme(&I2C_BME);

/*************** OLED ***************/
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_SDA 21
#define OLED_SCL 22
Adafruit_SSD1306 display(128, 64, &I2C_OLED);

/*************** SENSORS & RELAY ***************/
#define LIGHT_PIN 34
#define SOIL_PIN 35
#define RELAY_PIN 2

/*************** WiFi Setup ***************/
const char* ssid = "Miskin";
const char* password = "NinsoMarket02";

bool BME_OK = false;
bool OLED_OK = false;
bool relayState = false; 

// NTP Time Server Settings
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 28800; // +8 hours (Malaysia)
const int   daylightOffset_sec = 0;

/*************** Calibration Values ***************/
const int soilMinADC = 1000;
const int soilMaxADC = 3000;
const int lightMinADC = 100;
const int lightMaxADC = 4000;

/*************** BLYNK RELAY CONTROL ***************/
BLYNK_WRITE(V0) {      
  int pinValue = param.asInt(); 
  relayState = pinValue;
  digitalWrite(RELAY_PIN, relayState ? HIGH : LOW);
}

/*************** MAIN FUNCTION ***************/
void sendSensorData() {
  // 1. Read Sensors
  int lightADC = analogRead(LIGHT_PIN);
  int soilADC  = analogRead(SOIL_PIN);

  int soilPercent = map(soilADC, soilMinADC, soilMaxADC, 100, 0); 
  soilPercent = constrain(soilPercent, 0, 100);

  int lightPercent = map(lightADC, lightMinADC, lightMaxADC, 100, 0);
  lightPercent = constrain(lightPercent, 0, 100);

  float temp = 0, hum = 0;
  if (BME_OK && bme.performReading()) {
      temp = bme.temperature;
      hum  = bme.humidity;
  }

  // 2. Get Time
  struct tm timeinfo;
  bool timeValid = getLocalTime(&timeinfo);
  int currentHour = timeinfo.tm_hour; 

  // 3. AUTOMATION LOGIC
  bool automationTrigger = false;

  // Rule A: Temp > 40 AND Time 11:00 - 17:59
  if (timeValid && temp > 40.0 && currentHour >= 11 && currentHour < 18) {
    automationTrigger = true;
  }

  // Rule B: Soil Moisture under 10%
  if (soilPercent < 10) {
    automationTrigger = true;
  }

  // --- SPECIAL BURST & RESTART LOGIC ---
  if (automationTrigger) {
    // A. Notify OLED 
    if (OLED_OK) {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextSize(2);
      display.println("WATERING");
      display.setTextSize(1);
      display.println("\nPump ON 3s...");
      display.display();
    }

    // B. Run Pump
    digitalWrite(RELAY_PIN, HIGH); // ON
    delay(3000);                   // Wait 3 Seconds (Must keep this for watering)
    digitalWrite(RELAY_PIN, LOW);  // OFF
    
    // C. Restart Immediately
    ESP.restart(); 
  }

  // 4. Normal Data Update
  Blynk.virtualWrite(V1, temp);          
  Blynk.virtualWrite(V2, hum);           
  Blynk.virtualWrite(V3, lightPercent);  
  Blynk.virtualWrite(V4, soilPercent);   
  Blynk.virtualWrite(V0, relayState); 

  if (OLED_OK && BME_OK) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);

    display.print("Temp: "); display.print((int)temp); display.println(" C");
    display.print("Hum : "); display.print((int)hum); display.println(" %");
    display.print("Soil: "); display.print(soilPercent); display.println(" %");
    display.print("Light: "); display.print(lightPercent); display.println(" %");
    display.print("Relay: "); display.println(relayState ? "ON" : "OFF");
    
    display.println(); 
    if (WiFi.status() == WL_CONNECTED) {
      display.print("WiFi: Connected");
    } else {
      display.print("WiFi: ..");
    }
    display.display();
  }
}

void setup() {
  // Use a faster baud rate for debugging
  Serial.begin(115200);

  // 1. Initialize Relay IMMEDIATELY to ensure it's OFF
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); 

  // 2. I2C & OLED Init (Visual feedback first)
  I2C_BME.begin(BME680_SDA, BME680_SCL);
  I2C_OLED.begin(OLED_SDA, OLED_SCL);

  if (display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    OLED_OK = true;
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Booting..."); // Shorter text
    display.display();
  }

  // 3. Sensor Init
  if (bme.begin(0x77)) {
    BME_OK = true;
    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setHumidityOversampling(BME680_OS_2X);
    bme.setGasHeater(320, 150);
  }

  // 4. Fast WiFi Connection
  WiFi.mode(WIFI_STA); // Station Mode is faster
  WiFi.begin(ssid, password);

  // We do NOT block here with a long loop anymore. 
  // We let the loop() handle the rest, or just a quick check.
  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 20) { // Wait max 2 seconds (20 * 100ms)
    delay(100);
    timeout++;
  }
  
  // 5. Config Time & Blynk
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Blynk.config(BLYNK_AUTH_TOKEN);

  // 6. Start Timer
  timer.setInterval(1500L, sendSensorData);
}

void loop() {
  Blynk.run();
  timer.run(); 
}