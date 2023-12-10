
// ------------------------- SETTINGS --------------------
#define RST_CLK 0       // Reset clock to firmware loading time
#define SENSOR_TIME 30000     // Sensor reading update time on the screen, milliseconds
#define LED_TYPE 0          // RGB LED type: 0 - common cathode, 1 - common anode

// Brightness control
#define BRIGHT_LIMIT 150  // Signal level below which brightness will switch to minimum (0-1023)
#define LED_MAX_BR 255    // Maximum CO2 LED brightness (0 - 255)
#define LED_MIN_BR 10     // Minimum CO2 LED brightness (0 - 255)
#define LCD_MAX_BR 255    // Maximum LCD backlight brightness (0 - 255)
#define LCD_MIN_BR 10     // Minimum LCD backlight brightness (0 - 255)
#define DISPLAY_I2C_ADDR 0x27   // Display board I2C address
// Display limits for graphs
#define MIN_TEMP 15
#define MAX_TEMP 35
#define MIN_HUM 0
#define MAX_HUM 100
#define MIN_PRESS -100
#define MAX_PRESS 100
#define MIN_CO2 300
#define MAX_CO2 2000

// Pins
#define DISP_LIGHT 10
#define LIGHT_SENS A3

#define RX_MHZ 2
#define TX_MHZ 3

#define LED_COMMON_PIN 7
#define LED_RED_PIN 9
#define LED_GREEN_PIN 6
#define LED_BLUE_PIN 5
#define SENSOR_BUTTON_PIN 4

#define DISP_BACKLIT_PIN 10     // Display backlight pin
#define LIGHT_SENS_PIN 0   // Photoresistor pin

// Libraries
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C LCD_DISP(DISPLAY_I2C_ADDR, 20, 4);

#include "RTClib.h"
RTC_DS3231 rtc;
DateTime RealTime;

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 BME_SENS;

#include <MHZ19_uart.h>
MHZ19_uart MHZ_SENS;

#include <GyverTimer.h>
GTimer_ms sensorsTimer(SENSOR_TIME);
GTimer_ms drawSensorsTimer(SENSOR_TIME);
GTimer_ms clockTimer(500);
GTimer_ms hourPlotTimer((long)4 * 60 * 1000);         // 4 minutes
GTimer_ms dayPlotTimer((long)1.6 * 60 * 60 * 1000);   // 1.6 hours
GTimer_ms plotTimer(240000);
GTimer_ms predictTimer((long)10 * 60 * 1000);         // 10 minutes
GTimer_ms brightTimer(2000);

#include "GyverButton.h"
GButton button(SENSOR_BUTTON_PIN, LOW_PULL, NORM_OPEN);

int8_t hrs, mins, secs;
byte mode = 0;
/*
  0 clock and data
  1 temperature graph for the hour
  2 temperature graph for the day
  3 humidity graph for the hour
  4 humidity graph for the day
  5 pressure graph for the hour
  6 pressure graph for the day
  7 CO2 graph for the hour
  8 CO2 graph for the day
*/

// Variables for display
float dispTemp;
byte dispHum;
int dispPres;
int dispCO2;
int dispRain;

// Arrays for graphs
int tempHour[15], tempDay[15];
int humHour[15], humDay[15];
int pressHour[15], pressDay[15];
int co2Hour[15], co2Day[15];
int delta;
uint32_t pressure_array[6];
uint32_t sumX, sumY, sumX2, sumXY;
float a, b;
byte time_array[6];

// Characters
// Graph

