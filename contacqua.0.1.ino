//Pluvimentro versione 1.0

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define Spin D4 //sensor pin

volatile int check = 0;
const char* ssid = "Esterno";
const char* password = "";
int interruptCounter = 0;
int numberOfInterrupts = 10;
const char* mqtt_server = "192.168.1.131";
const char* mqtt_username = "";
const char* mqtt_password = "";
const char* mqtt_topic = "acqua";
const char* mqtt_topic2 = "debug";
unsigned long oldNow;
unsigned long testTime;
const unsigned long RstTime = 3 * 60 * 1000;
const unsigned long sndTime = 6 * 60 * 1000;
bool stopped = false; 
char strValore[2] = "X";

IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your network
IPAddress ip(*.*.*.*); // controllare che non sia doppio
IPAddress gateAway(*.*.*.*);
IPAddress DNS(8, 8, 8, 8);
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  WiFi.config(ip, gateAway, subnet);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP8266Client", mqtt_username, mqtt_password)) {
    } else {
    delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
}

void Hint () {
  check = 1;
  }

void setup() {
  attachInterrupt(Spin, Hint, FALLING);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  oldNow = millis();
  testTime = millis();
  setup_wifi();
  reconnect();
  client.publish(mqtt_topic2, "reset-acqua");
}

void loop() {
  if (check){
    detachInterrupt(Spin);  
    itoa (interruptCounter, strValore, 10);
    interruptCounter++;
    if (interruptCounter >= numberOfInterrupts) {
      interruptCounter = 0;  }
    if (WiFi.status() != WL_CONNECTED) {
      setup_wifi();
    }
    if (!client.connected()) {
      reconnect(); }
    client.publish(mqtt_topic, strValore);
    client.disconnect();
    delay(100);
    check = 0;
    //oldNow = millis();
    //testTime = millis();
    stopped = false;
    attachInterrupt(Spin, Hint, FALLING);
  }
  /*delay(0);
  if ((millis()-oldNow)> RstTime && !stopped) {
   if (!client.connected()) {
      reconnect(); } 
   client.publish(mqtt_topic2, "disconnet");
   espClient.stop();
   stopped = true;
  }*/
}
