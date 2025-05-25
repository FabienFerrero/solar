/*

   ___    _   _             _      ___                      ___      ___    ___      _       __   
  / __|  | | | |  _ _      (_)    / __|   __ _      o O O  | _ \    | __|  |_  )    / |     /  \  
  \__ \  | |_| | | ' \     | |   | (__   / _` |    o       |   /    | _|    / /     | |    | () | 
  |___/   \___/  |_||_|   _|_|_   \___|  \__,_|   TS__[O]  |_|_\   _|_|_   /___|   _|_|_   _\__/  
_|"""""|_|"""""|_|"""""|_|"""""|_|"""""|_|"""""| {======|_|"""""|_| """ |_|"""""|_|"""""|_|"""""| 
"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'./o--000'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-' 

   Author: fferrero, m1nhle, mtnguyen
   Code : RF210_RAK3172_Lab : Provide on STM32U5 Serial port a direct access in write and read to Rak3172 module for AT Command mode
   Date :25/05/3025

 */
 //Library


#include "lorawan_credential.h"
#include <SoftwareSerial.h>


// Define Default LoRaWan ABP credential

// String devAddr = "00000000";
// String nwkkey = "00000000000000000000000000000000";
// String appskey = "00000000000000000000000000000000";



// String appEUI = "0000000000000000";
// String devEUI = "0000000000000000";
// String appkey = "00000000000000000000000000000000";

// STM32 SERIAL4 (second UART)
SoftwareSerial RakSerial(PA1, PA0); // RX, TX

int period = 30000; // period to send LoRaWAN packet in ms
unsigned long currentMillis = 0, getSensorDataPrevMillis = 0;
boolean lora_sending=false;
int wait4txt=0; // int to get a txt to send with LoRa

int txPin = PA0;
int rxPin = PA1;
uint8_t button = 0;
char c;

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("RF210 Lab");
  pinMode(PE0, OUTPUT);
 digitalWrite(PE0, HIGH); // swith on LED
   pinMode(txPin, OUTPUT);
  pinMode(rxPin, INPUT);
  pinMode(PA6, OUTPUT); // Switch ON RAK LDO
  digitalWrite(PA6, HIGH);
  delay(3000);

  Serial.begin(115200);
  while (!Serial)
    ;
    
   pinMode(PE5, INPUT_PULLUP);  // Button user 1
  attachInterrupt(digitalPinToInterrupt(PE5), i_button_isr, CHANGE);
 // pinMode(1, OUTPUT); // GNSS enable
  blink(500);

  digitalWrite(PE0, LOW);
  RakSerial.begin(115200);
  Serial.println("RF210Lab");
  delay(1000);  
  //RakSerial.println("ATR");  // Reset RAK3172
  //delay(200);
  
  flush_serial_AT(true); 
 
}

