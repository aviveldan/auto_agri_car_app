#include <ESP8266WiFi.h>
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
  delay(500);
  Status = Firebase.getBool("NeedRefill");
  if(Status)
  {
    updateStationToRefill();
    Firebase.setBool("NeedRefill", false);
    while(Serial.available() <= 0)
    {
      delay(500);
    }
    Serial.parseFloat(); // Waits for ack
  }
}

void updateStationToRefill(){
  Serial.print(111);
  Serial.println("\n");
}

void wifiConnect()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // Connect to the network

  while (WiFi.status() != WL_CONNECTED)
  {  
    // Wait for the Wi-Fi to connect
    delay(1000);
  }
}
