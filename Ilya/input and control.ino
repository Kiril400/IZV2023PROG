/*
  TTP223B-Capacitive-Touch-Switch-Module
*/
const int SENSOR_PIN = 2;          
void setup() {
  Serial.begin(9600);
  pinMode(SENSOR_PIN, INPUT);
}

void loop() {
  if(digitalRead(SENSOR_PIN) == HIGH){
    Serial.println("Sensor is touched");
    while(digitalRead(SENSOR_PIN) == HIGH){}
  }
}