void loop()
{
  if (button > 0){

  delay(500);

  if (digitalRead(PE5)==0) {
  
    // Start GNSS NMEA

  digitalWrite(PE0, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(PE0, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);  
   }
  else { 
  SetLoRaABP();
  delay(2000);
  lora_sending=!lora_sending;
  delay(300);
  blink(200);
    
  }
button=0;
  }

  if (lora_sending==1){
      currentMillis = millis();  
      
      if (currentMillis - getSensorDataPrevMillis > period){
        getSensorDataPrevMillis=currentMillis;
        SendLoRa(1);        
      }
  }
  
  if (Serial.available()) {      // If anything comes in Serial (USB),
  c=Serial.read();
  if(wait4txt>0){
        String txt=Serial.readStringUntil('\r');
        Serial.println(txt);
        SendTxt(txt);
        flush_serial_AT(true);        
        wait4txt=0;
  }
  if(c=='$'){     
    blink(50);
    
    String CMD=Serial.readStringUntil('\r');
    Serial.println(CMD);           
    
    if(CMD.equalsIgnoreCase("ID")){
      Serial.println("Device N°16");
      }   
    else if(CMD.equalsIgnoreCase("SetABP")){
      SetLoRaABP();
      }  
    else if(CMD.equalsIgnoreCase("SetOTAA")){
      SetLoRaOTAA();
      } 
    else if(CMD.equalsIgnoreCase("SendLoRa")){
      SendLoRa(1);
      }
    else if(CMD.equalsIgnoreCase("Periodic")){
      lora_sending=!lora_sending;
      }
     else if(CMD.equalsIgnoreCase("StartCW")){
      StartCW();
      }
     else if(CMD.equalsIgnoreCase("StartFSK600")){
      SetFSK(0);
      }
     else if(CMD.equalsIgnoreCase("StartFSK6000")){
      SetFSK(1);
      } 
     else if(CMD.equalsIgnoreCase("StartFSK60000")){
      SetFSK(2);
      } 
     else if(CMD.equalsIgnoreCase("StartFSKWB600")){
      SetFSK(3);
      }    
     else if(CMD.equalsIgnoreCase("StartFSK")){
      StartFSK();
      }
      else if(CMD.equalsIgnoreCase("StartLoRa")){
      StartLoRa();
      }
       else if(CMD.equalsIgnoreCase("TestLoRaSF")){
      TestLoRaSF();
      }
      else if(CMD.equalsIgnoreCase("TestLoRaBW")){
      TestLoRaBW();
      }
      else if(CMD.equalsIgnoreCase("StartRX")){
      StartRX();
      }
      else if(CMD.equalsIgnoreCase("StopRX")){
      StopRX();
      } 
       else if(CMD.equalsIgnoreCase("Sendtxt")){
        Serial.print("Type your text :");
        wait4txt=1;
        flush_serial_AT(false);
      }          
    
    else{
     Serial.println("Wrong Command");
        }  
  }
    else{
    RakSerial.write(c);   // read it and send it out Serial1  
    }    
  }

  if (RakSerial.available()) {     // If anything comes in Serial1 
    Serial.write(RakSerial.read());   // read it and send it out Serial (USB)
  }
}

void i_button_isr(void) {
button++; 
}



void SetLoRaABP(){
  RakSerial.println("AT+NWM=1"); // Set LORAWAN
  Serial.println("AT+NWM=1 // Set LORAWAN"); // Set P2P_LORA
  delay(200);
  flush_serial_AT(true);
  delay(1000);
  flush_serial_AT(true);
  RakSerial.println("AT+NJM=0"); // Set ABP
  Serial.println("AT+NJM=0 // Set ABP"); // Set ABP
  delay(200);
  flush_serial_AT(true); 
  RakSerial.println("AT+BAND=4");// Set EU868 frequency band
  Serial.println("AT+BAND=4 // Set EU868 frequency band");// Set EU868 frequency band
  delay(200);
  flush_serial_AT(true);
  RakSerial.println("AT+DR=5");// Set EU866 frequency band
  Serial.println("AT+DR=5 // Set EU868 frequency band");// Set EU868 frequency band  
  delay(200); 
  flush_serial_AT(true);
  RakSerial.printf("AT+DEVADDR=");
  RakSerial.println(devAddr);
  Serial.printf("AT+DEVADDR=");
  Serial.println(devAddr);
  delay(200);
  flush_serial_AT(true);
  RakSerial.printf("AT+NWKSKEY=");
  RakSerial.println(nwkkey);
  Serial.printf("AT+NWKSKEY=");
  Serial.println(nwkkey);
  delay(200);
  flush_serial_AT(true);
  RakSerial.printf("AT+APPSKEY=");
  RakSerial.println(appskey);
  Serial.printf("AT+APPSKEY=");
  Serial.println(appskey);
  delay(200);
  flush_serial_AT(true);
}

void SetLoRaOTAA(){
  RakSerial.println("AT+NWM=1"); // Set LORAWAN
  Serial.println("AT+NWM=1 // Set LORAWAN"); // Set P2P_LORA
  delay(200);
  flush_serial_AT(true);
  delay(1000);
  flush_serial_AT(true);
  RakSerial.println("AT+NJM=1"); // Set OTAA
  Serial.println("AT+NJM=1 // Set OTAA mode"); // Set OTAA
  delay(200);
  flush_serial_AT(true); 
  RakSerial.println("AT+BAND=4");// Set EU868 frequency band
  Serial.println("AT+BAND=4 // Set EU868 frequency band");// Set EU868 frequency band
  delay(200);
  flush_serial_AT(true);
  RakSerial.println("AT+DR=5");// Set SF7
  Serial.println("AT+DR=5 // Set SF7 (default is SF12)");// Set SF7  
  delay(200); 
  flush_serial_AT(true);
  RakSerial.printf("AT+DEVEUI=");
  RakSerial.println(devEUI);
  Serial.printf("AT+DEVEUI=");
  Serial.println(devEUI);
  delay(200);
  flush_serial_AT(true);
  RakSerial.printf("AT+APPEUI=");
  RakSerial.println(appEUI);
  Serial.printf("AT+APPEUI=");
  Serial.println(appEUI);
  delay(200);
  flush_serial_AT(true);
  RakSerial.printf("AT+APPKEY=");
  RakSerial.println(appkey);
  Serial.printf("AT+APPKEY=");
  Serial.println(appkey);
  delay(200);
  flush_serial_AT(true);
}

void StartLoRa(){
  RakSerial.println("AT+NWM=0"); // Set P2P_LORA
  Serial.println("AT+NWM=0 // Set P2P_LORA"); // Set P2P_LORA
  delay(1000);
  RakSerial.println("AT+PFREQ=866000000");// Set frequency @868MHz 
  Serial.println("AT+PFREQ=866000000 // Set frequency @868MHz ");// Set frequency 
  delay(200);
  RakSerial.println("AT+PSF=12");// Set Spreading Factor @12 
  Serial.println("AT+PSF=12 // Set Spreading Factor @12 [5;12] ");// Set SF 
  delay(200);
  RakSerial.println("AT+PREAMBLELENGTH=7");// Set Peamble Length @7 
  Serial.println("AT+PREAMBLELENGTH=7 // Set Peamble Length @7  ");// Set SF 
  delay(200);
  RakSerial.println("AT+PBW=0");// Set Freq. Bandwidth [0:9] 
  Serial.println("AT+PBW=0 // Set Freq. Bandwidth [0 = 125, 1 = 250, 2 = 500, 3 = 7.8, 4 = 10.4, 5 = 15.63, 6 = 20.83, 7 = 31.25, 8 = 41.67, 9 = 62.5]  ");// Set BW
  delay(200);
  RakSerial.println("AT+PTP=10"); // Set Tx Power [5:22] 
  Serial.println("AT+PTP=10 // Set Tx Power [5:22]  "); // Set Tx Pow
  delay(200);
  RakSerial.println("AT+ENCRY=0"); // Set Tx Power [5:22] 
  Serial.println("AT+ENCRY=0 // disable encryption  "); // Disable encryption
  delay(200);
  RakSerial.println("AT+PSEND=48656C6C6F20576F726C64"); // Send Hello World
  Serial.println("AT+PSEND=48656C6C6F20576F726C64 // Send Hello World");
  delay(200);
  flush_serial_AT(true);  
}

void TestLoRaSF(){

  for (int i=5; i<=12;i++){
    RakSerial.print("AT+PSF=");// Set Spreading Factor @12
    RakSerial.println(i);// Set Spreading Factor @12
  Serial.print("AT+PSF=");// Set SF 
  Serial.println(i);// Set SF 
  delay(200);
  RakSerial.println("AT+PSEND=48656C6C6F20576F726C64"); // Send Hello World
  Serial.println("AT+PSEND=48656C6C6F20576F726C64 // Send Hello World");
  delay(10*i*i);
  flush_serial_AT(true);
  }
}

void TestLoRaBW(){
Serial.println("Sweep from 32KHz to 500 KHz");
RakSerial.println("AT+PSF=10");// Set Spreading Factor @10
Serial.println("AT+PSF=10 // Set Spreading Factor @10");// Set Spreading Factor @10
 delay(200);
 int BW_tab[]={7,9,0,1,2};
  for (int i=0; i<=4;i++){
    RakSerial.print("AT+PBW=");// Set BW
    RakSerial.println(BW_tab[i]);// Set BW
  Serial.print("AT+PBW=");// Set BW 
  Serial.println(BW_tab[i]);// Set bw 
  delay(200);
  RakSerial.println("AT+PSEND=48656C6C6F20576F726C64"); // Send Hello World
  Serial.println("AT+PSEND=48656C6C6F20576F726C64 // Send Hello World");
  delay(300*(5-i));
  flush_serial_AT(true);
  }
  RakSerial.println("AT+PSF=12");// Set Spreading Factor @12
  RakSerial.println("AT+PBW=0");// Set Spreading Factor @12
}


void SendTxt(String txt){
String hex = stringToHex(txt);
RakSerial.print("AT+PSEND="); // Send Txt
RakSerial.println(hex); // Send Txt
Serial.print("AT+PSEND=");
Serial.println(hex);
}

String stringToHex(String input) {
  String output = "";  
  for (int i = 0; i < input.length(); i++) {
    // Convertir chaque caractère en hexadécimal
    char c = input.charAt(i);
    output += String(c, HEX);
  }  
  return output;
}

void StartFSK(){
  RakSerial.println("AT+NWM=2"); // Set FSK
  Serial.println("AT+NWM=2 // Set FSK"); // Set FSK
  delay(1000);
  RakSerial.println("AT+PFREQ=866000000");// Set frequency @868MHz 
  Serial.println("AT+PFREQ=866000000 // Set frequency @866MHz ");// Set frequency 
  delay(200);
  RakSerial.println("AT+PBR=600");// Set FSK Bitrate [600:300000] b/s 
  Serial.println("AT+PBR=600 // Set FSK Bitrate [600:300000] b/s  ");// Set Bitrate
  delay(200);
  RakSerial.println("AT+PFDEV=600");// Set Freq deviation [600:200000] Hz 
  Serial.println("AT+PFDEV=600 // Set Freq deviation [600:200000] Hz ");// Set Freq dev
  delay(200);
  RakSerial.println("AT+PTP=10");// Set Tx Power [5:22] 
  Serial.println("AT+PTP=10 // Set Tx Power [5:22]"); // Set Tx Pow
  delay(200);
  RakSerial.println("AT+PSEND=48656C6C6F20576F726C64");// Send Hello World 
  Serial.println("AT+PSEND=48656C6C6F20576F726C64 // Send Hello World"); //Send Hello World 
  delay(200);
  flush_serial_AT(true);  
}

void SetFSK(int i){
 if(i==0){
  RakSerial.println("AT+PBR=600");// Set FSK Bitrate [600:300000] b/s 
  Serial.println("AT+PBR=600 // Set FSK Bitrate [600:300000] b/s  ");// Set Bitrate
  delay(200);
  RakSerial.println("AT+PFDEV=600");// Set Freq deviation [600:200000] Hz 
  Serial.println("AT+PFDEV=600 // Set Freq deviation [600:200000] Hz ");// Set Freq dev
  delay(200);
  RakSerial.println("AT+PSEND=00000000000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");// Send 96 bytes 
  Serial.println("AT+PSEND=00000000000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF// Send 96 bytes"); //Send Hello World 
  flush_serial_AT(true);
 }
 if(i==1){
  RakSerial.println("AT+PBR=6000");// Set FSK Bitrate [600:300000] b/s 
  Serial.println("AT+PBR=6000 // Set FSK Bitrate [600:300000] b/s  ");// Set Bitrate
  delay(200);
  RakSerial.println("AT+PFDEV=6000");// Set Freq deviation [600:200000] Hz 
  Serial.println("AT+PFDEV=6000 // Set Freq deviation [600:200000] Hz ");// Set Freq dev
  delay(200);
  RakSerial.println("AT+PSEND=00000000000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");// Send 96 bytes 
  Serial.println("AT+PSEND=00000000000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF // Send 96 bytes"); //Send Hello World 
  flush_serial_AT(true);
 } 
  if(i==2){
  RakSerial.println("AT+PBR=60000");// Set FSK Bitrate [600:300000] b/s 
  Serial.println("AT+PBR=60000 // Set FSK Bitrate [600:300000] b/s  ");// Set Bitrate
  delay(200);
  RakSerial.println("AT+PFDEV=60000");// Set Freq deviation [600:200000] Hz 
  Serial.println("AT+PFDEV=60000 // Set Freq deviation [600:200000] Hz ");// Set Freq dev
  delay(200);
  RakSerial.println("AT+PSEND=00000000000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");// Send 96 bytes 
  Serial.println("AT+PSEND=00000000000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF // Send 96 bytes"); //Send Hello World  
  flush_serial_AT(true);
 } 
   if(i==3){
  RakSerial.println("AT+PBR=600");// Set FSK Bitrate [600:300000] b/s 
  Serial.println("AT+PBR=600 // Set FSK Bitrate [600:300000] b/s  ");// Set Bitrate
  delay(200);
  RakSerial.println("AT+PFDEV=200000");// Set Freq deviation [600:200000] Hz 
  Serial.println("AT+PFDEV=200000 // Set Freq deviation [600:200000] Hz ");// Set Freq dev
  delay(200);
  RakSerial.println("AT+PSEND=00000000000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");// Send 96 bytes 
  Serial.println("AT+PSEND=00000000000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF // Send 96 bytes"); //Send Hello World  
  flush_serial_AT(true);
 } 
  
}

void StartCW(){
   RakSerial.println("AT+CW=866000000:10:1"); // Set CW 10dBm for 1s
   Serial.println("AT+CW=866000000:10:1 // Set CW 10dBm for 1s");
  delay(200);
  flush_serial_AT(true);   
}

void StopRX(){
   RakSerial.println("AT+PRECV=0"); // Set Rx mode continuous
   Serial.println("AT+PRECV=0 // Stop Rx mode Continuous");
  delay(200);
  flush_serial_AT(true);   
}

void StartRX(){
   RakSerial.println("AT+PRECV=65533"); // Set Rx mode continuous
   Serial.println("AT+PRECV=65533 // Set Rx mode Continuous");
  delay(200);
  flush_serial_AT(true);   
}

bool SendLoRa(uint8_t mode){

int16_t rx_delay=0;
int16_t t=(int16_t) 100*measure_temp(); // return temperature in cents of degree
uint8_t h=(uint8_t)2*measure_hum(); // return humidity in percent
int8_t x = 50*measure_acc(1);
int8_t y = 50*measure_acc(2);
int8_t z = 50*measure_acc(3);
int16_t l = 10*measure_lum();
uint16_t b = 0;
uint16_t bat = measure_bat();


//int blocks=7;
int i=0;
unsigned char mydata[64];
mydata[i++] = t >> 8;
mydata[i++] = t & 0xFF;
mydata[i++] = h;
mydata[i++] = x;
mydata[i++] = y;
mydata[i++] = z;
mydata[i++] = l >> 8;
mydata[i++] = l & 0xFF;
mydata[i++] = b >> 8;
mydata[i++] = b & 0xFF;
mydata[i++] = bat >> 8;
mydata[i++] = bat & 0xFF;

char str[32];
array_to_string(mydata, i, str);

flush_serial_AT(false); 
blink(100);
RakSerial.printf("AT+SEND=4:");
RakSerial.println(str);

while (RakSerial.available() == 0)
{
delay(100);
}
RakSerial.readStringUntil('\n');
while (RakSerial.available() == 0)
{
rx_delay=rx_delay+100;
delay(100);
if(rx_delay>8000){
  Serial.println("Error during uplink process (do you perform the JOIN ?)");
  return false;}
}
Serial.print("Rx delay : ");
Serial.println(rx_delay);


   if (RakSerial.available())
  { // If anything comes in Serial1
    while (RakSerial.available())
      Serial.write(RakSerial.read()); // read it and send it out Serial (USB)
  }
  Serial.println("AT set complete with downlink");
  return true;
}

  
 void array_to_string(byte array[], unsigned int len, char buffer[])
{
    for (unsigned int i = 0; i < len; i++)
    {
        byte nib1 = (array[i] >> 4) & 0x0F;
        byte nib2 = (array[i] >> 0) & 0x0F;
        buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
        buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
    }
    buffer[len*2] = '\0';
}

 // Return temperature level in degree
 float measure_temp(){

//Serial.flush();
flush_serial_AT(false);// flush AT Serial reading buffer
  
RakSerial.println("ATC+TEMP=?"); // Request bat value
 String temperature;
 delay(100);

 if(RakSerial.available()){
        temperature = RakSerial.readStringUntil('\n');
        Serial.print("Temperature:");
        Serial.println(temperature);
 }
 
return temperature.toFloat();
 }

 // Return humidity level in percent
 float measure_hum(){

//Serial.flush();
flush_serial_AT(false);// flush AT Serial reading buffer
  
RakSerial.println("ATC+HUM=?"); // Request bat value
 String hum;
 delay(100);
 if(RakSerial.available()){
        hum = RakSerial.readStringUntil('\n');
        Serial.print("Humidity:");
        Serial.println(hum);
 } 
return hum.toFloat();
 }

// Return humidity level in percent
 float measure_lum(){


flush_serial_AT(false);// flush AT Serial reading buffer
  
RakSerial.println("ATC+LUM=?"); // Request bat value
 String lum;
 delay(100);

 if(RakSerial.available()){
        lum = RakSerial.readStringUntil('\n');
        Serial.print("Luminosity:");
        Serial.println(lum);
 }
 
return lum.toFloat();
 } 

  // Return Acceleration level in G
 float measure_acc(int axis){


flush_serial_AT(false);// flush AT Serial reading buffer

if(axis==1){  
RakSerial.println("ATC+AX=?"); // Request bat value
}
else if(axis==2){  
RakSerial.println("ATC+AY=?"); // Request bat value
}
else if(axis==3){  
RakSerial.println("ATC+AZ=?"); // Request bat value
}
 String a;
 delay(100);
 if(RakSerial.available()){
        a = RakSerial.readStringUntil('\n');        
 }
 
return a.toFloat();
 }

// Return bat level in mv
 float measure_bat(){

//Serial.flush();
flush_serial_AT(false);// flush AT Serial reading buffer
  
RakSerial.println("ATC+BAT=?"); // Request bat value
 String bat;
  while (RakSerial.available() == 0)
{
delay(100);
  }
 delay(100);

 if(RakSerial.available()){
        bat = RakSerial.readStringUntil('\n');
        Serial.print("Bat:");
        Serial.println(bat);
 }
 
return bat.toFloat();
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

void blink(int blinktime){
digitalWrite(PE0, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(blinktime);                       // wait for a second
  digitalWrite(PE0, LOW);    // turn the LED off by making the voltage LOW
} 

