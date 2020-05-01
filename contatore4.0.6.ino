/*
test 1 per lettura led Conta1tori con esp8266
 */
// **********************
// DEBUG
// **********************
//#define DEBUG
// **********************
// INCLUDE
// **********************

#include <ESP8266WiFi.h>
#include "setmylan.h"
#include <AsyncMqttClient.h>

// *********************
// DEFINE
// *********************

#define Spin1 D4 // pin del sensore
#define Spin2 D1 // pin del sensore
#define Spin3 D2 // pin del sensore
#define Spin4 D5 // pin del sensore
#define Step 10 // numero di impulsi per il conteggio
#define MQTT_id "ESP8266-test"
#define numberOfInterrupts 10

// **********************
// VARIABILI
// **********************

volatile int check[5] = {0,0,0,0};
int checkp = 0;
volatile unsigned long checkTime [5] = {0,0,0,0};
volatile unsigned long lastCheckTime [5] = {0,0,0,0};
unsigned long checkTimep = 0;
unsigned long lastCheckTimep = 0;
int interruptCounter[5] = {0,0,0,0};
int pot[5] = {0,0,0,0};
unsigned long Tempo = 0;
char lett[3] = "0";
char inter[2] = ",";
char inter2[2] = ";";
char strValore[22] = "X";
char strPot[6] = "00000";
char strCheckTime [11] = "0000000000";
int Pot = 0;
unsigned long deTime = 0;
const char* mqtt_topic1 = "prod-mono";
const char* mqtt_topic2 = "prod-tri";
const char* mqtt_topic3 = "cont-mono";
const char* mqtt_topic4 = "cont-tri";
const char* mqtt_topicd = "debug";
IPAddress ip(192, 168, 1, 219); // controllare che non sia doppio

// ***********************
// ISTANZE
// ***********************

WiFiClient espClient;
AsyncMqttClient mqttClient;

// ***********************
// FUNZIONI
// ***********************

void ICACHE_RAM_ATTR Conta1 (){
  check[0]++ ;
  if (check[0] == 1){
  checkTime[0] = millis();
  } else {
  lastCheckTime[0] = millis();
  }
}

void ICACHE_RAM_ATTR Conta2 (){
  check[1]++;
  if (check[1] == 1){
  checkTime[1] = millis();
  } else {
  lastCheckTime[1] = millis();
  }
}

void ICACHE_RAM_ATTR Conta3 (){
  check[2]++;
  if (check[2] == 1){
  checkTime[2] = millis();
  } else {
  lastCheckTime[2] = millis();
  }
}

void ICACHE_RAM_ATTR Conta4 (){
  check[3]++;
  if (check[3] == 1){
  checkTime[3] = millis();
  } else {
  lastCheckTime[3] = millis();
  }
}

void setup_wifi() {
  #ifdef DEBUG
      Serial.println ("connessione wifi");
      #endif
  WiFi.config(ip, gateAway, subnet);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    #ifdef DEBUG
      Serial.println ("in connessione");
      #endif
    delay(1000);
  }
}

void reconnect() {
  #ifdef DEBUG
    Serial.print (",mqtt Connect,");
    Serial.print (millis());
    #endif
    mqttClient.connect();
}

void onMqttConnect(bool sessionPresent) {
  #ifdef DEBUG
    Serial.println("Connected to MQTT.");
    #endif
}

//void callback(char* topic, byte* payload, unsigned int length) {
//}

void controllo (int c, unsigned long tempi, int check, unsigned long adesso) {
    #ifdef DEBUG
      Serial.print (",In controllo,");
      Serial.print (c);
      #endif
    deTime = adesso - tempi;
    #ifdef DEBUG
      Serial.print (",deTime,");
      Serial.print (deTime);
      #endif
    Pot = (int) ((3600000*check)/deTime); // verificare
    #ifdef DEBUG
      Serial.print (",Pot,");
      Serial.print (Pot);
      #endif
    itoa (Pot, strPot, 10);
    itoa (interruptCounter[c], strValore, 10);
    delay(0);
    itoa (check, lett, 10);
    ultoa (tempi, strCheckTime, 10);
    strcat (strValore,inter);
    strcat (strValore,lett);
    strcat (strValore,inter2);
    strcat (strValore,strPot);
    strcat (strValore,inter);
    strcat (strValore,strCheckTime);    
    #ifdef DEBUG
      Serial.print (",stringa,");
      Serial.print (strValore);
      #endif
    interruptCounter[c]++;
    if (interruptCounter[c] >= numberOfInterrupts) {
      interruptCounter[c] = 0;  }
    if (WiFi.status() != WL_CONNECTED) {
      setup_wifi();
    }
    /*if (!client.connected()) {
      reconnect(); }
      else {
      #ifdef DEBUG
      Serial.print (",,,,");
      #endif
      }*/
      #ifdef DEBUG
      Serial.print (",pubb,");
      Serial.print (millis());
      #endif
      switch (c) {
        case 0:
        mqttClient.publish(mqtt_topic1, 0, 0, strValore);
        break;
        case 1:
        mqttClient.publish(mqtt_topic2, 0, 0, strValore);
        break;
        case 2:
        mqttClient.publish(mqtt_topic3, 0, 0, strValore);
        break;
        case 3:
        mqttClient.publish(mqtt_topic4, 0, 0, strValore);
        break;
      }
    //delay(0);
    strValore[0] = '\0';
    #ifdef DEBUG
      Serial.print (",out controllo,");
      #endif
}

void setup() {
  #ifdef DEBUG
    Serial.begin(115200);
    #endif  
  attachInterrupt(digitalPinToInterrupt(Spin1), Conta1, FALLING);
  attachInterrupt(digitalPinToInterrupt(Spin2), Conta2, FALLING);
  attachInterrupt(digitalPinToInterrupt(Spin3), Conta3, FALLING);
  attachInterrupt(digitalPinToInterrupt(Spin4), Conta4, FALLING);
  setup_wifi();
  mqttClient.onConnect(onMqttConnect);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  reconnect();
  mqttClient.publish(mqtt_topicd, 0, 0, "reset-corrente");  
}

void loop() {
   for (int C = 0; C < 4; C++) {
    if (check[C] >= Step){ 
       checkp = check[C];
       checkTimep = checkTime[C];
       lastCheckTimep = lastCheckTime[C];
       check[C] = 0;
       #ifdef DEBUG
        Serial.print (millis());
        #endif
       controllo (C, checkTimep, checkp, lastCheckTimep);
       #ifdef DEBUG
        Serial.println (millis());
        #endif
      }  
    }
  //delay(0);
}
