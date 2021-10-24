#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <FirebaseArduino.h>

#define WIFI_SSID "AmitZr"
#define WIFI_PASSWORD "tpmg5834"
#define FIREBASE_HOST "car-project-49490-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "ieGIVK3SFXpOGp3OYNsZyNPdZg1YAcjvQTFsWIqB"

#define READY_STATUS_STRING "ready"

String outputForArduino = "";
const char actionDelimiter[2] = " ";
String Status = "shlukkkkk";


void setup()
{
  delay(1000);
  Serial.begin(9600);
  wifiConnect();
  delay(1000);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  if (Firebase.failed()) {
     Serial.println(Firebase.error());  
     return;
 }
  delay(1000);
  Firebase.setString("Status", "idle");
  Firebase.setString("WifiAck", "Setup finished");
}


void loop()
{  
  delay(2000);
  //case arduino send shit to wifi if we need to refill
  if(Serial.available()>0){
    String s = Serial.readString();
    if(s == "refill"){
      gotoWaterStation();
      Firebase.setBool("NeedRefill", true);
      sendActionToArduino(5, 0);
    }
  }
  
  Status = Firebase.getString("Status");
  while(Status != READY_STATUS_STRING){
    delay(2000);
    Status = Firebase.getString("Status");
    delay(50);
  }
  
  Firebase.setString("Status", "idle");
  delay(50);
  String path = Firebase.getString("Path");
  Firebase.setString("WifiAck", path);
  delay(50);
  parseAndSendCommands(path);
  delay(50); 
}


void gotoWaterStation(){
//to implement  
}

void parseAndSendCommands(String path){
  
  char cstr[path.length() + 1];
  strcpy(cstr, path.c_str());
  char* token;
  char* action;
  char* amount;
  float faction;
  float famount;
  
  
  action = strtok(cstr, actionDelimiter);
  amount = strtok(NULL, actionDelimiter);
  faction = atof(action);
  famount = atof(amount);
  
  while( action != NULL  && amount != NULL) {
    sendActionToArduino(faction, famount);
    action = strtok(NULL, actionDelimiter);
    if(action == NULL) return;
    amount = strtok(NULL, actionDelimiter);
    if(amount == NULL) return;
    faction = atof(action);
    famount = atof(amount);
    delay(50);
   }
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
  //Serial.print("Connecting to ");
  //Serial.print(WIFI_SSID); Serial.println(" ...");

  int teller = 0;
  while (WiFi.status() != WL_CONNECTED)
  {                                       // Wait for the Wi-Fi to connect
    delay(1000);
   // Serial.print(++teller); Serial.print(' ');
  }

 // Serial.println('\n');
 // Serial.println("Connection established!");  
 // Serial.print("IP address:\t");
 // Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
}
