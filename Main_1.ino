#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
#include "EmonLib.h"

//rfid
#define SS_PIN 10
#define RST_PIN 9

// buttons
#define UP 11
#define DOWN 25
#define ENTER 28

//LED
int ledPin = LED_BUILTIN;

//enter
int ledState = LOW;
int btnState;
int lastButtonState = LOW;

bool upPrevState = HIGH;
bool downPrevState = HIGH;

//btn read
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

int currentMenu = 0;

//time
unsigned long time;
long int seconds = 0;
int minutes = 0;
int set = 0;
int reset = 0;

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

#define beep_pin 26

LiquidCrystal lcd(22, 2, 3, 4, 6, 7);


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

  pinMode(beep_pin, OUTPUT);   //buzzer
  digitalWrite(beep_pin, LOW);

  //button initialization
  pinMode(UP, INPUT_PULLUP);
  pinMode(DOWN, INPUT_PULLUP);
  pinMode(ENTER, INPUT);

  //LED initialization
  pinMode(ledPin, OUTPUT);


  pinMode(relayPin, OUTPUT);   //Relay
  digitalWrite(relayPin, HIGH);

  lcd.begin(16, 2);

  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  //  Serial.println("Press green button then");
  //  Serial.println("Put your card to the reader...");

  emon1.voltage(A0, 569.95, 1.7);  // Voltage: input pin, calibration, phase_shift
  emon1.current(A1, 1.6531);       // Current: input pin, calibration.

}


void RFID() {
  lcd.clear();
  delay(10);

  digitalWrite(beep_pin, LOW);
  digitalWrite(relayPin, HIGH);
  // actTime = 0;
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Scan RFID");
  
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
//    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  

  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content = "";
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
    monitoringValues();
  }
  else  {
    digitalWrite(relayPin, HIGH);
    digitalWrite(beep_pin, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("ID : ");
    lcd.print(content.substring(1));
    lcd.setCursor(0, 1);
    lcd.print("Access denied");
    Serial.println(" Access denied");
    delay(1500);
    // myTime = 0;
    lcd.clear();
    delay(50);
    lcd.setCursor(3, 0);
    lcd.print("Scan RFID");
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

void loop()
{
  int reading = digitalRead(ENTER);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }


  if ((millis() - lastDebounceTime) > debounceDelay) {

    //button changed
    if (reading != btnState) {
      btnState = reading;

      if (btnState == HIGH) {
        ledState = !ledState;
      }
    }
  }
  if (ledState == HIGH) {
    if (digitalRead(UP) != upPrevState) {
      upPrevState = !upPrevState;

      if (!upPrevState) {
        //btn pressed
        if (currentMenu > 0) {
          currentMenu--;
        } else {
          currentMenu = 1;
        }
        //print menu
        MenuChanged();
      }
    }

    if (digitalRead(DOWN) != downPrevState) {
      downPrevState = !downPrevState;

      if (!downPrevState) {
        //btn pressed
        if (currentMenu < 1) {
          currentMenu++;
        } else {
          currentMenu = 0;
        }
        //print menu
        MenuChanged();
        //        mainMenu();
      }
    }
    delay(50);
  }
  else {
    lcd.clear();
    delay(10);
    lcd.setCursor(1, 0);
    lcd.print("AC POWER METER");
    lcd.setCursor(0, 1);
    lcd.print("Green to START");
    delay(10);
  }

  Serial.println(ledState);
  digitalWrite(ledPin, ledState);
  lastButtonState = reading;
}

void calculate() {
  emon1.calcVI(20, 2000);        // Calculate all. No.of half wavelengths (crossings), time-out

  realPower = emon1.realPower;        //extract Real Power into variable
  Vrms      = emon1.Vrms;             //extract Vrms into Variable
  Irms      = emon1.Irms;             //extract Irms into Variable

  Energy = Energy + (realPower * (2.05 / 60 / 60 / 1000));                  // Kilowatt-hour
}

void monitoringValues() {
  calculate();
  lcd.begin(16, 2);
  lcd.clear();
  delay(10);
  digitalWrite(relayPin, LOW);

  lcd.setCursor(0, 0);
  lcd.print(realPower);
  lcd.print("W");
  lcd.setCursor(8, 0);
  lcd.print(Energy);
  lcd.print("kwh");
  lcd.setCursor(0, 1);
  lcd.print(Vrms);
  lcd.print("V");
  lcd.setCursor(8, 1);
  lcd.print(Irms);
  lcd.print("A");
  delay(800);


}
void MenuChanged() {

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);


  Serial.println(currentMenu);
  if (currentMenu == 1) {
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
  } else {
    RFID();
  }
}
