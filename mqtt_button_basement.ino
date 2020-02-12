#include <ESP8266WiFi.h>
#include <PubSubClient.h>

int LIGHT_btn = D3;

boolean short_press_Active = false;
boolean long_Press_Active = false;

long short_press_Timer = 0;
long long_press_Timer = 250;

const char* ssid = "SSID";
const char* password = "SSID_PASSWORD";
const char* mqtt_server = "MQTT_SERVER_IP";
const char* short_topic = "TOPIC_1";
const char* long_topic = "TOPIC_2";
const char* payload = "toggle";

// If your MQTT Broker requires a username and password, uncomment and fill in the section below
// const char* mqttUser = "MqttUser";
// const char* mqttPassword = "MqttPassword";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;

void setup_wifi() {
   delay(100);
  // We start by connecting to a WiFi network
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
    }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* short_topic, byte* payload, unsigned int length) 
{
  
} //end callback

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //if you MQTT broker has clientID,username and password
    //please change following line to    if (client.connect(clientId.c_str,MqttUser,MqttPassword))
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
     //once connected to MQTT broker, subscribe command if any
     client.subscribe(short_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
} //end reconnect()

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(LIGHT_btn,INPUT);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  int status; 
//  int vol_up;
//  int vol_down;
    if (now - lastMsg > 50) {
     lastMsg = now;
     status=digitalRead(LIGHT_btn);
     if(status==LOW )
     {
      if (short_press_Active == false) {

      short_press_Active = true;
      short_press_Timer = millis();

    }

    if ((millis() - short_press_Timer > long_press_Timer) && (long_Press_Active == false)) {

      long_Press_Active = true;

      //publish data to MQTT broker
      client.publish(long_topic, payload);
      Serial.println("Long Press");
      delay(1000);

    }

    } else {

    if (short_press_Active == true) {

      if (long_Press_Active == true) {

        long_Press_Active = false;

      } else {


      //publish data to MQTT broker
      client.publish(short_topic, payload);
      Serial.println("Short Press");
      delay(1000);

      }

      short_press_Active = false;

      }
    
     }
    }  
}
