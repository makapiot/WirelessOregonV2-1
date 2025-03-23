#include <Adafruit_Sensor.h>   
#include <Tiny_BME280.h>       
#include <CPUVolt.h>
#include <WlessOregonV2.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

const byte TX_PIN       = 4;               // Tha transtitter data PIN number
const byte LED_PIN      = 3;                // LED pin number, LED is on during the transmission
const byte channel      = 0x20;             // Not sure it is good idea to change it
const byte sensorID     = 0xBB;             // The ID of the sensor (byte) can be changed if there are several sensors exist
const byte sendHumidity = true;             // Whether the sensor can send humidity

Tiny_BME280 bme; 
OregonSensor os(TX_PIN, channel, sensorID, sendHumidity);

volatile byte f_wdt = 1;

void setup() {
  os.init();
  pinMode(LED_PIN, OUTPUT);

  // Setup the WDT
  MCUSR &= ~(1<<WDRF);                                // Clear the reset flag
  
  // In order to change WDE or the prescaler, we need to
  // set WDCE (This will allow updates for 4 clock cycles).
  WDTCR |= (1<<WDCE) | (1<<WDE);
  WDTCR = 1<<WDP0 | 1<<WDP3;                         // set new watchdog timeout prescaler value 8.0 seconds
  WDTCR |= _BV(WDIE);                                // Enable the WD interrupt (note no reset).

  while(!bme.begin(0x76)) {
    delay(1000);
  }  

  // Set BME to forced mode (take one sample and go to sleep)
  bme.setSampling(Tiny_BME280::MODE_FORCED);  
}

void enterSleep(void) {
  ADCSRA &= ~(1<<ADEN);                               // disable ADC
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);                // the lowest power consumption
  sleep_enable();
  sleep_cpu();                                        // Now enter sleep mode.
  
  // The program will continue from here after the WDT timeout. 
  sleep_disable();                                    // First thing to do is disable sleep.
  power_all_enable();                                 // Re-enable the peripherals.
  ADCSRA |= 1<<ADEN;                                  // enable ADC
}

void loop() {

  if (f_wdt == 1) {
    f_wdt = 0;
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

    enterSleep();                                     // power-save mode for 8 seconds
  }
}

ISR(WDT_vect) {
  if (f_wdt == 0) f_wdt = 1;
}
  

