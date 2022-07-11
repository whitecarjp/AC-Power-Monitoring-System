#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
#include "EmonLib.h"
 
#define SS_PIN 38
#define RST_PIN 39
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

#define beep_pin 10

LiquidCrystal lcd(9,2,3,4,6,7);

unsigned long myTime;

EnergyMonitor emon1;
double Energy;
double realPower;
double Vrms;
double Irms;
int relayPin = 8;
unsigned long clkCnt[99];
int actTime = 0;
int i = 0;

void setup() 
{
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
  

  
  if (content.substring(1) == "B9 17 EE 97") //change here the UID of the card/cards that you want to give access
  {
    timer();
    while(myTime <= 30000){
//    while(myTime != 1800000){
      timer();
      calculate();
    }

    myTime = 0;
    timer();
    digitalWrite(relayPin, HIGH);
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
   // actTime = 0;
    lcd.clear();
    lcd.setCursor(3,0);
    lcd.print("Scan RFID");
    
  }
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
//  }
 else  {
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
} 

void calculate(){
    emon1.calcVI(20,2000);         // Calculate all. No.of half wavelengths (crossings), time-out
  
  realPower = emon1.realPower;        //extract Real Power into variable
  Vrms      = emon1.Vrms;             //extract Vrms into Variable
  Irms      = emon1.Irms;             //extract Irms into Variable
  
  Energy = Energy + (realPower*(2.05/60/60/1000));                          // Kilowatt-hour

  lcd.begin(16,2);
    lcd.clear();
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
  Serial.println("Time: ");
  myTime = millis();

  Serial.println(myTime);
  delay(1000);
  
}
