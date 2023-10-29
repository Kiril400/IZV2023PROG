#include "GyverTimer.h"
// Include the GyverTimer library for timer functionality.

GTimer_ms myTimer;
// Create an instance of the GTimer_ms class called "myTimer" for millisecond-based timing.

void setup() {
  Serial.begin(9600);
  // Initialize serial communication at a baud rate of 9600.
  myTimer.setInterval(500); 
  // Set the interval for "myTimer" to 500 milliseconds (0.5 seconds).
}

void loop() {
  if (myTimer.isReady())
  // Check if the timer "myTimer" has reached its set interval.
    Serial.println("Timer!");
  // If the timer is ready, print "Timer!" to the serial monitor.
}
