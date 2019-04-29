/*
Test for PIR sensor and MQTT
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define SensPin D4
#define SLEEP_DELAY_IN_SECONDS  10

const char* ssid = "SSID";
const char* password = "PSW";

const char* mqtt_server = "192.168.1.131";
const char* mqtt_username = "";
const char* mqtt_password = "";
const char* mqtt_topic = "pir-test";

char sensState[4];
//int sensState;

IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your network
IPAddress ip(192, 168, 1, 222); // controllare che non sia doppio
IPAddress gateAway(192, 168, 1, 254);
IPAddress DNS(8, 8, 8, 8);

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  // setup serial port
  Serial.begin(115200);
  pinMode (SensPin, INPUT);
  //setup WiFi
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.config(ip, gateAway, subnet);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }

  //Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  //Serial.print("Message arrived [");
  //Serial.print(topic);
  //Serial.print("] ");
  //for (int i = 0; i < length; i++) {
    //Serial.print((char)payload[i]);
  //}
  //Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    //Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client", mqtt_username, mqtt_password)) {
     // Serial.println("connected");
    } else {
      //Serial.print("failed, rc=");
      //Serial.print(client.state());
      //Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      //delay(5000);
    }
  }
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  /*if (digitalRead(SensPin))
    {sensState = "on";}
  else
    {sensState = "off";}*/
  itoa(digitalRead(SensPin), sensState, 2);  
  client.publish(mqtt_topic, sensState);
  delay(5000);   // wait for deep sleep to happen */
}
