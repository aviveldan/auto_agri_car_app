// Global Variables and macros

// Watering stuff
#define PUMP_PIN A0
#define WATER_DELAY_TIME 15000

void setup() {
  disablePump();
  pinMode(PUMP_PIN, OUTPUT);
}

void loop() 
{
  delay(2000);
  activatePump();
  delay(WATER_DELAY_TIME);
  disablePump();
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
