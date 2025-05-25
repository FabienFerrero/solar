/**
 * @brief This example will forward serial signal from the USB to Rak3172 module and vice versa.
 *        In another word, it will by pass the MCU.
 *
 *        The DPDT RF Switch control signal can be reversed by sending '*' character to the USB Serial.
 *
 * @author mtnguyen, fferrero
 * @version 1.1.0
 */

#include <SoftwareSerial.h>

#include "lorawan_credential.h"

SoftwareSerial RakSerial(PA1, PA0); // RX, TX

// Define Default LoRaWan ABP credential

String devAddr = "260B8C12";
String nwkkey = "899D26FF817B5DDACD78F3008E6E0D2B";
String appskey = "4CC41F9FACF51385F349604A189100B7";

String appEUI = "0000000000000000";
String devEUI = "0000000000000000";
String appkey = "00000000000000000000000000000000";


void setup(void)
{
  pinMode(PE0, OUTPUT);
  digitalWrite(PE0, HIGH);


  pinMode(PA6, OUTPUT); // Switch ON RAK LDO
  digitalWrite(PA6, HIGH);
delay(3000);

  Serial.begin(115200);
  while (!Serial)
    ;

  Serial.println("Starting...");

  digitalWrite(PE0, LOW);

  RakSerial.begin(115200);

  Serial.println("Rak Connected...");

  delay(5000);

  flush_serial_AT(true); 

  SetLoRaABP();

  
}

void loop(void)
{
  if (RakSerial.available())
  {
    char d = RakSerial.read();
    Serial.write(d);
  }

  if (Serial.available())
  {
    char c = Serial.read();
    RakSerial.write(c);

  }
}



void SetLoRaABP(){
  RakSerial.println("AT+NWM=1"); // Set LORAWAN
  Serial.println("AT+NWM=1 // Set LORAWAN"); // Set P2P_LORA

flush_serial_AT(true); 
  delay(1000);
  
  RakSerial.println("AT+NJM=0"); // Set ABP
  Serial.println("AT+NJM=0 // Set ABP"); // Set ABP
 flush_serial_AT(true); 
  delay(1000);
  
  RakSerial.println("AT+BAND=4");// Set EU868 frequency band
  Serial.println("AT+BAND=4 // Set EU868 frequency band");// Set EU868 frequency band
flush_serial_AT(true); 
  delay(1000);
  
  RakSerial.println("AT+DR=5");// Set EU866 frequency band
  Serial.println("AT+DR=5 // Set EU868 frequency band");// Set EU868 frequency band  
flush_serial_AT(true); 
  delay(1000);

  RakSerial.printf("AT+DEVADDR=");
  RakSerial.println(devAddr);
  Serial.printf("AT+DEVADDR=");
  Serial.println(devAddr);
flush_serial_AT(true); 
  delay(1000);

  RakSerial.printf("AT+NWKSKEY=");
  RakSerial.println(nwkkey);
  Serial.printf("AT+NWKSKEY=");
  Serial.println(nwkkey);
flush_serial_AT(true); 
  delay(1000);

  RakSerial.printf("AT+APPSKEY=");
  RakSerial.println(appskey);
  Serial.printf("AT+APPSKEY=");
  Serial.println(appskey);

  flush_serial_AT(true); 

}


void flush_serial_AT(bool print){

  //while(mySerial1.available() == 0) {}

   if (RakSerial.available())
  { // If anything comes in Serial1 (pins 4 & 5)
    while (RakSerial.available())
      if(print) {Serial.write(RakSerial.read()); // read it and send it out Serial (USB)
      }
      else {
      RakSerial.read();}
  }
  delay(100);
}
