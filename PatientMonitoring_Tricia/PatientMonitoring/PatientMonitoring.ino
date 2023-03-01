#include <Adafruit_GFX.h>        //OLED libraries
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "MAX30105.h"           //MAX3010x library
#include "heartRate.h"          //Heart rate calculating algorithm
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <Adafruit_GFX.h>        
#include <Adafruit_SSD1306.h>
#define USE_SERIAL Serial

const char * ssid     = "HUAWEI-g54N";
const char * password = "y7qvA78k";

MAX30105 particleSensor;

#define NETWORK_LED 18

//Heart Rate Calculation Globals
const byte RATE_SIZE = 10; //Increase this for more averaging
byte rates[RATE_SIZE]; 
byte rateSpot = 0;
long lastBeat = 0; 
float beatsPerMinute;
int beatAvg;
int prevAvg = 0;
int stabilityCount = 0;
#define stabilityDiff  3
#define stabilityLimit 5

//OLED Paramas
#define SCREEN_WIDTH  128 
#define SCREEN_HEIGHT 32 
#define OLED_RESET    -1 

static const unsigned char PROGMEM logo2_bmp[] =
{ 0x03, 0xC0, 0xF0, 0x06, 0x71, 0x8C, 0x0C, 0x1B, 0x06, 0x18, 0x0E, 0x02, 0x10, 0x0C, 0x03, 0x10,             
0x04, 0x01, 0x10, 0x04, 0x01, 0x10, 0x40, 0x01, 0x10, 0x40, 0x01, 0x10, 0xC0, 0x03, 0x08, 0x88,
0x02, 0x08, 0xB8, 0x04, 0xFF, 0x37, 0x08, 0x01, 0x30, 0x18, 0x01, 0x90, 0x30, 0x00, 0xC0, 0x60,
0x00, 0x60, 0xC0, 0x00, 0x31, 0x80, 0x00, 0x1B, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x04, 0x00,  };

static const unsigned char PROGMEM logo3_bmp[] =
{ 0x01, 0xF0, 0x0F, 0x80, 0x06, 0x1C, 0x38, 0x60, 0x18, 0x06, 0x60, 0x18, 0x10, 0x01, 0x80, 0x08,
0x20, 0x01, 0x80, 0x04, 0x40, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x02, 0xC0, 0x00, 0x08, 0x03,
0x80, 0x00, 0x08, 0x01, 0x80, 0x00, 0x18, 0x01, 0x80, 0x00, 0x1C, 0x01, 0x80, 0x00, 0x14, 0x00,
0x80, 0x00, 0x14, 0x00, 0x80, 0x00, 0x14, 0x00, 0x40, 0x10, 0x12, 0x00, 0x40, 0x10, 0x12, 0x00,
0x7E, 0x1F, 0x23, 0xFE, 0x03, 0x31, 0xA0, 0x04, 0x01, 0xA0, 0xA0, 0x0C, 0x00, 0xA0, 0xA0, 0x08,
0x00, 0x60, 0xE0, 0x10, 0x00, 0x20, 0x60, 0x20, 0x06, 0x00, 0x40, 0x60, 0x03, 0x00, 0x40, 0xC0,
0x01, 0x80, 0x01, 0x80, 0x00, 0xC0, 0x03, 0x00, 0x00, 0x60, 0x06, 0x00, 0x00, 0x30, 0x0C, 0x00,
0x00, 0x08, 0x10, 0x00, 0x00, 0x06, 0x60, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x01, 0x80, 0x00  };


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //Declaring the display name (display)



struct Patient{
  int bpm;
  int spo2;
  int temp;
  String deviceID;
};

Patient patient;

void setup() {  
  
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //Start the OLED display
  display.display();
  pinMode(NETWORK_LED, OUTPUT);


  initWiFi();
  checkNetwork();
  
  initSensor();

  patient.deviceID = "CM001";
  //patient.spo2 = particleSensor.getSPO2(90);
  //Serial.println("SPO2: " + String(patient.spo2));

 
}

void loop() {

  checkNetwork();
  
  long irValue = particleSensor.getIR();    //Reading the IR value it will permit us to know if there's a finger on the sensor or not   
                                     
  if(irValue > 7000){                                         //If a finger is detected
    
    displayReadings(&patient, false);
      
    if (checkForBeat(irValue) == true){                       //If a heart beat is detected
      patient.temp = particleSensor.readTemperature();
      Serial.println("BPM: " + String(patient.bpm));
      if(abs(prevAvg - patient.bpm) < stabilityDiff && patient.bpm > 40 && patient.spo2 > 90){
        stabilityCount++;
        if(stabilityCount >= stabilityLimit){
          displaySending();

          sendDataToWeb(&patient);
          
          Serial.println("\n------------------------");
          Serial.println("Sending Data:");
          Serial.println("Patient ID: " + String(patient.deviceID));
          Serial.println("BPM: " + String(patient.bpm));
          Serial.println("SPO2: " + String(patient.spo2));
          Serial.println("TEMP: " + String(patient.temp));
          Serial.println("------------------------\n");
          sendingTone();
         
          delay(3000);
          stabilityCount = 0;
        }
      }
      else
        stabilityCount = 0;
      
      
      prevAvg = patient.bpm;
      displayReadings(&patient, true);
      
      tone(19,3000);                                        //And tone the buzzer for a 100ms you can reduce it it will be better
      delay(100);
      tone(19, 0);                                          //Deactivate the buzzer to have the effect of a "bip"
      
      calculateBPM();
      
    }
  
  }
  if (irValue < 7000){                             //If no finger is detected it inform the user and put the average BPM to 0 or it will be stored for the next measure
     patient.bpm  = 0;
     patient.spo2 = 0;
     patient.temp = 0;
     displayNoneDetection();
  }
}
