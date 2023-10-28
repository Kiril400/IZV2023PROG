#include "GyverTimer.h"
GTimer_ms myTimer;               // create timer

void setup() {
  Serial.begin(9600);
  myTimer.setInterval(500);   // set interval
}

void loop() {
  if (myTimer.isReady()) Serial.println("Timer!");
}
