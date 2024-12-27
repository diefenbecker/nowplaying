#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ESP8266WebServer.h>

//WLAN
const char* ssid = "<wlan_ssid>";
const char* pass = "<wlan_passwort>";
ESP8266WebServer server(80);

// Hardwaretyp des MAX7219
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW

// PINs und Anzahl der MAX-Elemente
#define MAX_DEVICES 8
#define CS_PIN 15

MD_Parola display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// BME280
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme; // I2C

//Schalter um Platte zu erkennen
int pinNowSpinning = D0; 

//Warteschleifen
unsigned long startMillisTemp;
unsigned long currentMillisTemp;
unsigned long startMillisWechsel;
unsigned long currentMillisWechsel;

//Externe Daten deklarieren
String aussentemperatur = "keine Daten";
String akkustand = "keine Daten";
String puffer = "keine Daten";
String pvleistung = "keine Daten";

//Sonstiges
int anzeigennummer=0;

void setup() {

  Serial.begin(115200); 

  display.begin();
  display.setIntensity(0);
  display.displayClear();
  display.setTextAlignment(PA_CENTER);
  
  pinMode(pinNowSpinning, INPUT); 

  bool status;
  status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Kann BME Sensor nicht finden. Verkabelung prüfen!");
    while (1);
  }

  startMillisTemp = millis();
  startMillisWechsel = millis();

  //WLAN
  Serial.print("Verbinde zu: ");
  Serial.println(ssid);
  display.print("Verbinde Wifi");
  WiFi.begin(ssid, pass);
   
  while(WiFi.status() != WL_CONNECTED){
    delay(500); 
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi verbunden");
  Serial.print("IP-Adresse des ESP8266: ");  
  Serial.println(WiFi.localIP());
  display.displayClear();
  display.print(WiFi.localIP());
  delay(3000);

  //Webendpunkte
  server.on("/",handleRoot);
  server.on("/receivedata", receiveData);

  server.begin();
}

void loop() {
  //Client-Anfragen abfragen
  server.handleClient(); 

  if (digitalRead(pinNowSpinning) == HIGH) {
    //Text wenn eine Platte abgestellt ist    
    display.print("NOW SPINNING");
    Serial.println("Now Spinning");  
  } else {
    currentMillisWechsel = millis();    
    if (currentMillisWechsel - startMillisWechsel >= 5000) {
      startMillisWechsel = currentMillisWechsel;
      anzeigennummer++;
      Serial.print("Anzeigenummer: ");
      Serial.println(anzeigennummer);
      switch (anzeigennummer) {
        case 1:
          //Temperatur alle 30 Sekunden abfragen
          currentMillisTemp = millis();        
          if (currentMillisTemp - startMillisTemp >= 10000) {
            startMillisTemp = currentMillisTemp;
            String temperatur = String(bme.readTemperature());
            display.print("> "+temperatur.substring(0,temperatur.length()-1)+" \"C");

            Serial.print("Temperatur: ");
            Serial.print(bme.readTemperature());
            Serial.println(" °C");

            Serial.print("Luftfeuchte: ");
            Serial.print(bme.readHumidity());
            Serial.println(" %");
          }
          break;
        case 2:
          display.print(aussentemperatur);
          Serial.println(aussentemperatur);          
          break;
        case 3:
          display.print(akkustand);
          Serial.println(akkustand);          
          break;
        case 4:
          display.print(pvleistung);
          Serial.println(pvleistung);          
          break;
        case 5:
          display.print(puffer);
          Serial.println(puffer);          
          break;
        default:
          // statements
          break;
      }
      if (anzeigennummer>=5) { 
        anzeigennummer=0; 
        Serial.println("Anzeigenummer reset");
      }
    }  
  }
}

void handleRoot() {
  String message="<h1>Now Spinning</h1>";
  message += "Das ist die lieblose Webseite des 'Now Spinning'-Anzeigers</br></br>";
  message += "Temperatur: ";
  message += bme.readTemperature();
  message += " °C<br>";
  message += "Luftfeuchte: ";
  message += bme.readHumidity();
  message += " rel%<br>";
  server.send(200, "text/html", message);
}

void receiveData() {
  String message="<h1>Empfange Daten</h1>";
  message += "Daten werden empfangen...</br></br>";
  server.send(200, "text/html", message);

  Serial.println("Daten empfangen...");
 
  //Aussentemperatur
  Serial.print(server.argName(0));
  Serial.print(F(": "));
  Serial.println(server.arg(0));
  aussentemperatur="< " + server.arg(0) + " \"C";

  //Akkustand
  Serial.print(server.argName(1));
  Serial.print(F(": "));
  Serial.println(server.arg(1));
  akkustand="Akku "+ server.arg(1) + "%";

  //PV Leistung
  Serial.print(server.argName(2));
  Serial.print(F(": "));
  Serial.println(server.arg(2));
  pvleistung="PV "+ server.arg(2) + "KW";

  //Puffer oben u. unten
  Serial.print(server.argName(3));
  Serial.print(F(": "));
  Serial.println(server.arg(3));
  Serial.print(server.argName(4));
  Serial.print(F(": "));
  Serial.println(server.arg(4));
  puffer = "- " + server.arg(3) + "  _ " + server.arg(4); 
}