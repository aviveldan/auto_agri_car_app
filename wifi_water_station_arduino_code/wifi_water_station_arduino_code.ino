#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <FirebaseArduino.h>

#define WIFI_SSID "AmitZr"
#define WIFI_PASSWORD "tpmg5834"
#define FIREBASE_HOST "car-project-49490-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "ieGIVK3SFXpOGp3OYNsZyNPdZg1YAcjvQTFsWIqB"

bool Status = false;

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
}

void loop()
{  
  Status = Firebase.getBool("NeedRefill");
  while(!Status){
    delay(500);
    Status = Firebase.getBool("NeedRefill");
    delay(50);
  }
  updateStationToRefill();
  Firebase.setBool("NeedRefill", false);
}

void updateStationToRefill(){
  Serial.println("refill");
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
