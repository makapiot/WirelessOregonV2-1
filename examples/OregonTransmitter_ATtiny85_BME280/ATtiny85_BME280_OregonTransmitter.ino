#include <Adafruit_Sensor.h>   
#include <Tiny_BME280.h>       
#include <CPUVolt.h>
#include <WlessOregonV2.h>

const byte TX_PIN       = 4;               // Tha transtitter data PIN number
const byte LED_PIN      = 3;                // LED pin number, LED is on during the transmission
const byte channel      = 0x20;             // Not sure it is good idea to change it
const byte sensorID     = 0xBB;             // The ID of the sensor (byte) can be changed if there are several sensors exist
const byte sendHumidity = true;             // Whether the sensor can send humidity

//#define THN132N
Tiny_BME280 bme; 
OregonSensor os(TX_PIN, channel, sensorID, sendHumidity);
 
void setup() {
  os.init();
  pinMode(LED_PIN, OUTPUT);

  while(!bme.begin(0x76)) {
    delay(1000);
  }  
}
 
void loop() {
  digitalWrite(LED_PIN, HIGH);
  float temp = bme.readTemperature();
  float humidity = bme.readHumidity();
  static bool battOK = true;  

  // Serial.print("Sending data of sensor "); Serial.print(sensorID, HEX);
  // Serial.print(": temp = "); Serial.print(temp);
  // Serial.print(", humidity = "); Serial.println(humidity);
  os.sendTempHumidity((int)(temp * 10.0), (byte)humidity, battOK);
  digitalWrite(LED_PIN, LOW);

  delay(5000);
  
}

