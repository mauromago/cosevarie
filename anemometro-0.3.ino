//Anemometro 0.2
//---------------------------------
//INCLUDE
//---------------------------------
//#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//---------------------------------
//variables
//---------------------------------
const char* ssid = "SSID";
const char* password = "PSW";
const char* mqtt_server = "192.168.1.131";
const char* mqtt_username = "";
const char* mqtt_password = "";
const char* mqtt_topic = "vento";
const char* mqtt_topic2 = "debug";
byte sensorInterrupt = D4;  // 0 = digital pin 2
byte sensorPin       = D4; //collegare al pin D3
volatile unsigned long pulseCount;
unsigned long oldTime = 0;
long timeInt = 10000;
char countStr[6];
IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your network
IPAddress ip(192, 168, 1, 217); // controllare che non sia doppio
IPAddress gateAway(192, 168, 1, 254);
IPAddress DNS(8, 8, 8, 8);

//---------------------------------
//ISTANCES
//---------------------------------
WiFiClient espClient;
PubSubClient client(espClient);

//---------------------------------
void pulseCounter()
{
   pulseCount++;
}

//---------------------------------
void setup_wifi() {
  delay(10);
  WiFi.config(ip, gateAway, subnet);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  }
}

//---------------------------------
void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP8266Client", mqtt_username, mqtt_password)) {
    } else {
    delay(5000);
    }
  }
}

//---------------------------------
void callback(char* topic, byte* payload, unsigned int length) {
}

//---------------------------------
void setup() {
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);
  pulseCount        = 0;
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  setup_wifi();
  reconnect();
  client.publish(mqtt_topic2, "reset-anemometro");
}

//---------------------------------
void loop() {
  if (millis() - oldTime > timeInt) {
  detachInterrupt(sensorInterrupt);
  itoa (pulseCount, countStr, 10);
  if (WiFi.status() != WL_CONNECTED) {
      setup_wifi();
    }
  if (!client.connected()) {
      reconnect(); }
  client.publish(mqtt_topic, countStr);
  pulseCount = 0;
  oldTime = millis();
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  delay(0);
 }
}


