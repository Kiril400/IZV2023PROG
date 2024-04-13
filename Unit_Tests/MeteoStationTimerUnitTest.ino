#include <AUnit.h>
using namespace aunit;

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <MHZ19_uart.h>
#include <GyverButton.h>
#include <GyverTimer.h>

//////////////////////////////////main code defines//////////////////////////////////////////////

#define RST_CLK 0       // Reset clock to firmware loading time
#define SENSOR_TIME 30000     // Sensor reading update time on the screen, milliseconds
#define LED_TYPE 0          // RGB LED type: 0 - common cathode, 1 - common anode

#define BRIGHT_LIMIT 150
#define LED_MAX_BR 255   
#define LED_MIN_BR 10     
#define LCD_MAX_BR 255    
#define LCD_MIN_BR 10     
#define DISPLAY_I2C_ADDR 0x27   
#define MIN_TEMP 15
#define MAX_TEMP 35
#define MIN_HUM 0
#define MAX_HUM 100
#define MIN_PRESS -100
#define MAX_PRESS 100
#define MIN_CO2 300
#define MAX_CO2 2000

#define DISP_LIGHT 10
#define LIGHT_SENS A3

#define RX_MHZ 2
#define TX_MHZ 3

#define LED_COMMON_PIN 7
#define LED_RED_PIN 9
#define LED_GREEN_PIN 6
#define LED_BLUE_PIN 5
#define SENSOR_BUTTON_PIN 4

#define DISP_BACKLIT_PIN 10     
#define LIGHT_SENS_PIN 0  

int FuncCallCounter = 0;

// Mockable Timer Class
class MockableTimer {
  GTimer_ms timer;
  bool isReadyFlag = false;

public:
  MockableTimer(uint32_t interval) : timer(interval) {}

  void setReady(bool ready) {
    isReadyFlag = ready;
  }

  bool isReady() {
    return isReadyFlag ? true : timer.isReady();
  }
};

// Mock timers
MockableTimer brightTimer(2000);
MockableTimer sensorsTimer(SENSOR_TIME);
MockableTimer clockTimer(500);
MockableTimer drawSensorsTimer(SENSOR_TIME);
MockableTimer plotTimer(240000);
int timerSum =2000+SENSOR_TIME+500+240000;
MockableTimer testTimer(timerSum);

bool brightnessAdjusted = false; 
bool DataRead = false; 
bool DataDrawn = false;
bool ClockTicks = false; 


void checkBrightness() {
//Serial.println("checkBrightness function is triggered");
brightnessAdjusted = true;
}

void readSensors() {
//Serial.println("Sensor data is being read...");
DataRead = true; 
}

void drawSensors() {
//Serial.println("Sensor data is being drawn on display ...");
DataDrawn = false;
}


void clockTick() {
//Serial.println("Signal for clock ticking is being send ...");
ClockTicks = true; 
}

void setup() {
  
  Serial.begin(9600);
  TestRunner::setTimeout(3000);

   Serial.println("Meteo station is working. Timers' Unit test are going to preceed... It might take up to 3 minutes.");
    Serial.println("Please wait.");
        Serial.println("Kind regards, RTU IZV.");
}

void loop() {
 
//Serial.println (FuncCallCounter);

  if (brightTimer.isReady()) {

    checkBrightness();
    FuncCallCounter++; 
    }

      if (sensorsTimer.isReady()){

        readSensors();
        FuncCallCounter++; 
      } 

          if (clockTimer.isReady()){

          clockTick();
              FuncCallCounter++; 
          }

              if (drawSensorsTimer.isReady()) {
                
                drawSensors();
                  FuncCallCounter++; 
              }
                
                  if (testTimer.isReady()){

                      aunit::TestRunner::run();    

                      // Serial.println(brightnessAdjusted);
                      // Serial.println(ClockTicks);
                      // Serial.println(DataDrawn);
                      // Serial.println(DataRead);

                    }
 }

test(BrightnessRefreshRate) {
  
    assertTrue(brightnessAdjusted); 
}

        test(ClockTickingInternal) {
          
            assertTrue(ClockTicks); 
        }

              test(DataRefreshRate) {
                
                  assertTrue(DataDrawn);
              }

                      test(SensorReadInterval) {

                          assertTrue(DataRead); 
                      }


