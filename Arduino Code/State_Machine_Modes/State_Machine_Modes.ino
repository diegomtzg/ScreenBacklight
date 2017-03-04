#include <LiquidCrystal.h>
#define GREENPIN 11
#define REDPIN 10
#define BLUEPIN 9
#define BUTTONPIN 2
#define PRESS_LENGTH 1

// initialize the LCD library with the numbers of the interface pins
LiquidCrystal lcd(3, 4, 5, 6, 7, 8);

int state = 0;

void setup() 
{
  lcd.begin(16, 2);
  pinMode(BUTTONPIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
}

void loop() 
{
  if(state == 0) // mode 0
  {
    mode0();
  }
  else if(state == 1) // mode 1
  {
    mode1();
  }
  else if(state == 2)
  {
    red();
  }
  else if(state == 3)
  {
    green();
  }
  else if(state == 4)
  {
    blue();
  }
}

bool buttonIsPressed() //pressed but not held
{
  int buttonPress = 0;
  while (digitalRead(BUTTONPIN) == HIGH){
   digitalWrite(LED_BUILTIN, HIGH);
   buttonPress++; 
   delay(50);
  }
  digitalWrite(LED_BUILTIN, LOW);
  if(buttonPress > PRESS_LENGTH && buttonPress < PRESS_LENGTH + 5) // 10 = 1 second
  {
    lcd.clear();
    return true;
  }
  else
    return false;
}

bool buttonIsHeld()
{
  int buttonPress = 0;
  while (digitalRead(BUTTONPIN) == HIGH){
   digitalWrite(LED_BUILTIN, HIGH);
   buttonPress++; 
   delay(50);
  }
  digitalWrite(LED_BUILTIN, LOW);
  if(buttonPress > PRESS_LENGTH + 20) // 10 = 1 second
  {
    return true;
  }
}

void mode0()
{
  while(buttonIsPressed() != true)
  {
    lcd.setCursor(0, 0);
    lcd.print("1:  Computer");
    lcd.setCursor(3, 1);
    lcd.print("BG Lighting");
    int red = 255;
    int blue = 255;
    int green = 255;
    //protocol expects data in format of 4 bytes
    //(xff) as a marker to ensure proper synchronization always
    //followed by red, green, blue bytes
    if (Serial.available()>=4) {
      if(Serial.read() == 0xff){
        red = Serial.read();
        green= Serial.read();
        blue = Serial.read();
      }
    }
    //finally control led brightness through pulse-width modulation
    analogWrite(REDPIN, red);
    analogWrite(GREENPIN, green);
    analogWrite(BLUEPIN, blue);
  }
  state++;
}

void mode1()
{
  int FADESPEED = 5;
  while(buttonIsPressed() != true)
  {
    lcd.setCursor(0, 0);
    lcd.print("2:   Color");
    lcd.setCursor(5, 1);
    lcd.print("Swirl");
      int r = 0;
      int g = 0;
      int b = 0;
       //fade from blue to violet
      for (r = 0; r < 256; r++) { 
        if(buttonIsPressed() == true)
          goto exitfunction;
          
        analogWrite(REDPIN, r);
        delay(FADESPEED);
      } 
       //fade from violet to red
      for (b = 255; b > 0; b--) { 
        if(buttonIsPressed() == true)
          goto exitfunction;
          
        analogWrite(BLUEPIN, b);
        delay(FADESPEED);
      } 
      // fade from red to yellow
      for (g = 0; g < 256; g++) { 
        if(buttonIsPressed() == true)
          goto exitfunction;
          
        analogWrite(GREENPIN, g);
        delay(FADESPEED);
      } 
      // fade from yellow to green
      for (r = 255; r > 0; r--) { 
        if(buttonIsPressed() == true)
          goto exitfunction;
          
        analogWrite(REDPIN, r);
        delay(FADESPEED);
      } 
      // fade from green to teal
      for (b = 0; b < 256; b++) { 
        if(buttonIsPressed() == true)
          goto exitfunction;
          
        analogWrite(BLUEPIN, b);
        delay(FADESPEED);
      } 
      // fade from teal to blue
      for (g = 255; g > 0; g--) { 
        if(buttonIsPressed() == true)
          goto exitfunction;
          
        analogWrite(GREENPIN, g);
        delay(FADESPEED);
      } 
  }
  exitfunction: state++;
}

void red()
{
  while(buttonIsPressed() != true)
  {
    lcd.setCursor(0, 0);
    lcd.print("3:    Red");
    analogWrite(REDPIN, 255);
    analogWrite(GREENPIN, 0);
    analogWrite(BLUEPIN, 0);
  }
  state++;
}

void green()
{
  while(buttonIsPressed() != true)
  {
    lcd.setCursor(0, 0);
    lcd.print("4:    Green");
    analogWrite(GREENPIN, 255);
    analogWrite(REDPIN, 0);
    analogWrite(BLUEPIN, 0);
  }
  state++;
}

void blue()
{
  while(buttonIsPressed() != true)
  {
    lcd.setCursor(0, 0);
    lcd.print("5:    Blue");
    analogWrite(BLUEPIN, 255);
    analogWrite(REDPIN, 0);
    analogWrite(GREENPIN, 0);
  }
  state = 0;
}

