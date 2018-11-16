

#include <ESP8266WiFi.h>
#include "PubSubClient.h"
#include "Bounce2.h"
#include <EEPROM.h>
#include <Wire.h>
#include "TSL2561.h"
#include "Adafruit_MLX90614.h"



TSL2561 tsl(TSL2561_ADDR_FLOAT); 
Adafruit_MLX90614 mlx = Adafruit_MLX90614();


const char* ssid = "iPax-WiFi";
const char* password =  "Abcd@1234#";
const char* mqttServer = "192.168.201.95";
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


bool relayState = LOW;


Bounce debouncer = Bounce(); 


void setup_wifi() {

  delay(10);
  
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
   // digitalWrite(relay_pin, LOW);   // Turn the LED on (Note that LOW is the voltage level
    Serial.println("relay_pin -> LOW");
    relayState = LOW;
    EEPROM.write(0, relayState);    // Write state to EEPROM
    EEPROM.commit();
  } else if ((char)payload[0] == '1') {
    //digitalWrite(relay_pin, HIGH);  // Turn the LED off by making the voltage HIGH
    Serial.println("relay_pin -> HIGH");
    relayState = HIGH;
    EEPROM.write(0, relayState);    // Write state to EEPROM
    EEPROM.commit();
  } else if ((char)payload[0] == '2') {
    relayState = !relayState;
    //digitalWrite(relay_pin, relayState);  // Turn the LED off by making the voltage HIGH
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
      client.publish(outTopic, "hello world");
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
  //client.publish(outTopic, full);
  debouncer.update();
  
   
   // Call code if Bounce fell (transition from HIGH to LOW) :
   if ( debouncer.fell() ) {
     Serial.println("Debouncer fell");
     Serial.println(relayState);
     // Toggle relay state :
     relayState = !relayState;
     //digitalWrite(relay_pin,relayState);
     EEPROM.write(0, relayState);    // Write state to EEPROM
     if (relayState == 1){
      //client.publish(outTopic, ir);
     }
     else if (relayState == 0){
      //client.publish(outTopic, ir);
     }
   }
}

void setup() {

  Serial.begin(115200);
  /////////////////////////////////////////////////////////////////////////////////////////
  Serial.println("Adafruit MLX90614 test");
  mlx.begin(); 
  ////////////////////////////////////////////////////////////////////////////////////////////
  if (tsl.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No sensor?");
    while (1);
  }
    
 
  tsl.setGain(TSL2561_GAIN_16X);      // set 16x gain (for dim situations)
  
  
  tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);  // shortest integration time (bright light)









  //-------------------------------------------------------------------------------------------------------
  EEPROM.begin(512);              // Begin eeprom to store on/off state
  //pinMode(relay_pin, OUTPUT);     // Initialize the relay pin as an output
  //pinMode(button_pin, INPUT);     // Initialize the relay pin as an output
 // pinMode(D1, OUTPUT);
  relayState = EEPROM.read(0);
//  digitalWrite(relay_pin,relayState);
  //Serial.println(relayState);
  //Serial.println("--------------------------------------------------------------------------------------------------------------------");
  
  
  debouncer.attach(D2);   // Use the bounce2 library to debounce the built in button
  debouncer.interval(50);         // Input must be low for 50 ms
  
 
  
  Serial.begin(115200);
  setup_wifi();                   // Connect to wifi 
  client.setServer(mqttServer, 1883);
  client.setCallback(callback);
}

void loop(void) {
  uint16_t x = tsl.getLuminosity(TSL2561_VISIBLE);     
  
  Serial.println(x, DEC);

  Serial.print("*C\tObject = "); Serial.print(mlx.readObjectTempC()); Serial.println("*C");

  
  uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir, full;
  ir = lum >> 16;
  full = lum & 0xFFFF;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  int i = ir;
char buf[4];
itoa (i, buf, 10);

int j = full - ir;
char buf1[4];
itoa (j, buf1, 10);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int k = mlx.readObjectTempC();
char buf2[4];
itoa (k, buf2, 10);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   client.publish(outTopic,buf);
   client.publish("full",buf1);

   ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  client.publish("temp",buf2);


   
   delay(2000);
  
   
   

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  //extButton();
}
