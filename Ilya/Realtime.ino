#include "Wire.h"
#define DS3231_I2C_ADDRESS 0x68

byte decToBcd(byte val) {
  return (val / 10 * 16) + (val % 10);
}

byte bcdToDec(byte val) {
  return (val / 16 * 10) + (val % 16);
}

void setup() {
  Wire.begin();
  Serial.begin(9600);
  setDS3231time(30, 42, 16, 5, 13, 10, 16);
}

void setDS3231time(byte sec, byte min, byte hour, byte weekd, byte mond, byte mon, byte year) {
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0);
  Wire.write(decToBcd(sec));
  Wire.write(decToBcd(min));
  Wire.write(decToBcd(hour));
  Wire.write(decToBcd(weekd));
  Wire.write(decToBcd(mond));
  Wire.write(decToBcd(mon));
  Wire.write(decToBcd(year));
  Wire.endTransmission();
}

void readDS3231time(byte *sec, byte *min, byte *hour, byte *weekd, byte *mond, byte *mon, byte *year) {
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0);
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  *sec = bcdToDec(Wire.read() & 0x7F);
  *min = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3F);
  *weekd = bcdToDec(Wire.read());
  *mond = bcdToDec(Wire.read());
  *mon = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}

void displayTime() {
  byte sec, min, hour, weekd, mond, mon, year;
  readDS3231time(&sec, &min, &hour, &weekd, &mond, &mon, &year);
  Serial.print(hour < 10 ? "0" : "");
  Serial.print(hour, DEC);
  Serial.print(":");
  Serial.print(min < 10 ? "0" : "");
  Serial.print(min, DEC);
  Serial.print(":");
  Serial.print(sec < 10 ? "0" : "");
  Serial.print(sec, DEC);
  Serial.print(" ");
  Serial.print(mond, DEC);
  Serial.print("/");
  Serial.print(mon, DEC);
  Serial.print("/");
  Serial.print(year, DEC);
  Serial.print(" Day of week: ");
  switch (weekd) {
    case 1:
      Serial.println("Monday");
      break;
    case 2:
      Serial.println("Tuesday");
      break;
    case 3:
      Serial.println("Wednesday");
      break;
    case 4:
      Serial.println("Thursday");
      break;
    case 5:
      Serial.println("Friday");
      break;
    case 6:
      Serial.println("Saturday");
      break;
    case 7:
      Serial.println("Sunday");
      break;
  }
}

void loop() {
  displayTime();
  delay(1000);
}

