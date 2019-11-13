#include <OneWire.h>


OneWire  ds(8);  // on pin 2 (a 4.7K resistor is necessary)
OneWire ds1(7);

void setup(void) {
  Serial.begin(9600);
}

void loop(void) {
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius;

  byte present1 = 0;
  byte type_s1;
  byte data1[12];
  byte addr1[8];
  float celsius1;

  if ( !ds.search(addr)) {
    ds.reset_search();
    delay(250);
    return;
  }

  if ( !ds1.search(addr1)) {
    ds1.reset_search();
    delay(250);
    return;
  }


  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid on sensor 1!");
      return;
  }

  if (OneWire::crc8(addr1, 7) != addr1[7]) {
    Serial.println("CRC is not valid on sensor 2!");
      return;
  }

  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      type_s = 1;
      break;
    case 0x28:
      type_s = 0;
      break;
    case 0x22:
      type_s = 0;
      break;
    default:
      return;
  }

  switch (addr1[0]) {
    case 0x10:
      type_s1 = 1;
      break;
    case 0x28:
      type_s1 = 0;
      break;
    case 0x22:
      type_s1 = 0;
      break;
    default:
      return;
  }

  ds.reset();
  ds1.reset();

  ds.select(addr);
  ds1.select(addr1);

  ds.write(0x44);        // start conversion, use ds.write(0x44,1) with parasite power on at the end
  ds1.write(0x44);        // start conversion, use ds.write(0x44,1) with parasite power on at the end

  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.

  present = ds.reset();
  present1 = ds1.reset();

  ds.select(addr);
  ds1.select(addr1);

  ds.write(0xBE);         // Read Scratchpad
  ds1.write(0xBE);         // Read Scratchpad

  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    data1[i] = ds1.read();
  }


  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  int16_t raw1 = (data1[1] << 8) | data1[0];

  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;

  if (type_s1) {
    raw1 = raw1 << 3; // 9 bit resolution default
    if (data1[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw1 = (raw1 & 0xFFF0) + 12 - data1[6];
    }
  } else {
    byte cfg1 = (data1[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg1 == 0x00) raw1 = raw1 & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg1 == 0x20) raw1 = raw1 & ~3; // 10 bit res, 187.5 ms
    else if (cfg1 == 0x40) raw1 = raw1 & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius1 = (float)raw1 / 16.0;

  // HEREEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
  Serial.print(celsius);
  Serial.print("|");
  Serial.println(celsius1);
  // HEREEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
}
