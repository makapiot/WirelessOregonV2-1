#include <WlessOregonV2.h>

/* Receive Temperature and humidity data using oregon protocol V2.1
 * To turn on dump of received packet, set last parameter of receiveData method to true. 
 * Ensure you have anabled serial port.
 */
OregonDecoderV2 orscV2;
#define LED_PIN 0 //  GPIO0 or D3

void setup () {
    Serial.begin(9600);                         // Enable Serial for packet DUMP output
    Serial.println("\n[ookDecoder]");
    pinMode(LED_PIN, OUTPUT);    
    orscV2.begin(digitalPinToInterrupt(4)); // GPIO4 or D2
}

void loop () {
  uint8_t ch   = 0;
  uint8_t id   = 0;
  int16_t temp  = 0;
  uint8_t humm = 0;
  bool battOK = false;
  if (orscV2.receiveData(ch, id, temp, humm, battOK)) {
    digitalWrite(LED_PIN, HIGH);
    Serial.print("Channel = "); Serial.print(ch, HEX);
    Serial.print(", ID = ");    Serial.print(id, HEX);
    char minus = ' ';
    if (temp < 0) {
      minus = '-';
      temp *= -1;
    }
    Serial.print(", Temp = ");  Serial.print(minus); Serial.print(temp/10); Serial.print("."); Serial.print(temp%10);
    Serial.print(", Humm = ");  Serial.print(humm);
    if (battOK) Serial.println(", battery OK");
    else        Serial.println(", replace battery");
    delay(100);
    digitalWrite(LED_PIN, LOW);   
  }
}
