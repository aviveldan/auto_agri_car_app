
// Watering stuff
#define PUMP_PIN A0
#define WATER_DELAY_TIME 45000

void setup() {
  delay(50);
  Serial.begin(9600);
  digitalWrite(PUMP_PIN, HIGH); // disablePump();
  pinMode(PUMP_PIN, OUTPUT);

  delay(2000);
}

void loop() 
{
  if(Serial.available()>0)
  {
    float f = Serial.parseFloat();
    delay(50);
    if(f == 111)
    {
      activatePump();
      delay(WATER_DELAY_TIME);
      disablePump();
      delay(5000);
    }
    Serial.print(1);
    Serial.println("\n");
  }
  delay(500);
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
