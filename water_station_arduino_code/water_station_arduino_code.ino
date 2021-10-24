#include <SoftwareSerial.h>

SoftwareSerial ArduinoUno(0,1);

// Watering stuff
#define PUMP_PIN A0
#define WATER_DELAY_TIME 45000

void setup() {
  delay(50);
  Serial.begin(115200);
  ArduinoUno.begin(9600);
  digitalWrite(PUMP_PIN, HIGH); // disablePump();
  pinMode(PUMP_PIN, OUTPUT);

  delay(2000);
}

void loop() 
{
  if(ArduinoUno.available()>0)
  {
    float f = ArduinoUno.parseFloat();
    delay(50);
    if(f == 1)
    {
      activatePump();
      delay(WATER_DELAY_TIME);
      disablePump();
      delay(5000);
    }
  }
  delay(2000);
}


// Pump Functions

void activatePump() {
  delay(50);
  digitalWrite(PUMP_PIN, LOW);
}

void disablePump() {
  delay(50);
  digitalWrite(PUMP_PIN, HIGH);
}
