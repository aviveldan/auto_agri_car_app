#include <ESP8266WiFi.h>
//#include <FirebaseArduino.h>

#define WIFI_SSID "AmitZr"
#define WIFI_PASSWORD "tpmg5834"
#define FIREBASE_HOST "car-project-49490-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "ieGIVK3SFXpOGp3OYNsZyNPdZg1YAcjvQTFsWIqB"

#define READY_STATUS_STRING "ready"

String outputForArduino = "";
const char actionDelimiter[2] = " ";
String Status = "shlukkkkk";

char* token;
char* action;
char* amount;
float faction;
float famount;
String path;

void setup()
{
  delay(1000);
  Serial.begin(9600);
  wifiConnect();
  delay(1000);
  fireBaseConnect();
  delay(50);
  char* cstr[path.length() + 1];
  strcpy(cstr, path.c_str());

  action = strtok(cstr, actionDelimiter);
  amount = strtok(NULL, actionDelimiter);
  faction = atof(action);
  famount = atof(amount);
}


void loop()
{  
// Can be refill or ack
  if(action != NULL  && amount != NULL)
  {
    sendActionToArduino(faction, famount);
    while(Serial.available() <= 0)
    {
      delay(500);
    }
    float f = Serial.parseFloat();
    if(f == 1)
    {
      gotoWaterStation();
      Firebase.setBool("NeedRefill", true);
      sendActionToArduino(5, 0);
      goBackToPlace();
    }
    action = strtok(NULL, actionDelimiter);
    if(action == NULL) return;
    amount = strtok(NULL, actionDelimiter);
    if(amount == NULL) return;
    faction = atof(action);
    famount = atof(amount);
    delay(50);
  }
  delay(50); 
}


void gotoWaterStation()
{
//  TODO  
}

void goBackToPlace()
{
//   TODO
}

void sendActionToArduino(float faction, float famount){
  Serial.print(faction);
  delay(10);
  Serial.println("\n");
  delay(10);
  Serial.print(famount);
  delay(10);
  Serial.println("\n");
  delay(10);
}

void wifiConnect()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);             // Connect to the network
  while (WiFi.status() != WL_CONNECTED)
  {                                       // Wait for the Wi-Fi to connect
    delay(1000);
  }
}

void fireBaseConnect()
{
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  if (Firebase.failed()) {
     Serial.println(Firebase.error());  
     return;
 }
  delay(1000);
  Firebase.setString("Status", "idle");
  Firebase.setString("WifiAck", "Setup finished");

  Status = Firebase.getString("Status");
  while(Status != READY_STATUS_STRING)
  {
    delay(2000);
    Status = Firebase.getString("Status");
    delay(50);
  }
  
  Firebase.setString("Status", "idle");
  delay(50);
  path = Firebase.getString("Path");
  Firebase.setString("WifiAck", path);
}