byte row8[8] = {0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
byte row7[8] = {0b00000,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
byte row6[8] = {0b00000,  0b00000,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
byte row5[8] = {0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
byte row4[8] = {0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111,  0b11111};
byte row3[8] = {0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111};
byte row2[8] = {0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111};
byte row1[8] = {0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111};

// numbers
uint8_t LT[8] = {0b00111,  0b01111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
uint8_t UB[8] = {0b11111,  0b11111,  0b11111,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000};
uint8_t RT[8] = {0b11100,  0b11110,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
uint8_t LL[8] = {0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b01111,  0b00111};
uint8_t LB[8] = {0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111};
uint8_t LR[8] = {0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11110,  0b11100};
uint8_t UMB[8] = {0b11111,  0b11111,  0b11111,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111};
uint8_t LMB[8] = {0b11111,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111};

void drawDig(byte dig, byte x, byte y) {
  switch (dig) {
    case 0:
      LCD_DISP.setCursor(x, y); // set cursor to column 0, line 0 (first row)
      LCD_DISP.write(0);  // call each segment to create
      LCD_DISP.write(1);  // top half of the number
      LCD_DISP.write(2);
      LCD_DISP.setCursor(x, y + 1); // set cursor to colum 0, line 1 (second row)
      LCD_DISP.write(3);  // call each segment to create
      LCD_DISP.write(4);  // bottom half of the number
      LCD_DISP.write(5);
      break;
    case 1:
      LCD_DISP.setCursor(x + 1, y);
      LCD_DISP.write(1);
      LCD_DISP.write(2);
      LCD_DISP.setCursor(x + 2, y + 1);
      LCD_DISP.write(5);
      break;
    case 2:
      LCD_DISP.setCursor(x, y);
      LCD_DISP.write(6);
      LCD_DISP.write(6);
      LCD_DISP.write(2);
      LCD_DISP.setCursor(x, y + 1);
      LCD_DISP.write(3);
      LCD_DISP.write(7);
      LCD_DISP.write(7);
      break;
    case 3:
      LCD_DISP.setCursor(x, y);
      LCD_DISP.write(6);
      LCD_DISP.write(6);
      LCD_DISP.write(2);
      LCD_DISP.setCursor(x, y + 1);
      LCD_DISP.write(7);
      LCD_DISP.write(7);
      LCD_DISP.write(5);
      break;
    case 4:
      LCD_DISP.setCursor(x, y);
      LCD_DISP.write(3);
      LCD_DISP.write(4);
      LCD_DISP.write(2);
      LCD_DISP.setCursor(x + 2, y + 1);
      LCD_DISP.write(5);
      break;
    case 5:
      LCD_DISP.setCursor(x, y);
      LCD_DISP.write(0);
      LCD_DISP.write(6);
      LCD_DISP.write(6);
      LCD_DISP.setCursor(x, y + 1);
      LCD_DISP.write(7);
      LCD_DISP.write(7);
      LCD_DISP.write(5);
      break;
    case 6:
      LCD_DISP.setCursor(x, y);
      LCD_DISP.write(0);
      LCD_DISP.write(6);
      LCD_DISP.write(6);
      LCD_DISP.setCursor(x, y + 1);
      LCD_DISP.write(3);
      LCD_DISP.write(7);
      LCD_DISP.write(5);
      break;
    case 7:
      LCD_DISP.setCursor(x, y);
      LCD_DISP.write(1);
      LCD_DISP.write(1);
      LCD_DISP.write(2);
      LCD_DISP.setCursor(x + 1, y + 1);
      LCD_DISP.write(0);
      break;
    case 8:
      LCD_DISP.setCursor(x, y);
      LCD_DISP.write(0);
      LCD_DISP.write(6);
      LCD_DISP.write(2);
      LCD_DISP.setCursor(x, y + 1);
      LCD_DISP.write(3);
      LCD_DISP.write(7);
      LCD_DISP.write(5);
      break;
    case 9:
      LCD_DISP.setCursor(x, y);
      LCD_DISP.write(0);
      LCD_DISP.write(6);
      LCD_DISP.write(2);
      LCD_DISP.setCursor(x + 1, y + 1);
      LCD_DISP.write(4);
      LCD_DISP.write(5);
      break;
    case 10:
      LCD_DISP.setCursor(x, y);
      LCD_DISP.write(32);
      LCD_DISP.write(32);
      LCD_DISP.write(32);
      LCD_DISP.setCursor(x, y + 1);
      LCD_DISP.write(32);
      LCD_DISP.write(32);
      LCD_DISP.write(32);
      break;
  }
}

void drawdots(byte x, byte y, boolean state) {
  byte code;
  if (state) code = 165;
  else code = 32;
  LCD_DISP.setCursor(x, y);
  LCD_DISP.write(code);
  LCD_DISP.setCursor(x, y + 1);
  LCD_DISP.write(code);
}

void drawClock(byte hours, byte minutes, byte x, byte y, boolean dotState) {
  // make it blank 
  LCD_DISP.setCursor(x, y);
  LCD_DISP.print("               ");
  LCD_DISP.setCursor(x, y + 1);
  LCD_DISP.print("               ");

  //if (hours > 23 || minutes > 59) return;
  if (hours / 10 == 0) drawDig(10, x, y);
  else drawDig(hours / 10, x, y);
  drawDig(hours % 10, x + 4, y);
  drawDig(minutes / 10, x + 8, y);
  drawDig(minutes % 10, x + 12, y);
}


static const char *dayNames[]  = {
  "Sund",
  "Mond",
  "Tues",
  "Wedn",
  "Thur",
  "Frid",
  "Satu",
};


void drawData() {
  LCD_DISP.setCursor(15, 0);
  if (RealTime.day() < 10) LCD_DISP.print(0);
  LCD_DISP.print(RealTime.day());
  LCD_DISP.print(".");
  if (RealTime.month() < 10) LCD_DISP.print(0);
  LCD_DISP.print(RealTime.month());

}

void drawPlot(byte pos, byte row, byte width, byte height, int min_val, int max_val, int *plot_array, String label) {
  int max_value = -32000;
  int min_value = 32000;

  for (byte i = 0; i < 15; i++) {
    if (plot_array[i] > max_value) max_value = plot_array[i];
    if (plot_array[i] < min_value) min_value = plot_array[i];
  }
  LCD_DISP.setCursor(16, 0); LCD_DISP.print(max_value);
  LCD_DISP.setCursor(16, 1); LCD_DISP.print(label);
  LCD_DISP.setCursor(16, 2); LCD_DISP.print(plot_array[14]);
  LCD_DISP.setCursor(16, 3); LCD_DISP.print(min_value);

  for (byte i = 0; i < width; i++) {                  // For each column of parameters
    int fill_val = plot_array[i];
    fill_val = constrain(fill_val, min_val, max_val);
    byte infill, fract;
    // Find the number of full blocks considering the minimum and maximum for graph display
    if (plot_array[i] > min_val)
      infill = floor((float)(plot_array[i] - min_val) / (max_val - min_val) * height * 10);
    else infill = 0;
    fract = (float)(infill % 10) * 8 / 10;                   // Find the number of remaining stripes
    infill = infill / 10;

    for (byte n = 0; n < height; n++) {     // For each line of the graph
      if (n < infill && infill > 0) {       // While we are below the level
        LCD_DISP.setCursor(i, (row - n));        // Fill with full cells
        LCD_DISP.write(0);
      }
      if (n >= infill) {                    // If we reached the level
        LCD_DISP.setCursor(i, (row - n));
        if (fract > 0) LCD_DISP.write(fract);          // Fill with fractional cells
        else LCD_DISP.write(16);                       // If fractional is 0, fill with empty
        for (byte k = n + 1; k < height; k++) {   // Fill everything above with empty
          LCD_DISP.setCursor(i, (row - k));
          LCD_DISP.write(16);
        }
        break;
      }
    }
  }
}


void loadClock() {
  LCD_DISP.createChar(0, LT);
  LCD_DISP.createChar(1, UB);
  LCD_DISP.createChar(2, RT);
  LCD_DISP.createChar(3, LL);
  LCD_DISP.createChar(4, LB);
  LCD_DISP.createChar(5, LR);
  LCD_DISP.createChar(6, UMB);
  LCD_DISP.createChar(7, LMB);
}

void loadPlot() {
  LCD_DISP.createChar(0, row8);
  LCD_DISP.createChar(1, row1);
  LCD_DISP.createChar(2, row2);
  LCD_DISP.createChar(3, row3);
  LCD_DISP.createChar(4, row4);
  LCD_DISP.createChar(5, row5);
  LCD_DISP.createChar(6, row6);
  LCD_DISP.createChar(7, row7);
}

#if (LED_TYPE == 0)
byte LED_ON = (LED_MAX_BR);
byte LED_OFF = (LED_MIN_BR);
#else
byte LED_ON = (255 - LED_MAX_BR);
byte LED_OFF = (255 - LED_MIN_BR);
#endif

void setLED(byte color) {
  // at first turn everything off 
  if (!LED_TYPE) {
    analogWrite(LED_RED_PIN, 0);
    analogWrite(LED_GREEN_PIN, 0);
    analogWrite(LED_BLUE_PIN, 0);
  } else {
    analogWrite(LED_RED_PIN, 255);
    analogWrite(LED_GREEN_PIN, 255);
    analogWrite(LED_BLUE_PIN, 255);
  }
  switch (color) {    // 0 off, 1 red, 2 green, 3 blue 
    case 0:
      break;
    case 1: analogWrite(LED_RED_PIN, LED_ON);
      break;
    case 2: analogWrite(LED_GREEN_PIN, LED_ON);
      break;
    case 3: analogWrite(LED_BLUE_PIN, LED_ON);
      break;
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(DISP_LIGHT, OUTPUT);
  pinMode(LED_COMMON_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_BLUE_PIN, OUTPUT);
  setLED(0);

  digitalWrite(LED_COMMON_PIN, LED_TYPE);
  analogWrite(DISP_LIGHT, LCD_MAX_BR);

  LCD_DISP.init();
  LCD_DISP.backlight();
  LCD_DISP.clear();

#if (true)
  boolean status = true;

  setLED(1);


  LCD_DISP.setCursor(0, 0);
  LCD_DISP.print(F("MHZ-19... "));
  Serial.print(F("MHZ-19... "));
  MHZ_SENS.begin(TX_MHZ, RX_MHZ);
  MHZ_SENS.setAutoCalibration(false);
  MHZ_SENS.getStatus();    // asking for the first time, response always - 1
  delay(500);
  if (MHZ_SENS.getStatus() == 0) {
    LCD_DISP.print(F("OK"));
    Serial.println(F("OK"));
  } else {
    LCD_DISP.print(F("ERROR"));
    Serial.println(F("ERROR"));
    status = false;
  }


  setLED(2);
  LCD_DISP.setCursor(0, 1);
  LCD_DISP.print(F("RTC... "));
  Serial.print(F("RTC... "));
  delay(50);
  if (rtc.begin()) {
    LCD_DISP.print(F("OK"));
    Serial.println(F("OK"));
  } else {
    LCD_DISP.print(F("ERROR"));
    Serial.println(F("ERROR"));
    status = false;
  }

  setLED(3);
  LCD_DISP.setCursor(0, 2);
  LCD_DISP.print(F("BME280... "));
  Serial.print(F("BME280... "));
  delay(50);
  if (BME_SENS.begin(&Wire)) {
    LCD_DISP.print(F("OK"));
    Serial.println(F("OK"));
  } else {
    LCD_DISP.print(F("ERROR"));
    Serial.println(F("ERROR"));
    status = false;
  }

  setLED(0);
  LCD_DISP.setCursor(0, 3);
  if (status) {
    LCD_DISP.print(F("All good"));
    Serial.println(F("All good"));
  } else {
    LCD_DISP.print(F("Check wires!"));
    Serial.println(F("Check wires!"));
  }
  while (1) {
    LCD_DISP.setCursor(14, 1);
    LCD_DISP.print("P:    ");
    LCD_DISP.setCursor(16, 1);
    LCD_DISP.print(analogRead(LIGHT_SENS), 1);
    Serial.println(analogRead(LIGHT_SENS));
    delay(300);
  }
#else


  MHZ_SENS.begin(TX_MHZ, RX_MHZ);
  MHZ_SENS.setAutoCalibration(false);

  rtc.begin();
  BME_SENS.begin(&Wire);
#endif

  BME_SENS.setSampling(Adafruit_BME280::MODE_FORCED,
                  Adafruit_BME280::SAMPLING_X1, // temperature
                  Adafruit_BME280::SAMPLING_X1, // pressure
                  Adafruit_BME280::SAMPLING_X1, // humidity
                  Adafruit_BME280::FILTER_OFF   );

  if (RST_CLK || rtc.lostPower())
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  RealTime = rtc.now();
  secs = RealTime.second();
  mins = RealTime.minute();
  hrs = RealTime.hour();

  BME_SENS.takeForcedMeasurement();
  uint32_t Pressure = BME_SENS.readPressure();
  for (byte i = 0; i < 6; i++) {   // counter from 0 to 5
    pressure_array[i] = Pressure;  // fill with current pressure 
    time_array[i] = i;             // fill with numbers 0 - 5
  }

  if (true
) {
    loadClock();
    drawClock(hrs, mins, 0, 0, 1);
    drawData();
  }
  readSensors();
  drawSensors();
}

void loop() {
  if (brightTimer.isReady()) checkBrightness(); // Check brightness
  if (sensorsTimer.isReady()) readSensors();    // Read sensor readings periodically

#if (true)
  if (clockTimer.isReady()) clockTick();        // Recalculate time twice a second and blink dots
  plotSensorsTick();                            // Here are several timers for recalculating graphs (hourly, daily, and forecast)
  modesTick();                                  // Here we catch button presses and switch modes
  if (mode == 0) {                                  // In the "main screen" mode
    if (drawSensorsTimer.isReady()) drawSensors();  // Update sensor readings on the display periodically
  } else {                                          // In any of the graph modes
    if (plotTimer.isReady()) redrawPlot();          // Redraw the graph
  }
#else
  if (drawSensorsTimer.isReady()) drawSensors();
#endif
}

void checkBrightness() {
  if (analogRead(LIGHT_SENS) < BRIGHT_LIMIT) {   // if dark
    analogWrite(DISP_LIGHT, LCD_MIN_BR);
#if (LED_TYPE == 0)
    LED_ON = (LED_MIN_BR);
#else
    LED_ON = (255 - LED_MIN_BR);
#endif
  } else {                                      // if not dark
    analogWrite(DISP_LIGHT, LCD_MAX_BR);
#if (LED_TYPE == 0)
    LED_ON = (LED_MAX_BR);
#else
    LED_ON = (255 - LED_MAX_BR);
#endif
  }
  if (dispCO2 < 800) setLED(2);
  else if (dispCO2 < 1200) setLED(3);
  else if (dispCO2 >= 1200) setLED(1);
}

void modesTick() {
  button.tick();
  boolean changeFlag = false;
  if (button.isClick()) {
    mode++;

#if (true)
    if (mode > 8) mode = 0;
#else
    if (mode > 6) mode = 0;
#endif
    changeFlag = true;
  }
  if (button.isHolded()) {
    mode = 0;
    changeFlag = true;
  }

  if (changeFlag) {
    if (mode == 0) {
      LCD_DISP.clear();
      loadClock();
      drawClock(hrs, mins, 0, 0, 1);
      drawData();
      drawSensors();
    } else {
      LCD_DISP.clear();
      loadPlot();
      redrawPlot();
    }
  }
}

void redrawPlot() {
  LCD_DISP.clear();
  switch (mode) {
    case 1: drawPlot(0, 3, 15, 4, MIN_TEMP, MAX_TEMP, (int*)tempHour, "t hr");
      break;
    case 2: drawPlot(0, 3, 15, 4, MIN_TEMP, MAX_TEMP, (int*)tempDay, "t day");
      break;
    case 3: drawPlot(0, 3, 15, 4, MIN_HUM, MAX_HUM, (int*)humHour, "h hr");
      break;
    case 4: drawPlot(0, 3, 15, 4, MIN_HUM, MAX_HUM, (int*)humDay, "h day");
      break;
    case 5: drawPlot(0, 3, 15, 4, MIN_PRESS, MAX_PRESS, (int*)pressHour, "p hr");
      break;
    case 6: drawPlot(0, 3, 15, 4, MIN_PRESS, MAX_PRESS, (int*)pressDay, "p day");
      break;
    case 7: drawPlot(0, 3, 15, 4, MIN_CO2, MAX_CO2, (int*)co2Hour, "c hr");
      break;
    case 8: drawPlot(0, 3, 15, 4, MIN_CO2, MAX_CO2, (int*)co2Day, "c day");
      break;
  }
}

void readSensors() {
  BME_SENS.takeForcedMeasurement();
  dispTemp = BME_SENS.readTemperature();
  dispHum = BME_SENS.readHumidity();
  dispPres = (float)BME_SENS.readPressure() * 0.00750062;
  dispCO2 = MHZ_SENS.getPPM();

  if (dispCO2 < 800) setLED(2);
  else if (dispCO2 < 1200) setLED(3);
  else if (dispCO2 >= 1200) setLED(1);

}

void drawSensors() {
  LCD_DISP.setCursor(0, 2);
  LCD_DISP.print(String(dispTemp, 1));
  LCD_DISP.write(223);
  LCD_DISP.setCursor(6, 2);
  LCD_DISP.print(" " + String(dispHum) + "%  ");

  LCD_DISP.print(String(dispCO2) + " ppm");
  if (dispCO2 < 1000) LCD_DISP.print(" ");

  LCD_DISP.setCursor(0, 3);
  LCD_DISP.print(String(dispPres) + " mm  rain ");
  LCD_DISP.print(F("       "));
  LCD_DISP.setCursor(13, 3);
  LCD_DISP.print(String(dispRain) + "%");  
}

void plotSensorsTick() {
  // 4 min timer
  if (hourPlotTimer.isReady()) {
    for (byte i = 0; i < 14; i++) {
      tempHour[i] = tempHour[i + 1];
      humHour[i] = humHour[i + 1];
      pressHour[i] = pressHour[i + 1];
      co2Hour[i] = co2Hour[i + 1];
    }
    tempHour[14] = dispTemp;
    humHour[14] = dispHum;
    co2Hour[14] = dispCO2;

    pressHour[14] = dispRain;
    
  }

  // 1.5 hour timer 
  if (dayPlotTimer.isReady()) {
    long averTemp = 0, averHum = 0, averPress = 0, averCO2 = 0;

    for (byte i = 0; i < 15; i++) {
      averTemp += tempHour[i];
      averHum += humHour[i];
      averPress += pressHour[i];
      averCO2 += co2Hour[i];
    }
    averTemp /= 15;
    averHum /= 15;
    averPress /= 15;
    averCO2 /= 15;

    for (byte i = 0; i < 14; i++) {
      tempDay[i] = tempDay[i + 1];
      humDay[i] = humDay[i + 1];
      pressDay[i] = pressDay[i + 1];
      co2Day[i] = co2Day[i + 1];
    }
    tempDay[14] = averTemp;
    humDay[14] = averHum;
    pressDay[14] = averPress;
    co2Day[14] = averCO2;
  }

  // 10 min timer 
 if (predictTimer.isReady()) {
    // Here we perform linear approximation for weather prediction
    long averPress = 0;
    for (byte i = 0; i < 10; i++) {
      BME_SENS.takeForcedMeasurement();
      averPress += BME_SENS.readPressure();
      delay(1);
    }
    averPress /= 10;

    for (byte i = 0; i < 5; i++) {                   // Counter from 0 to 5 (yes, up to 5, because 4 is less than 5)
      pressure_array[i] = pressure_array[i + 1];     // Shift the pressure array back one step, except for the last element
    }
    pressure_array[5] = averPress;                    // The last element of the array is now the new pressure
    sumX = 0;
    sumY = 0;
    sumX2 = 0;
    sumXY = 0;
    for (int i = 0; i < 6; i++) {                    // For all elements of the array
      sumX += time_array[i];
      sumY += (long)pressure_array[i];
      sumX2 += time_array[i] * time_array[i];
      sumXY += (long)time_array[i] * pressure_array[i];
    }
    a = 0;
    a = (long)6 * sumXY;             // Calculate the slope coefficient of the line
    a = a - (long)sumX * sumY;
    a = (float)a / (6 * sumX2 - sumX * sumX);
    delta = a * 6;      // Calculate the change in pressure
    dispRain = map(delta, -250, 250, 100, -100);  // Convert to percentage
  }
}

boolean dotFlag;
void clockTick() {
  dotFlag = !dotFlag;
  if (dotFlag) {          // Recalculate time every second
    secs++;
    if (secs > 59) {      // Every minute
      secs = 0;
      mins++;
      if (mins <= 59 && mode == 0) drawClock(hrs, mins, 0, 0, 1);
    }
    if (mins > 59) {      // Every hour
      RealTime = rtc.now();
      secs = RealTime.second();
      mins = RealTime.minute();
      hrs = RealTime.hour();
      if (mode == 0) drawClock(hrs, mins, 0, 0, 1);
      if (hrs > 23) {
        hrs = 0;
      }
      if (mode == 0 ) drawData();
    }

    
      LCD_DISP.setCursor(16, 1);
      if (secs < 10) LCD_DISP.print(" ");
      LCD_DISP.print(secs);
    
  }
  if (mode == 0) drawdots(7, 0, dotFlag);
  if (dispCO2 >= 1200) {
    if (dotFlag) setLED(1);
    else setLED(0);
  }
}

