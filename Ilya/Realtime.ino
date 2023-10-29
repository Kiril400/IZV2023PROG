#include "Wire.h"
// Include the Wire library for I2C communication.
#define DS3231_I2C_ADDRESS 0x68
// Define the I2C address of the DS3231 RTC module.

byte decToBcd(byte val) {
  // Convert a decimal value to binary-coded decimal (BCD) format.
  return (val / 10 * 16) + (val % 10);
}

byte bcdToDec(byte val) {
  // Convert a BCD value to decimal format.
  return (val / 16 * 10) + (val % 16);
}

void setup() {
  Wire.begin();
  // Initialize the I2C communication.
  Serial.begin(9600);
  // Initialize serial communication for debugging.
  setDS3231time(30, 42, 16, 5, 13, 10, 16);
  // Set the date and time on the DS3231 RTC module.
}

void setDS3231time(byte sec, byte min, byte hour, byte weekd, byte mond, byte mon, byte year) {
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  // Start communication with the DS3231 RTC module.
  Wire.write(0);
  // Set the register address to 0 (the beginning of the time and date data).
  Wire.write(decToBcd(sec));
  // Write the seconds data to the DS3231 in BCD format.
  Wire.write(decToBcd(min));
  // Write the minutes data in BCD format.
  Wire.write(decToBcd(hour));
  // Write the hours data in BCD format.
  Wire.write(decToBcd(weekd));
  // Write the day of the week data in BCD format.
  Wire.write(decToBcd(mond));
  // Write the day of the month data in BCD format.
  Wire.write(decToBcd(mon));
  // Write the month data in BCD format.
  Wire.write(decToBcd(year));
  // Write the year data in BCD format.
  Wire.endTransmission();
  // End communication with the DS3231 RTC module.
}

void readDS3231time(byte *sec, byte *min, byte *hour, byte *weekd, byte *mond, byte *mon, byte *year) {
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  // Start communication with the DS3231 RTC module.
  Wire.write(0);
  // Set the register address to 0 (the beginning of the time and date data).
  Wire.endTransmission();
  // End the transmission.
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // Request 7 bytes of data from the DS3231.
  *sec = bcdToDec(Wire.read() & 0x7F);
  // Read the seconds data, convert from BCD to decimal, and mask the oscillator on/off bit.
  *min = bcdToDec(Wire.read());
  // Read the minutes data and convert from BCD to decimal.
  *hour = bcdToDec(Wire.read() & 0x3F);
  // Read the hours data, convert from BCD to decimal, and mask the 12/24-hour bit.
  *weekd = bcdToDec(Wire.read());
  // Read the day of the week data and convert from BCD to decimal.
  *mond = bcdToDec(Wire.read());
  // Read the day of the month data and convert from BCD to decimal.
  *mon = bcdToDec(Wire.read());
  // Read the month data and convert from BCD to decimal.
  *year = bcdToDec(Wire.read());
  // Read the year data and convert from BCD to decimal.
}

void displayTime() {
  byte sec, min, hour, weekd, mond, mon, year;
  readDS3231time(&sec, &min, &hour, &weekd, &mond, &mon, &year);
  // Read the time and date from the DS3231 RTC module.
  Serial.print(hour < 10 ? "0" : "");
  // If the hour is less than 10, print a leading zero.
  Serial.print(hour, DEC);
  // Print the hour in decimal format.
  Serial.print(":");
  Serial.print(min < 10 ? "0" : "");
  // If the minute is less than 10, print a leading zero.
  Serial.print(min, DEC);
  // Print the minute in decimal format.
  Serial.print(":");
  Serial.print(sec < 10 ? "0" : "");
  // If the second is less than 10, print a leading zero.
  Serial.print(sec, DEC);
  // Print the second in decimal format.
  Serial.print(" ");
  Serial.print(mond, DEC);
  // Print the day of the month in decimal format.
  Serial.print("/");
  Serial.print(mon, DEC);
  // Print the month in decimal format.
  Serial.print("/");
  Serial.print(year, DEC);
  // Print the year in decimal format.
  Serial.print(" Day of the week: ");
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
  // Display the current time and date.
  delay(1000);
  // Delay for one second.
}
