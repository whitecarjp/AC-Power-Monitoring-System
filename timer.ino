#include <LiquidCrystal.h>                 //includes the library code of LCD in aurdino 
unsigned long time; 
long int seconds; 
int minutes=1; 
int set=0; 
int reset=0; 
LiquidCrystal LCD(9,2,3,4,6,7);  //RS,EN,D4,D5,D6,D7 pin of LCD respectively 

void setup() 
{ 
Serial.begin(9600);                   //initiates serial communication 
LCD.begin(16, 2);     //sets the number of columns and rows 
} 

void loop() 
{ 
//set=digitalRead(8); 
//Serial.println(set); 
while(minutes != 0 | seconds != 0){
  printClock();
}
}

void printClock(){
  setClock();         //function 
  LCD.setCursor(0,1);     //sets the cursor to 0th column and 1st row,numbering starts from 0 

  if(minutes<10) 
  { 
  LCD.print("0"); 
  LCD.print(minutes); 
  LCD.print(":"); 
  } 
  else 
  { 
  LCD.print(minutes); 
  LCD.print(":"); 
  } 
   
  
  if(seconds<10) 
  { 
  LCD.print("0"); 
  LCD.print(seconds); 
  } 
  else 
  { 
  LCD.print(seconds); 
  } 
 } 




void setClock() 
{       
if (minutes > 0) 
{ 
minutes--;
}
delay(1000);     //a delay of 1000 milliseconds is given for each second count 
if (minutes == 0) 
{ 
seconds = 59;  
minutes = 0;      //whenever second is greater than 59 than second is made equal to 
seconds--;     // zero and a minute is counted for it 
} 

//if(seconds > 59){
//  
//}
} 
