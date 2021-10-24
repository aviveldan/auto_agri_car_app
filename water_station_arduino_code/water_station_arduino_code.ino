#include <SoftwareSerial.h>

SoftwareSerial ArduinoUno(0,1);

// Watering stuff
#define PUMP_PIN A0
#define WATER_DELAY_TIME 45000

float need_water = 0;

void setup() {
  delay(50);
  Serial.begin(9600);
  ArduinoUno.begin(9600);
  digitalWrite(PUMP_PIN, HIGH); // disablePump();
  pinMode(PUMP_PIN, OUTPUT);

  delay(2000);
}

void loop() 
{
  if(ArduinoUno.available()>0)
  {
    String s = ArduinoUno.readString();
    delay(50);
    if(s == "refill")
    {
      activatePump();
      delay(WATER_DELAY_TIME);
      disablePump();
      need_water = 0;
      delay(5000);
    }
  }
  delay(2000);
}

void sendFireBase(float need_water)
{
  Serial.print(need_water);
  delay(50);
  Serial.println("\n");
  delay(50);
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
