#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <FirebaseArduino.h>

#define WIFI_SSID "AmitZr"
#define WIFI_PASSWORD "tpmg5834"
#define FIREBASE_HOST "car-project-49490-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "ieGIVK3SFXpOGp3OYNsZyNPdZg1YAcjvQTFsWIqB"

SoftwareSerial NodeMCU(TX,RX);

int num = 0;

String currentServerInput = "";

String outputForArduino = "";

const char pathDelimiter[2] = "*";
const char actionDelimiter[2] = " ";

void setup()
{
  Serial.begin(9600);
  NodeMCU.begin(9600);
  
  wifiConnect();
   Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  if(Firebase.failed()){
    Serial.println(Firebase.error());
  }
  delay(10);
 // Serial.println("Setup completed");
  
}

void loop()
{  
  num++;
  delay(2000);
 // Serial.println("Wifi Loop Refresh");
  delay(500);
  Firebase.setFloat("number", num); //counter for sanity check
  delay(500);
  String path = Firebase.getString("Path");
  if(path == currentServerInput) return;
  currentServerInput = path;

  
  parseAndSendCommands(path);
  
  
  Firebase.setString("Rec", path);
  delay(500);
 
  
  if(WiFi.status() != WL_CONNECTED)
  {
    wifiConnect();
  }
  delay(10);
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
  
  //testing purposes whileeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee
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
  delay(50);
  Serial.println("\n");
  delay(50);
  Serial.print(famount);
  delay(50);
  Serial.println("\n");
  delay(50);
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
