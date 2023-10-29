/*
  TTP223B-Capacitive-Touch-Switch-Module
*/
const int SENSOR_PIN = 2;
// Declare a constant integer SENSOR_PIN and set it to pin 2. This is the pin connected to the touch sensor.

void setup() {
  Serial.begin(9600);
  // Initialize serial communication at a baud rate of 9600 for debugging output.
  pinMode(SENSOR_PIN, INPUT);
  // Set the SENSOR_PIN as an input to read the touch sensor.
}

void loop() {
  if (digitalRead(SENSOR_PIN) == HIGH) {
    // Check if the digital signal read from SENSOR_PIN is HIGH, which indicates the sensor is touched.
    Serial.println("Sensor is touched");
    // Print a message indicating that the sensor is touched.
    while (digitalRead(SENSOR_PIN) == HIGH) {}
    // Wait in a loop until the sensor is no longer touched (the signal becomes LOW).
  }
}
// The main loop of the program continuously checks the status of the touch sensor and prints a message when it's touched.
