#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include <WiFi.h>
#include "secrets.h"
#include "ThingSpeak.h" 


char ssid[] = SECRET_SSID;   
char pass[] = SECRET_PASS;   
//int keyIndex = 0;     
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

#define TEMP_FIELD 1
#define HUMD_FIELD 2
#define PRES_FIELD 3
#define ALTI_FIELD 4

#define SEALEVELPRESSURE_HPA (1013.25)

#define I2C_SDA 18
#define I2C_SCL 19

TwoWire I2CBME = TwoWire(0);
Adafruit_BME280 bme;

unsigned long delayTime = 20000;

float Temp = 0;
float Pressure = 0;
float Humidity = 0;
float Altitude = 0;

void Update_Values();
void Print_Values();
void Reconnect_Wifi();

void setup() {

  Serial.begin(9600);
  Serial.println(F("BME280 ThingSpeak test"));

  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client); 

  I2CBME.begin(I2C_SDA, I2C_SCL, 100000);

  bool status;

  status = bme.begin(0x76,&I2CBME);  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
}


void loop() { 
  //Print_Values();
  Update_Values();

  if(WiFi.status() != WL_CONNECTED){
    Reconnect_Wifi();
    Serial.println("\nConnected.");
  }

  ThingSpeak.setField(TEMP_FIELD, Temp);
  ThingSpeak.setField(HUMD_FIELD, Humidity);
  ThingSpeak.setField(PRES_FIELD, Pressure);
  ThingSpeak.setField(ALTI_FIELD, Altitude);

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  delay(delayTime);
}

void Update_Values() {
  Temp = bme.readTemperature();
  Pressure = bme.readPressure() / 100.0F;
  Altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  Humidity = bme.readHumidity();
}

void Print_Values() {
  Serial.print("Temperature = ");
  Serial.print(Temp);
  Serial.println(" *C");
  
  Serial.print("Pressure = ");
  Serial.print(Pressure);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(Altitude);
  Serial.println(" m");

  Serial.print("Humidity = ");
  Serial.print(Humidity);
  Serial.println(" %");

  Serial.println();
}

void Reconnect_Wifi() {
  Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);   
    }
}