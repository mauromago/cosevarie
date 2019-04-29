//------------------------------------------
//INCLUDE
//------------------------------------------
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
//------------------------------------------
//DEFINE
//------------------------------------------
#define pinR D1
#define DEBUG
//------------------------------------------
//Variable declaration
//------------------------------------------
const char* ssid = "SSID";
const char* password = "PSW";
const char* mqtt_topic2 = "debug";
const char* mqtt_topic = "fari";
//char* topicPublish = "fariS";
const char* server = "192.168.1.131";
char message_buff[100];
//------------------------------------------
//Istances
//------------------------------------------
WiFiClient wifiClient;
//PubSubClient client(server, 1883, callback, wifiClient);
PubSubClient client(wifiClient);
//------------------------------------------
void callback(char* mqtt_topic, byte* payload, unsigned int length) {
  int i = 0;
  #ifdef DEBUG
  Serial.println("Message arrived:  topic: " + String(mqtt_topic));
  Serial.println("Length: " + String(length,DEC));
  #endif
  // create character buffer with ending null terminator (string)
  for(i=0; i<length; i++) {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';
  String msgString = String(message_buff);
  #ifdef DEBUG
  Serial.println("Payload: " + msgString);
  #endif
  if(msgString == "dev1-on"){
   #ifdef DEBUG
   Serial.println("Device 1 on");
   #endif
    digitalWrite(pinR, 1);
  } else if(msgString == "dev1-off"){
    #ifdef DEBUG
    Serial.println("Device 1 off");
    #endif
    digitalWrite(pinR, 0);    
  } 
}
//------------------------------------------
String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}
//------------------------------------------
void setup() {
  pinMode(pinR, OUTPUT);
  //pinMode(2, OUTPUT);
  #ifdef DEBUG
  Serial.begin(115200);
  #endif
  delay(10);
  #ifdef DEBUG
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  #endif
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    #ifdef DEBUG
    Serial.print(".");
    #endif
  }
  #ifdef DEBUG 
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  #endif
  client.setServer(server, 1883);
  client.setCallback(callback);
  // Generate client name based on MAC address and last 8 bits of microsecond counter
  String clientName;
  clientName += "esp8266-";
  uint8_t mac[6];
  WiFi.macAddress(mac);
  clientName += macToStr(mac);
  clientName += "-";
  clientName += String(micros() & 0xff, 16);
  #ifdef DEBUG
  Serial.print("Connecting to ");
  Serial.print(server);
  Serial.print(" as ");
  Serial.println(clientName);
  #endif
  if (client.connect((char*) clientName.c_str())) {
    #ifdef DEBUG
    Serial.println("Connected to MQTT broker");
    Serial.print("Topic is: ");
    Serial.println(mqtt_topic);
    #endif
    if (client.subscribe(mqtt_topic)){
      #ifdef DEBUG
      Serial.println("Successfully subscribed");
      #endif
    }
  }
  else {
    #ifdef DEBUG 
    Serial.println("MQTT connect failed");
    Serial.println("Will reset and try again...");
    #endif
    abort();
  }
  client.publish(mqtt_topic2, "reset relay fari");
}
//------------------------------------------
void loop() {
  client.loop();
  delay(0);
}
