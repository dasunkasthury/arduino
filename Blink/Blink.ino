/*

 It connects to an MQTT server then:
  - on 0 switches off relay
  - on 1 switches on relay
  - on 2 switches the state of the relay

  - sends 0 on off relay
  - sends 1 on on relay

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 The current state is stored in EEPROM and restored on bootup

*/
edited
#include <ESP8266WiFi.h>
#include "PubSubClient.h"
#include "Bounce2.h"
#include <EEPROM.h>


const char* ssid = "iPax-WiFi";
const char* password =  "Abcd@1234#";
const char* mqttServer = "192.168.91.141";
const int mqttPort = 1883;
const char* mqttUser = "eyepax";
const char* mqttPassword = "eyepax";


WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

const char* outTopic = "proout";
const char* inTopic = "proin";

int relay_pin = D2;
int button_pin = D6;
bool relayState = LOW;

// Instantiate a Bounce object :
Bounce debouncer = Bounce(); 


void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    extButton();
    for(int i = 0; i<500; i++){
      extButton();
      delay(1);
    }
    Serial.print(".");
  }
  digitalWrite(D1, LOW);
  delay(500);
  digitalWrite(D1, HIGH);
  delay(500);
  digitalWrite(D1, LOW);
  delay(500);
  digitalWrite(D1, HIGH);
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '0') {
    digitalWrite(relay_pin, LOW);   // Turn the LED on (Note that LOW is the voltage level
    Serial.println("relay_pin -> LOW");
    relayState = LOW;
    EEPROM.write(0, relayState);    // Write state to EEPROM
    EEPROM.commit();
  } else if ((char)payload[0] == '1') {
    digitalWrite(relay_pin, HIGH);  // Turn the LED off by making the voltage HIGH
    Serial.println("relay_pin -> HIGH");
    relayState = HIGH;
    EEPROM.write(0, relayState);    // Write state to EEPROM
    EEPROM.commit();
  } else if ((char)payload[0] == '2') {
    relayState = !relayState;
    digitalWrite(relay_pin, relayState);  // Turn the LED off by making the voltage HIGH
    Serial.print("relay_pin -> switched to ");
    Serial.println(relayState); 
    EEPROM.write(0, relayState);    // Write state to EEPROM
    EEPROM.commit();
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client", "eyepax", "eyepax")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, "Sonoff1 booted");
      // ... and resubscribe
      client.subscribe(inTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      for(int i = 0; i<5000; i++){
        extButton();
        delay(1);
      }
    }
  }
}

void extButton() {
  debouncer.update();
   
   // Call code if Bounce fell (transition from HIGH to LOW) :
   if ( debouncer.fell() ) {
     Serial.println("Debouncer fell");
     // Toggle relay state :
     relayState = !relayState;
     digitalWrite(relay_pin,relayState);
     EEPROM.write(0, relayState);    // Write state to EEPROM
     if (relayState == 1){
      client.publish(outTopic, "1");
     }
     else if (relayState == 0){
      client.publish(outTopic, "0");
     }
   }
}

void setup() {
  EEPROM.begin(512);              // Begin eeprom to store on/off state
  pinMode(relay_pin, OUTPUT);     // Initialize the relay pin as an output
  pinMode(button_pin, INPUT);     // Initialize the relay pin as an output
  pinMode(D1, OUTPUT);
  relayState = EEPROM.read(0);
  digitalWrite(relay_pin,relayState);
  
  debouncer.attach(button_pin);   // Use the bounce2 library to debounce the built in button
  debouncer.interval(50);         // Input must be low for 50 ms
  
  digitalWrite(D1, LOW);          // Blink to indicate setup
  delay(500);
  digitalWrite(D1, HIGH);
  delay(500);
  
  Serial.begin(115200);
  setup_wifi();                   // Connect to wifi 
  client.setServer(mqttServer, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  extButton();
}
