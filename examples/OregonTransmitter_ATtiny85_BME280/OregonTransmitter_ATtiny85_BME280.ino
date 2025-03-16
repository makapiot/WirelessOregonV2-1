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

  // Set BME to forced mode (take one sample and go to sleep)
  bme.setSampling(Tiny_BME280::MODE_FORCED);  
}
 
void loop() {

  // Read BME data, BME goes back to sleep after that
  bme.takeForcedMeasurement();
  float temp = bme.readTemperature();
  float humidity = bme.readHumidity();

  // Check battery status  
  static bool battOK;
  if(readVcc() > 2.1) {
    battOK = true;
  }  
  else {
    battOK = false;
  }  

  // Send the data over RF using the OS protocol
  digitalWrite(LED_PIN, HIGH);
  os.sendTempHumidity((int)(temp * 10.0), (byte)humidity, battOK);
  digitalWrite(LED_PIN, LOW);

  delay(5*1000);
  
}

