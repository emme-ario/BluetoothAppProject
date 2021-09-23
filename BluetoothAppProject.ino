//Include Libraries
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include "DHT.h"
#include <Wire.h>
#include "RTClib.h"

//Define RxD and TxD pins on arduino
#define Rx 10
#define Tx 9

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); //Define lcd pins on arduino
SoftwareSerial BTSerial(Rx, Tx); //Define RxD and TxD pins for SoftwareSerial

//Define 16x2 LCD
int screenWidth = 16;
int screenHeight = 2;

//Define DHT pin on arduino
#define DHTPin 13

//Define DHT sensor type
#define DHTType DHT11

//Initialize DHT sensor
DHT DHTSensor(DHTPin, DHTType);

//Initialize DS3231 module
RTC_DS3231 rtc;

//Define RGB led pins
#define redPin 6
#define greenPin 7
#define bluePin 8

//Define two lcd lines
String line1 = "Please, type a message...";
String line2 = "I'll wait!";

//Define vars to Hmove text on lcd
int stringStart = 0;
int stringEnd = 0;
int scrollCursor = screenWidth;

//Define variable to start Serial connection
char incomingValue = 0;

void setup() {
  BTSerial.begin(9600);
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Please, connect");
  lcd.setCursor(0, 1);
  lcd.print("your device...");
    
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  DHTSensor.begin();
  //Check if RTC module has started
  if(!rtc.begin()) {
    Serial.println("verify RTC connection");
    while(true);
  }
  //Sets date and time if rtc has no power
  if(rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  RGBColor(255, 0, 255);

  if(BTSerial.available() > 0) {
    incomingValue = BTSerial.read();
    BTSerial.print(incomingValue);
    Serial.println(incomingValue);

    lcd.clear();
    
    //DATE and TIME
    while(incomingValue == '1') {
      DateTime now = rtc.now();
      lcd.setCursor(0, 0);
      lcd.print("Date: ");
      lcd.print(pad(now.day()));
      lcd.print("/");
      lcd.print(pad(now.month()));
      lcd.print("/");
      lcd.print(now.year(), DEC);
      lcd.setCursor(0, 1);
      lcd.print("Time: ");
      lcd.print(pad(now.hour()));
      lcd.print(":");
      lcd.print(pad(now.minute()));
      lcd.print(":");
      lcd.print(pad(now.second()));

      if(BTSerial.available() > 0)
        incomingValue = BTSerial.read();
    }

    //SEND MESSAGE
    while(incomingValue == '2') {
      lcd.setCursor(3, 1); //Set the cursor on first row
      lcd.print(line2); //Print line2 message
      lcd.setCursor(scrollCursor, 0); //Set the cursor on first row and scroll from L to R
      lcd.print(line1.substring(stringStart, stringEnd)); //Print line1 first character
    
      delay(400);
      lcd.clear();
    
      if(stringStart == 0 && scrollCursor > 0) {
        scrollCursor--; //Move cursor from 16 to 0
        stringEnd++; //Print character from 0 to 15
      }
    
      else if(stringStart == stringEnd) { //Start again
        stringStart = stringEnd = 0;
        scrollCursor = screenWidth;
      }
    
      else if(stringEnd == line1.length() && scrollCursor == 0) {
        stringStart++;
      }
    
      else {
        stringStart++;
        stringEnd++;
      }

      if(BTSerial.available() > 0)
        incomingValue = BTSerial.read();
    }

    //TEMPERATURE and HUMIDITY
    while(incomingValue == '3') {
      //Read Humidity and Temperature to keep them updated
      float h = DHTSensor.readHumidity();
      float t = DHTSensor.readTemperature();
      
      if(isnan(h) || isnan(t)) {
        lcd.setCursor(0, 0);
        lcd.print("Failed to read!");
        return;
      }
      
      lcd.setCursor(0, 0);
      lcd.print("Temp: ");
      lcd.print(t);
      lcd.print(" C");
      lcd.setCursor(0, 1);
      lcd.print("Hum: ");
      lcd.print(h);
      lcd.print(" %");

      if(BTSerial.available() > 0)
        incomingValue = BTSerial.read();
    }
  }
}

void RGBColor(int redValue, int greenValue, int blueValue) {
  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin, blueValue);
}

char *res = malloc(5); //Alloca memoria nell'arduino (*res = puntatore)
//Formatta i numeri a una cifra aggiungendo uno 0 a sinistra (0 -> 00 ... 9 -> 09)
String pad(int n) {
  sprintf(res, "%02d", n);
  return String(res);
}

/*
    if(BTSerial.available() > 0)
      incomingValue = BTSerial.read();
*/
