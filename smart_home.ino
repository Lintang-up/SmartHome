// Modul ESP
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

// pin sensor LDR
#define sensorLDR A0

// pin relay
const int relay1 = D1;
const int relay2 = D0;

// Wifi Connection
const char* ssid = "Robotika";
const char* password = "zxcvbnmv";

// Domain Name with full URL Path for HTTP POST Request
const char* serverName = "http://connectis.my.id:3000";

// FOR Decode JSON
char msg[300];

// set variable untuk sensor ldr
int nilaiSensor;

void setup() {
  Serial.begin(9600);

  // set mode relay
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);

  // ESP init
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting ... ");  // Start Printing
  }
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  // set title app
  Serial.println("WEBSOCKET CLIENT");
  delay(2000);
}

void loop() {
  nilaiSensor = analogRead(sensorLDR);

  
   digitalWrite(relay1, LOW);
   digitalWrite(relay2, LOW);

   
  Serial.print("Nilai Sensor : ");
  Serial.println(nilaiSensor);
  int luxValue = mapToLux(nilaiSensor); // Mengonversi nilai analog ke lux
  Serial.print("Nilai LDR: ");
  Serial.print(nilaiSensor);
  Serial.print("\tNilai Lux: ");
  Serial.println(luxValue);

  // post data sensor ldr to server
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);

    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    // Data to send with HTTP POST
    String httpRequestData = "ldr_value=" + String(luxValue);
    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);

    String payload = http.getString();
    Serial.println("");
    Serial.println("Response API : ");
    Serial.println(payload);

    if(payload){
      // decode payload with ArduinoJson 6
      StaticJsonDocument<300> doc;
      deserializeJson(doc, payload);
      JsonObject root = doc.as<JsonObject>();

      Serial.print("Status : ");
      Serial.println(String(root["status"]));
      Serial.print("relay_1 : ");
      Serial.println(String(root["relay_1"]));
      Serial.print("relay_2 : ");
      Serial.println(String(root["relay_2"]));

      if(String(root["status"]) == "success") {
        // check status relay 1
        if (String(root["relay_1"]) == "0") {
          digitalWrite(relay1, HIGH);
        } else {
          digitalWrite(relay1, LOW);
        }

        // check status relay 2
        if (String(root["relay_2"]) == "0") {
          digitalWrite(relay2, HIGH);
        } else {
          digitalWrite(relay2, LOW);
        }
      }

      
    }else{
      Serial.println("No Response From API");
    }

    // Free resources
    http.end();
  }

  // set delay 3 detik 
  delay(3000);
}

float mapToLux(int ldrValue) {
  // Di sini Anda harus melakukan pemetaan nilai analog terbalik ke lux sesuai dengan karakteristik sensor LDR Anda
  // Penting untuk diketahui bahwa pemetaan ini harus didasarkan pada kalibrasi yang akurat
  // Data berikut hanyalah contoh dan tidak mewakili koreksi atau kalibrasi yang tepat.

  // Contoh pemetaan linier sederhana dari nilai analog terbalik ke nilai lux
  float luxMin = 500; // Nilai minimum cahaya dalam satuan lux (misalnya pada cahaya yang paling terang)
  float luxMax = 0;    // Nilai maksimum cahaya dalam satuan lux (misalnya pada cahaya yang paling gelap)
  int analogMin = 0;   // Nilai minimum sensor LDR terbalik (misalnya ketika cahaya paling terang)
  int analogMax = 1023; // Nilai maksimum sensor LDR terbalik (misalnya ketika cahaya paling gelap)

  // Melakukan pemetaan linier dari nilai analog terbalik ke nilai lux
  float luxValue = luxMin + (float)(ldrValue - analogMin) * (luxMax - luxMin) / (analogMax - analogMin);
  
  return luxValue;
}
