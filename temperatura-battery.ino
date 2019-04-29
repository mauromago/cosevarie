/*
Temperatura con DS18B20 e wimos. protocollo mqtt
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//#include <OneWire.h>
#include <DallasTemperature.h>
//#include <Streaming.h>

#define SLEEP_DELAY_IN_SECONDS  10
#define ONE_WIRE_BUS            D4      // DS18B20 pin

const char* ssid = "SSID";
const char* password = "PSW";

const char* mqtt_server = "192.168.1.131";
const char* mqtt_username = "";
const char* mqtt_password = "";
const char* mqtt_topic = "tempsud";
const char* mqtt_topic2 = "battery";

IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your network
IPAddress ip(192, 168, 1, 212); // controllare che non sia doppio
IPAddress gateAway(192, 168, 1, 254);
IPAddress DNS(8, 8, 8, 8);

WiFiClient espClient;
PubSubClient client(espClient);

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

char temperatureString[6];
char valbat[4];

void setup() {
  // setup serial port
  Serial.begin(115200);

  // setup WiFi
  //setup_wifi();
  //client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);

  // setup OneWire bus
  DS18B20.begin();
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

float getTemperature() {
  //Serial << "Requesting DS18B20 temperature..." << endl;
  float temp;
  do {
    DS18B20.requestTemperatures(); 
    temp = DS18B20.getTempCByIndex(0);
    delay(100);
  } while (temp == 85.0 || temp == (-127.0));
  return temp;
}

void loop() {

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float temperature = getTemperature();
  // convert temperature to a string with two digits before the comma and 2 digits for precision
  itoa(temperature, temperatureString, 2);
  // send temperature to the serial console
  //Serial << "Sending temperature: " << temperatureString << endl;
  // send temperature to the MQTT topic
  client.publish(mqtt_topic, temperatureString);
  //itoa(analogRead(A0),valbat,10);
  //client.publish(mqtt_topic2, valbat);
  //Serial << "Closing MQTT connection..." << endl;
  client.disconnect();

  //Serial << "Closing WiFi connection..." << endl;
  WiFi.disconnect();

  delay(100);

  //Serial << "Entering deep sleep mode for " << SLEEP_DELAY_IN_SECONDS << " seconds..." << endl;
  ESP.deepSleep(SLEEP_DELAY_IN_SECONDS * 1000000); //, WAKE_RF_DEFAULT);
  //ESP.deepSleep(1000 * 1000, WAKE_NO_RFCAL);
  delay(500);   // wait for deep sleep to happen */
}
