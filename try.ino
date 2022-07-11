#include <LiquidCrystal.h>

LiquidCrystal lcd(9,2,3,4,6,7); //rs,en,dio1,dio2,dio3,dio4

//Input & Button Logic
const int numOfInputs = 3;
const int inputPins[numOfInputs] = {22,23,24};
int inputState[numOfInputs];
int lastInputState[numOfInputs] = {LOW,LOW,LOW};
bool inputFlags[numOfInputs] = {LOW,LOW,LOW};
long lastDebounceTime[numOfInputs] = {0,0,0};
long debounceDelay = 5;

//LCD Menu Logic
const int numOfScreens = 3;
int currentScreen = 0;
//String screens[numOfScreens] = {{"Welcome"},{"Monitoring"},{"Clock"}};

unsigned long time; 
long int seconds=0; 
int minutes=0; 
int set=0; 
int reset=0; 

void setup(){
  for(int i = 0; i < numOfInputs; i++){
    pinMode(inputPins[i], INPUT);
    digitalWrite(inputPins[i], HIGH); //pullup 20k
  }
  Serial.begin(9600);
  lcd.begin(16,2);
}

void loop(){
  setInputFlags();
  resolveInputFlags();
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
//          Serial.println(i);
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
//      Serial.println(i);
      Serial.println(currentScreen);
      inputFlags[i] = LOW;
      printScreen();
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
    lcd.print("Welcome");
  }else if(currentScreen == 1){
    lcd.clear();
    delay(10);
    lcd.setCursor(0,0);
    lcd.print("RFID");
  }else{
    lcd.clear();
    delay(10);
    lcd.setCursor(0,0);
    lcd.print("CLOCK");
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
