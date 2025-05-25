/**
 * @brief This example will forward serial / UART signal from the USB to
 *        the GNSS module and vice versa. In another word, it will by pass the MCU.
 * 
 * @author mtnguyen, fferrero
 * @version 1.1.0
 * 
 */

 #include <SoftwareSerial.h>

SoftwareSerial GNSSSerial(PC1, PC0); // RX, TX

void setup(void)
{
  pinMode(PE0, OUTPUT);
  digitalWrite(PE0, HIGH);

  pinMode(PA2, OUTPUT);
  digitalWrite(PA2, HIGH);
  //pinMode(GNSS_V_BCKP_PIN, OUTPUT);
  //digitalWrite(GNSS_V_BCKP_PIN, HIGH);
  
  Serial.begin(115200);
  while (!Serial)
    ;

  Serial.println("Starting...");

  GNSSSerial.begin(9600);
  delay(1000);
  digitalWrite(PE0, LOW);
}

void loop(void)
{
  if (GNSSSerial.available())
  {
    Serial.write(GNSSSerial.read());
  }

  if (Serial.available())
  {
    char c = Serial.read();
    GNSSSerial.write(c);
  }
}
