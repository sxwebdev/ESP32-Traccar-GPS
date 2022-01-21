#include <HTTPClient.h>
#include <TinyGPS++.h>
#include <WiFi.h>

// GPS Module RX pin to ESP32 2
// GPS Module TX pin to ESP32 4
#define RXD2 4
#define TXD2 2

// Change your device id
String DEVICENUM = "12345";

double CurrentLati, CurrentLogi;
TinyGPSPlus gps;
void writeString(char add, String data);
String read_String(char add);
////EEPROM 2
int addr = 0;
#define EEPROM_SIZE 1
int DeviceState = 0;
// INTER NET USEFULL
int testnet = 1;

const char *ssid = "WIFI SSID";         // change this for your own network
const char *password = "WIFI PASSWORD"; // change this for your own network

String FINALLATI = "0", FINALLOGI = "0", FINALSPEED = "0";
int refreshtime;
void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Wifi_connect();
}

void Wifi_connect() {

  delay(500);
  Serial.println("Connecting To  Wifi");
  WiFi.begin(ssid, password);

  delay(5000);
}

void loop() {

  while (Serial2.available()) {
    gps.encode(Serial2.read());
    if (gps.location.isUpdated()) {
      CurrentLati = gps.location.lat();
      CurrentLogi = gps.location.lng();
      String SerialData = "";
      SerialData = String(CurrentLati, 6);
      String SerialData1 = "";
      SerialData1 = String(CurrentLogi, 6);
      FINALLATI = SerialData;
      FINALLOGI = SerialData1;
      FINALSPEED = gps.speed.kmph();
      Serial.println("gps ok");
      Serial.println(FINALLATI);
      Serial.println(FINALLOGI);

      if ((WiFi.status() ==
           WL_CONNECTED)) { // Check the current connection status

        Serial.println("WIFI OK");
        HTTPClient http;
        http.begin("http://traccarserverip:5055/?id=" + DEVICENUM +
                   "&lat=" + FINALLATI + "&lon=" + FINALLOGI +
                   "&speed=" + FINALSPEED + "");
        int httpCode = http.GET();

        if (httpCode == 200) { // Check for the returning code

          Serial.println("DATA SENT TO THE SERVER");
          delay(3000);
        }

        else {

          Serial.println("Error on HTTP request");
        }

        http.end(); // Free the resources
      } else {
        // connect WIFI AGAIN
        Serial.println("NO WIFI :( ");

        Wifi_connect();
      }
    }
  }
  if ((WiFi.status() == WL_CONNECTED)) {
    Serial.println("WIFI OK");
    delay(3000);
  } else {
    Wifi_connect();
  }
}
