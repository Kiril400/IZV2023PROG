// The code demonstrates the usage of the "GyverTimer" library, specifically the GTimer_ms class, to create a timer that triggers an action at regular intervals.
// This code sets up a millisecond-based timer (myTimer) with a 500-millisecond interval. 
// In the loop function, it continuously checks if the timer has reached its interval and prints "Timer!" to the serial monitor when it does. 
// This demonstrates how to use a timer to trigger actions at specific time intervals.
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
