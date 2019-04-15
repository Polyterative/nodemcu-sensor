#include <DHT.h> //  DHT.h library
#include <ESP8266WiFi.h> // ESP8266WiFi.h library

#define DHTPIN D2

#define DHTTYPE  DHT22

//==============================================
const char* Sensore = "0";
//==============================================

const char* ssid     = "****";
const char* password = "****";
const char* host = "192.168.1.111";

//float TempAct = 20;
//float HRAct = 50;
float TempCorrection = 0.0;     //Used to adjust temp readings, if the sensor needs calibration
float HRCorrection = 0.0;       //Used to adjust HR readings, if the sensor needs calibration

DHT dht(DHTPIN, DHTTYPE, 15);

void setup() {

  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoConnect(false);
  WiFi.disconnect(true);

  delay(1000);
  Serial.println("                                   ");
  Serial.println("Booting sensor n° " + String(Sensore));
  Serial.println();

  // Initialize sensor
  dht.begin();
  delay(1000);

  //  Connect to WiFi network
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print ("Connecting to " + String(ssid) + "...");
    WiFi.begin(ssid, password);
  }
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  WiFi.setAutoConnect(true);
  Serial.println (" OK, connected! :) ");
  Serial.print("MAC:                     ");
  Serial.println(WiFi.macAddress());
  Serial.print("WiFi signal strength:    ");
  Serial.println(WiFi.RSSI());
  Serial.print("IP address:              ");
  Serial.println(WiFi.localIP());

  delay(5000);


  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    return;
  }

  Serial.println ("");
  Serial.println ("Lettura iniziale sensore (senza correzioni calibrazione):");

  Serial.print("Initial Temperature: ");
  Serial.print(temperature, 1);
  Serial.println (" C");
  Serial.print("Initial Humidity % : ");
  Serial.print(humidity, 1);
  Serial.println (" %");

  Serial.println ();
  Serial.print ("Correzione calibrazione Temperatura : ");
  Serial.print (TempCorrection, 1);
  Serial.println (" C");
  Serial.print ("Correzione calibrazione Humidity %  : ");
  Serial.print (HRCorrection, 1);
  Serial.println (" %");

  //TempAct = temperature;
  //HRAct = humidity;
  delay(5000);
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  //float humidity=60;
  //float temperature=25;

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("impossibile leggere temperatura o umidità");
    delay(2000);
    return;
  }

  temperature = float (temperature + TempCorrection);
  humidity = float (humidity + HRCorrection);

  //=============================================================================
  //  float TempAvg = float((temperature + TempAct + (2 * TempCorrection))/2);

  //        if (TempAvg > TempAct){
  //          TempAct += 0.1;
  //        }
  //        if (TempAvg < TempAct){
  //          TempAct -= 0.1;
  //        }

  //  float HRAvg = float((humidity + HRAct)/2);

  //        if (HRAvg > HRAct){
  //          HRAct += 1;
  //        }
  //        if (HRAvg < HRAct){
  //          HRAct -= 1;
  //        }
  //==============================================================================

  Serial.println ("");
  Serial.println ("Sending dataread to " + String(host) + " with calibration correction:");

  Serial.print("Actual Temperature : ");
  Serial.print(temperature, 1);  
  Serial.println (" C");
  Serial.print("Actual Humidity %  : ");
  Serial.print(humidity, 1);       
  Serial.println (" %");


  // make TCP connections
  WiFiClient client;
  const int httpPort = 5000;
  if (!client.connect(host, httpPort)) {
    Serial.println("Error while sending data to server! :( ");
    return;
  }

  String url = "/api/dataadder/add?mac=";
  url += WiFi.macAddress();
  url += "&temperature=";
  url += String(temperature, 1);
  url += "&humidity=";
  url += String(humidity, 0);

  // Request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  delay(10000);   // 900000 = 1 lettura ogni 15 minuti

}
