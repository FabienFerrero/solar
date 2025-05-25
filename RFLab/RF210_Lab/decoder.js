function Decoder(bytes, port) {
 
  var decoded = {};

  if( port == 4) {
    var temperature = ((bytes[0] << 8) | bytes[1]);
     if(temperature >= 0x8000) temperature = -(0x10000 - temperature);
    decoded.temperature = temperature / 100.0;
    decoded.humidity = bytes[2]/2;
    x = bytes[3];
     if (x >= 0x80) x = -(0x100 - x);
    decoded.accx = x/50;
    y = bytes[4];
     if (y >= 0x80) y = -(0x100 - y);
    decoded.accy = y/50;
    z = bytes[5];
     if (z >= 0x80) z = -(0x100 - z);
    decoded.accz = z/50;
    var luminosity = ((bytes[6] << 8) | bytes[7]);
    decoded.luminosity = luminosity/10;
    var BLE = ((bytes[8] << 8) | bytes[9]);
    decoded.ble=BLE;
    voltraw = ((bytes[10] << 8) | bytes[11]);
    decoded.voltage = (voltraw) /1000.0;
    
    decoded.length = bytes.length;
  }
  
  return decoded;
}
