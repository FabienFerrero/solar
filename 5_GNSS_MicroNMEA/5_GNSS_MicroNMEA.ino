/**
 * @brief This example demonstrates how to read data from the GNSS module.
 *
 *        The example reset the GNSS at the beginning by cut off the power to the
 *        GNSS module for 5 seconds. After the GNSS get fixed with 4 satellite (or more),
 *        the Serial will stop printing NMEA sentences and how time-to-first-fix & GNSS data.
 * 
 * @author mtnguyen, fferrero
 * @version 1.1.0
 */

#include <MicroNMEA.h> // https://github.com/stevemarple/MicroNMEA
#include <SoftwareSerial.h>

SoftwareSerial GNSSSerial(PC1, PC0); // RX, TX

char nmeaBuffer[100];
MicroNMEA nmea(nmeaBuffer, sizeof(nmeaBuffer));

bool gnss_fix_status = false;

uint32_t start_timestamp = 0;
uint32_t stop_timestamp = 0;

void setup(void)
{
  gnss_fix_status = false;

  pinMode(PE0, OUTPUT);
  digitalWrite(PE0, HIGH);

  pinMode(PA2, OUTPUT);
  

  Serial.begin(115200);
  while (!Serial)
    ;

  Serial.println("Starting...");

  GNSSSerial.begin(9600);

  delay(1000);
 digitalWrite(PA2, HIGH);
  start_timestamp = millis();

  digitalWrite(PE0, LOW);
}

void loop(void)
{
  if (GNSSSerial.available())
  {
    char c = GNSSSerial.read();

    if (gnss_fix_status == false)
      Serial.print(c);
    nmea.process(c);
  }

  if (Serial.available())
  {
    char c = Serial.read();
    GNSSSerial.write(c);
  }

  if ((nmea.isValid() == true) && (nmea.getNumSatellites() > 5))
  {
    if (!gnss_fix_status)
    {
      gnss_fix_status = true;
      stop_timestamp = millis();

      Serial.println("\n\nGNSS Module fixed!");
      Serial.print("Time-to-first-fix (milliseconds): ");
      Serial.println(stop_timestamp - start_timestamp);

      Serial.print("Nav. system: ");
      if (nmea.getNavSystem())
        Serial.println(nmea.getNavSystem());
      else
        Serial.println("none");

      Serial.print("Num. satellites: ");
      Serial.println(nmea.getNumSatellites());

      Serial.print("HDOP: ");
      Serial.println(nmea.getHDOP() / 10., 1);

      Serial.print("Date/time: ");
      Serial.print(nmea.getYear());
      Serial.print('-');
      Serial.print(int(nmea.getMonth()));
      Serial.print('-');
      Serial.print(int(nmea.getDay()));
      Serial.print('T');
      Serial.print(int(nmea.getHour()));
      Serial.print(':');
      Serial.print(int(nmea.getMinute()));
      Serial.print(':');
      Serial.println(int(nmea.getSecond()));

      long latitude_mdeg = nmea.getLatitude();
      long longitude_mdeg = nmea.getLongitude();
      Serial.print("Latitude (deg): ");
      Serial.println(latitude_mdeg / 1000000., 6);

      Serial.print("Longitude (deg): ");
      Serial.println(longitude_mdeg / 1000000., 6);

      long alt;
      Serial.print("Altitude (m): ");
      if (nmea.getAltitude(alt))
        Serial.println(alt / 1000., 3);
      else
        Serial.println("not available");

      Serial.print("Speed: ");
      Serial.println(nmea.getSpeed() / 1000., 3);
      Serial.print("Course: ");
      Serial.println(nmea.getCourse() / 1000., 3);
    }
  }
}
