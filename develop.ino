#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
#include "EmonLib.h"
 
#define SS_PIN 38
#define RST_PIN 39
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

#define beep_pin 10

LiquidCrystal lcd(9,2,3,4,6,7);

//Input & Button Logic
const int numOfInputs = 3;
const int inputPins[numOfInputs] = {24,26,28};
int inputState[numOfInputs];
int lastInputState[numOfInputs] = {LOW,LOW,LOW};
bool inputFlags[numOfInputs] = {LOW,LOW,LOW};
long lastDebounceTime[numOfInputs] = {0,0,0};
long debounceDelay = 5;

//LCD Menu Logic
const int numOfScreens = 3;
int currentScreen = 0;

EnergyMonitor emon1;
double Energy;
double realPower;
double Vrms;
double Irms;
int relayPin = 8;
unsigned long clkCnt[99];
int actTime = 0;
int i = 0;
unsigned long myTime;

//time
unsigned long time;
long int seconds = 0;
int minutes = 0;
int set = 0;
int reset = 0;

void setup() 
{
  for(int i = 0; i < numOfInputs; i++){
    pinMode(inputPins[i], INPUT);
    digitalWrite(inputPins[i], HIGH); //pullup 20k
  }
  
  Serial.begin(9600);   // Initiate a serial communication
  
  pinMode(beep_pin,OUTPUT);    //buzzer
  digitalWrite(beep_pin,LOW);

  pinMode(relayPin, OUTPUT);   //Relay
  digitalWrite(relayPin, HIGH);
  
  lcd.begin(16, 2);
  lcd.setCursor(3,0);
  lcd.print("Scan RFID");
  delay(1000);
  
   
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Put your card to the reader...");
  Serial.println();

 emon1.voltage(A0, 569.95, 1.7);  // Voltage: input pin, calibration, phase_shift
 emon1.current(A1, 1.6531);       // Current: input pin, calibration.
  
}

void loop() 
{
  
  digitalWrite(beep_pin, LOW);

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();

  
  calculate();
//  setClock();
//  setInputFlags();
//  resolveInputFlags();
   lcd.clear();
//  lcd.print(screens[currentScreen]);
if (content.substring(1) == "B9 17 EE 97") //change here the UID of the card/cards that you want to give access
  {
    timer();
    while(actTime != 30000){
//    while(myTime != 1800000){
    Serial.println(actTime);

    digitalWrite(relayPin, HIGH);
    digitalWrite(relayPin, LOW);

    lcd.setCursor(0,0); 
    lcd.print(realPower); 
    lcd.print("W"); 
    lcd.setCursor(8,0);
    lcd.print(Energy); 
    lcd.print("kwh");
    lcd.setCursor(0,1); 
    lcd.print(Vrms); 
    lcd.print("V"); 
    lcd.setCursor(8,1);
    lcd.print(Irms);
    lcd.print("A"); 
    delay(800);
   // actTime = 0;
//    lcd.clear();
// if (content.substring(1) == "47 52 8D 56") // timer
//  {
//      calculate();
//    digitalWrite(beep_pin,HIGH);
//    delay(200);
//    digitalWrite(beep_pin,LOW);
//    delay(100);
//    lcd.begin(16,2);
//    lcd.clear();
////    lcd.setCursor(0,0);
////    lcd.print("ID : ");
////    lcd.print(content.substring(1));
////    lcd.setCursor(0,1);
////    lcd.print("Authorized access");
////    Serial.println("Authorized access");
//    digitalWrite(relayPin, LOW);
//    lcd.setCursor(0,0); 
//    lcd.print(realPower); 
//    lcd.print("W"); 
//    delay(20); 
//    lcd.setCursor(8,0);
//    lcd.print(Energy); 
//    lcd.print("kwh");
//    delay(20); 
//    lcd.setCursor(0,1); 
//    lcd.print(Vrms); 
//    lcd.print("V");
//    delay(20); 
//    lcd.setCursor(8,1);
//    lcd.print(Irms);
//    lcd.print("A"); 
//    delay(20); 
  }
  }else  {
    digitalWrite(relayPin, HIGH);
    digitalWrite(beep_pin,HIGH);
    lcd.setCursor(0, 0);
    lcd.print("ID : ");
    lcd.print(content.substring(1));
    lcd.setCursor(0, 1);
    lcd.print("Access denied");
    Serial.println(" Access denied");
    delay(1500);
   // myTime = 0;
    lcd.clear();
    lcd.setCursor(3,0);
    lcd.print("Scan RFID");
     }
//  if(currentScreen == 0){
//    lcd.clear();
//    delay(10);
//    lcd.setCursor(1,0);
//    lcd.print("AC POWER METER");
//    lcd.setCursor(0,1);
//    lcd.print("Press Button");
//  }else if(currentScreen == 1){
    lcd.begin(16,2);
    lcd.clear();
//    lcd.setCursor(0,0);
//    lcd.print("ID : ");
//    lcd.print(content.substring(1));
//    lcd.setCursor(0,1);
//    lcd.print("Authorized access");
//    Serial.println("Authorized access");
//  }else{
//    lcd.clear();
//    delay(10);
//    lcd.setCursor(0, 1);
//
//    if (minutes < 10) {
//      lcd.print("0");
//      lcd.print(minutes);
//      lcd.print(":");
//    } else {
//      lcd.print(minutes);
//      lcd.print(":");
//    }
//
//    if (seconds < 10) {
//      lcd.print("0");
//      lcd.print(seconds);
//    } else {
//      lcd.print(seconds);
//    }
   }


