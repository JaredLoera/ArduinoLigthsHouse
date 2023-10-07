#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "DHT.h"


/********* WiFi Access Point ***********/

#define WLAN_SSID "RED-JARED"
#define WLAN_PASS "74925926..1a3a"

/********* Adafruit.io Setup ***********/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME "JaredLoera"
#define AIO_KEY      "key"

/**** Global State (you don't need to change this!) ******/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/********** Feeds *************/

// Setup a feed called 'photocell' for publishing.

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe ledbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/iluminacion");

/********* Sketch Code ************/

int distancia;
// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

int pin_dos = 2;
void setup() {
  pinMode(4,OUTPUT);
 pinMode(pin_dos, OUTPUT);
  Serial.begin(115200);
  delay(10);

  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); 
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.print("IP address: "); 
  Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&ledbutton);
}


void loop() {
  MQTT_connect();
  delay(3000);
  led(); 
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
void led()
{
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &ledbutton) {
      Serial.print(F("Got: "));
      Serial.println((char *)ledbutton.lastread);
	  String interruptor = String((char *)ledbutton.lastread);
      if(interruptor == "0"){
        Serial.print("ENCENDIO");
        digitalWrite(pin_dos, HIGH);
        digitalWrite(22,HIGH);
        }
      if(interruptor == "1"){
        Serial.print("SE APAGO");
        digitalWrite(22,LOW);
        digitalWrite(pin_dos, LOW);
        }
    }
  }
}