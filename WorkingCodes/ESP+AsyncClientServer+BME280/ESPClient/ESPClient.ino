/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-client-server-wi-fi/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClient.h>

ESP8266WiFiMulti WiFiMulti;

const char* ssid = "BME280SERVER";
const char* password = "123456789";

//Your IP address or domain name with URL path
const char* serverNameTemp = "http://192.168.4.1/temperature";
const char* serverNameHumi = "http://192.168.4.1/humidity";
const char* serverNamePres = "http://192.168.4.1/pressure";
const char* serverNameAlti = "http://192.168.4.1/altitude";

#include <Wire.h>

// For OLED Display
/*
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
*/

// For 20x4 LCD Display

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

String temperature;
String humidity;
String pressure;
String altitude;

unsigned long previousMillis = 0;
const long interval = 5000; 

void setup() {

// For 20x4 LCD Display
  lcd.init();                       
  lcd.init();
  lcd.backlight();
// For 20x4 LCD Display

  Serial.begin(115200);
  
/* Uncomment for OLED Display 
  // Address 0x3C for 128x64, you might need to change this value (use an I2C scanner)
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  
  
*/ //Uncomment ends here

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
    unsigned long currentMillis = millis();  
    if(currentMillis - previousMillis >= interval) {
      if(WiFi.status()== WL_CONNECTED ){             // Check WiFi connection status                           
        lcd.clear();
        temperature = httpGETRequest(serverNameTemp);
        humidity = httpGETRequest(serverNameHumi);
        pressure = httpGETRequest(serverNamePres);
        altitude = httpGETRequest(serverNameAlti); 
        Serial.println("Temperature: " + temperature + " *C - Humidity: " + humidity + " % - Pressure: " + pressure + " hPa - Altitude: " + altitude + " m");
        
        //For LCD Display
        
        //Temperature
        lcd.setCursor(0,0);
        lcd.print("T: ");
        lcd.print(temperature);
        lcd.print("00 C");

        //Humidity
        lcd.setCursor(0,1);
        lcd.print("H: ");
        lcd.print(humidity);
        lcd.print("00 %");

        //Pressure
        lcd.setCursor(0,2);
        lcd.print("P: ");
        lcd.print(pressure);
        lcd.print(" hPa");

        //Altitude
        lcd.setCursor(0,3);
        lcd.print("A: ");
        lcd.print(altitude);
        lcd.print("00 m");
        
        /* Uncomment for OLED Display

        display.clearDisplay();
      
        //display temperature

        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(0,0);
        display.print("T: ");
        display.print(temperature);
        display.print(" ");
        display.setTextSize(1);
        display.cp437(true);
        display.write(248);
        display.setTextSize(2);
        display.print("C");
      
        // display humidity
        display.setTextSize(2);
        display.setCursor(0, 25);
        display.print("H: ");
        display.print(humidity);
        display.print(" %"); 
      
        // display pressure
        display.setTextSize(2);
        display.setCursor(0, 50);
        display.print("P:");
        display.print(pressure);
        display.setTextSize(1);
        display.setCursor(110, 56);
        display.print("hPa");
           
        display.display();

        */ //Uncomment ends here
     
        // save the last HTTP GET Request
        previousMillis = currentMillis;
      }
      else {
        Serial.println("WiFi Disconnected");
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("WiFi Connection Lost");
        delay(200);
      }
    }
  }

  String httpGETRequest(const char* serverName) {
    WiFiClient client;
    HTTPClient http;
    if (http.begin(client, serverName)) { 
//      Serial.print("Connecting to ..."(const char* serverName));       // start connection and send HTTP header
      int httpCode = http.GET();
      String payload = "--"; 
      if (httpCode > 0) {       // httpCode will be negative on error
        Serial.printf("[HTTP] GET... code: %d\n", httpCode); // HTTP header has been send and Server response header has been handled
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {          // file found at server
          payload = http.getString();
          Serial.println(payload);
        }
      }
      else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }      
      http.end();
      return payload;
    } 
    else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
  }
