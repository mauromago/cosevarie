// EmonLibrary examples openenergymonitor.org, Licence GNU GPL V
//-------------------------------------------
// INCLUDE
//-------------------------------------------
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "EmonLib.h"  // Include Emon Library
//-------------------------------------------
// variables
//-------------------------------------------
#define SLEEP_DELAY_IN_SECONDS  1800
#define Cicli 100
#define Cicli2 10
const char* ssid = "SSID";
const char* password = "PSW";
char strValore[6];
const char* mqtt_server = "192.168.1.131";
const char* mqtt_username = "";
const char* mqtt_password = "";
const char* mqtt_topic = "corrente";
IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your network
IPAddress ip(192, 168, 1, 215); // controllare che non sia doppio
IPAddress gateAway(192, 168, 1, 254);
IPAddress DNS(8, 8, 8, 8);
double Irms = 0;
double IrmsSum = 0;
double IrmsMed = 0;
int Cont = 0;
//-------------------------------------------
// Instances
//-------------------------------------------
WiFiClient espClient;
PubSubClient client(espClient);
EnergyMonitor emon1; // Create an instance

//-------------------------------------------
void setup_wifi() {
  delay(10);
  WiFi.config(ip, gateAway, subnet);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

//-------------------------------------------
void callback(char* topic, byte* payload, unsigned int length) { 
}

//-------------------------------------------
void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP8266Client", mqtt_username, mqtt_password)) { //????
    } else {
    delay(5000);
    }
  }
}

//-------------------------------------------
void setup()
{  
  Serial.begin(115200);
  emon1.current(A0, 11.1);             // Current: input pin, calibration.
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

//-------------------------------------------
void loop()
{
  client.loop();
  for (int i=0;i<Cicli2;i++){
    for (int i=0;i<Cicli;i++){
    Irms = emon1.calcIrms(1480);  // Calculate Irms only
    IrmsSum = IrmsSum + Irms;
    delay(0);
    }  
  IrmsMed = IrmsMed + IrmsSum;
  IrmsSum = 0;
  Serial.println(IrmsMed); // poi togliere
  }  
  Irms = IrmsMed/(Cicli*Cicli2);
  IrmsMed = 0;
  dtostrf(Irms, 2, 2, strValore);
  Serial.print(Irms); // poi togliere
  Serial.print("--"); / poi togliere
  Serial.println(strValore); / poi togliere
  setup_wifi();
  Serial.print("."); / poi togliere
  if (!client.connected()) {
    reconnect(); }
  Serial.print(".");
  client.publish(mqtt_topic, strValore);
  Serial.println(".");
  delay(10);
  client.disconnect();
  WiFi.disconnect();
  delay(100);
}
