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

SoftwareSerial RakSerial(PA1, PA0); // RX, TX

// Define Default LoRaWan ABP credential

String devAddr = "00000000";
String nwkkey = "00000000000000000000000000000000";
String appskey = "00000000000000000000000000000000";

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
  EchoSerial.println("AT+NWM=1"); // Set LORAWAN
  Serial.println("AT+NWM=1 // Set LORAWAN"); // Set P2P_LORA

  delay(1000);
  
  EchoSerial.println("AT+NJM=0"); // Set ABP
  Serial.println("AT+NJM=0 // Set ABP"); // Set ABP
  delay(200);
  
  EchoSerial.println("AT+BAND=4");// Set EU868 frequency band
  Serial.println("AT+BAND=4 // Set EU868 frequency band");// Set EU868 frequency band
  
  EchoSerial.println("AT+DR=5");// Set EU866 frequency band
  Serial.println("AT+DR=5 // Set EU868 frequency band");// Set EU868 frequency band  

  EchoSerial.printf("AT+DEVADDR=");
  EchoSerial.println(devAddr);
  Serial.printf("AT+DEVADDR=");
  Serial.println(devAddr);

  EchoSerial.printf("AT+NWKSKEY=");
  EchoSerial.println(nwkkey);
  Serial.printf("AT+NWKSKEY=");
  Serial.println(nwkkey);

  EchoSerial.printf("AT+APPSKEY=");
  EchoSerial.println(appskey);
  Serial.printf("AT+APPSKEY=");
  Serial.println(appskey);

}