void calculate(){
    emon1.calcVI(20,2000);         // Calculate all. No.of half wavelengths (crossings), time-out
  
  realPower = emon1.realPower;        //extract Real Power into variable
  Vrms      = emon1.Vrms;             //extract Vrms into Variable
  Irms      = emon1.Irms;             //extract Irms into Variable
  
  Energy = Energy + (realPower*(2.05/60/60/1000));                          // Kilowatt-hour
}

void setInputFlags(){
  for(int i = 0; i < numOfInputs; i++){
    int reading = digitalRead(inputPins[i]);
    if(reading != lastInputState[i]){
      lastDebounceTime[i] = millis();
    }
    if((millis() - lastDebounceTime[i]) > debounceDelay){
      if(reading != inputState[i]){
        inputState[i] = reading;
        if(inputState[i] == HIGH){
          Serial.println(i);
          inputFlags[i] = HIGH;
        }
      }
    }
    lastInputState[i] = reading;
  }
}

void resolveInputFlags(){
  for(int i = 0; i < numOfInputs; i++){
    if(inputFlags[i] == HIGH){
      inputAction(i);
      Serial.println(i);
      Serial.println(currentScreen);
      inputFlags[i] = LOW;  
    }
  }
}

void inputAction(int input){
  if(input == 0){
    if(currentScreen == 0){
      currentScreen = numOfScreens - 1;  
    }else{
      currentScreen--;
    }
  }else if(input == 1){
    if(currentScreen == numOfScreens - 1){
      currentScreen = 0;
    }else{
      currentScreen++;
    }
  }
}

void printScreen(){
  lcd.clear();
//  lcd.print(screens[currentScreen]);
  if(currentScreen == 0){
    lcd.clear();
    delay(10);
    lcd.setCursor(0,0);
    lcd.print("AC POWER METER");
  }else if(currentScreen == 1){
    calculate();
    lcd.begin(16,2);
    lcd.clear();
//    lcd.setCursor(0,0);
//    lcd.print("ID : ");
//    lcd.print(content.substring(1));
//    lcd.setCursor(0,1);
//    lcd.print("Authorized access");
//    Serial.println("Authorized access");
    digitalWrite(relayPin, LOW);

    lcd.setCursor(0,0); 
    lcd.print(realPower); 
    lcd.print("W"); 
    lcd.setCursor(8,0);
    lcd.print(Energy); 
    lcd.print("kwh");
    lcd.setCursor(0,1); 
    lcd.print(Vrms); 
    lcd.print("V"); 
    lcd.setCursor(8,1);
    lcd.print(Irms);
    lcd.print("A"); 
    delay(800);
  }else{
    setClock();
    lcd.clear();
    delay(10);
    lcd.setCursor(0, 1);

    if (minutes < 10) {
      lcd.print("0");
      lcd.print(minutes);
      lcd.print(":");
    } else {
      lcd.print(minutes);
      lcd.print(":");
    }

    if (seconds < 10) {
      lcd.print("0");
      lcd.print(seconds);
    } else {
      lcd.print(seconds);
    }
  }
}

void setClock() {
  seconds++;
  delay(1000);

  if (seconds > 59) {
    seconds = 0;
    minutes++;
  }

  if (minutes > 59) {
    minutes = 0;
  }
}

void printValues(){
    calculate();
    lcd.begin(16,2);
    lcd.clear();
//    lcd.setCursor(0,0);
//    lcd.print("ID : ");
//    lcd.print(content.substring(1));
//    lcd.setCursor(0,1);
//    lcd.print("Authorized access");
//    Serial.println("Authorized access");
    digitalWrite(relayPin, LOW);

    lcd.setCursor(0,0); 
    lcd.print(realPower); 
    lcd.print("W"); 
    lcd.setCursor(8,0);
    lcd.print(Energy); 
    lcd.print("kwh");
    lcd.setCursor(0,1); 
    lcd.print(Vrms); 
    lcd.print("V"); 
    lcd.setCursor(8,1);
    lcd.print(Irms);
    lcd.print("A"); 
    delay(800);
}

void timer(){
  Serial.print("Time: ");
  myTime = millis();

//  Serial.println(myTime);
  delay(1000);
  
}
