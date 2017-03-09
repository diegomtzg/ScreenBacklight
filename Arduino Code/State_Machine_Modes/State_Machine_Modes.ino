#include <LiquidCrystal.h>
#define GREENPIN 11
#define REDPIN 10
#define BLUEPIN 9
#define PUSHBUTTON_PIN 2
#define JOYSTICK_BUTTON_PIN 13
#define PRESS_LENGTH 1

// initialize the LCD library with the numbers of the interface pins
LiquidCrystal lcd(3, 4, 5, 6, 7, 8);

int state = 0;
int red = 0;
int blue = 0;
int green = 0;

void setup() 
{
  lcd.begin(16, 2);
  pinMode(PUSHBUTTON_PIN, INPUT);
  pinMode(JOYSTICK_BUTTON_PIN, INPUT);
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
    mode2();
  }
}

bool buttonIsPressed(int buttonPin) //pressed but not held
{
  int buttonPress = 0;
  while (digitalRead(buttonPin) == HIGH){
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

bool checkSignal()
{
      //TODO: EXIT OUT OF THESE LOOPS WHEN BUTTON IS PRESSED
      
      if(Serial.available() == 0) // not connected to computer
      { 
        lcd.setCursor(0, 1);
        lcd.print("No signal");
        int timer = 6;
        while(timer >= 0)
        {
          lcd.print(".");
          timer--;
          if(buttonIsPressed(PUSHBUTTON_PIN))
          {
            return false;
          }
          delay(1000);
          
          if(Serial.available() != 0) 
          {
            lcd.clear();
            lcd.print("Signal found");
            lcd.setCursor(0,1);
            lcd.print("Connecting");
            for(int i = 0; i < 3; i++)
            {
              lcd.print(".");
              delay(1000);
            }
            return true;
          }
        }
        
        if(Serial.available() == 0) // if still not connected, move to next state
        {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("NO SIGNAL FOUND:");
          lcd.setCursor(0,1);
          lcd.print("CHANGING MODE");
          for(int i = 0; i < 4; i++)
          {
            lcd.print(".");
            delay(1000);
          }
          return false;
      }
    }
    else
      return true;
}

void mode0()
{
    lcd.setCursor(0, 0);
    lcd.print("1: BG Lighting");

    if(checkSignal() == false)
      goto nextstate1;

    lcd.clear();
    while(!buttonIsPressed(PUSHBUTTON_PIN))
    { 
    lcd.setCursor(0, 0);
    lcd.print("1: BG Lighting");
    //protocol expects data in format of 4 bytes
    //(xff) as a marker to ensure proper synchronization always
    //followed by red, green, blue bytes
    if (Serial.available() >= 4) {
      if(Serial.read() == 0xff){
        red = Serial.read();
        green= Serial.read();
        blue = Serial.read();
      }
    }
    // finally control led brightness through pulse-width modulation
    analogWrite(REDPIN, red);
    analogWrite(GREENPIN, green);
    analogWrite(BLUEPIN, blue);
  }
  nextstate1: state++;
}

void mode1()
{
  int FADESPEED = 5;
  while(!buttonIsPressed(PUSHBUTTON_PIN))
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("2:   Color");
    lcd.setCursor(5, 1);
    lcd.print("Swirl");
      int r = 0;
      int g = 0;
      int b = 0;
       //fade from blue to violet
      for (r = 0; r < 256; r++) { 
        if(buttonIsPressed(PUSHBUTTON_PIN))
          goto exitfunction;
          
        analogWrite(REDPIN, r);
        delay(FADESPEED);
      } 
       //fade from violet to red
      for (b = 255; b > 0; b--) { 
        if(buttonIsPressed(PUSHBUTTON_PIN))
          goto exitfunction;
          
        analogWrite(BLUEPIN, b);
        delay(FADESPEED);
      } 
      // fade from red to yellow
      for (g = 0; g < 256; g++) { 
        if(buttonIsPressed(PUSHBUTTON_PIN))
          goto exitfunction;
          
        analogWrite(GREENPIN, g);
        delay(FADESPEED);
      } 
      // fade from yellow to green
      for (r = 255; r > 0; r--) { 
        if(buttonIsPressed(PUSHBUTTON_PIN))
          goto exitfunction;
          
        analogWrite(REDPIN, r);
        delay(FADESPEED);
      } 
      // fade from green to teal
      for (b = 0; b < 256; b++) { 
        if(buttonIsPressed(PUSHBUTTON_PIN))
          goto exitfunction;
          
        analogWrite(BLUEPIN, b);
        delay(FADESPEED);
      } 
      // fade from teal to blue
      for (g = 255; g > 0; g--) { 
        if(buttonIsPressed(PUSHBUTTON_PIN))
          goto exitfunction;
          
        analogWrite(GREENPIN, g);
        delay(FADESPEED);
      } 
  }
  exitfunction: state++;
}

void mode2()
{
  float scale = 255.0/1024;
  int xpin = A1;    // select the input pin for the potentiometer
  int ypin = A0;
  int pushpin = 13;
  pinMode(xpin, INPUT);
  pinMode(ypin, INPUT);
  pinMode(pushpin, INPUT);
  
  int X = 0; 
  int Y = 0;
  int Z = 0;
  
  while(!buttonIsPressed(PUSHBUTTON_PIN))
  {
    lcd.setCursor(0, 0);
    lcd.print("3: Joystick");
    X = analogRead(ypin);
    Y = analogRead(xpin);
    Z = digitalRead(pushpin);
  
    X = ((float) X * scale);
    Y = ((float) Y * scale);
    Z = !Z; // Joystick switch is asserted when not pushed

    if(Z) {
        analogWrite(REDPIN, 0);
        analogWrite(GREENPIN, 0);
        analogWrite(BLUEPIN, 0);
    }
    else {
      if(X < 10) // magenta to blue
      {
          analogWrite(REDPIN, Y);
          analogWrite(GREENPIN, 0);
          analogWrite(BLUEPIN, 255);
      }
      else if(Y < 10) // bottom third of wheel
      {
        if(X < 125) // from blue to cyan
        {
          analogWrite(REDPIN, 0);
          analogWrite(GREENPIN, 2*X);
          analogWrite(BLUEPIN, 255);
        }
        else // from cyan to green
        {
          analogWrite(REDPIN, 0);
          analogWrite(GREENPIN, 255);
          analogWrite(BLUEPIN, 255 - X);
        }
      }
      else if(X > 245) // left third of wheel
      {
        if(Y < 255) // from magenta to blue
        {
          analogWrite(REDPIN, Y);
          analogWrite(GREENPIN, 255);
          analogWrite(BLUEPIN, 0);
        }
      }
      else if(Y > 245) // top third
      {
        if(X > 119) // right half of top third
        {
          analogWrite(REDPIN, 255);
          analogWrite(GREENPIN, (X - 120)*2);   
          analogWrite(BLUEPIN, 0);
        }
        else
        {
          analogWrite(REDPIN, 255);
          analogWrite(GREENPIN, 0);
          analogWrite(BLUEPIN, 255 - 2*X);
        }
      }
    } 
    
    Serial.print(X);
    Serial.print("     ");
    Serial.println(Y);
    lcd.setCursor(0, 1);
    lcd.print("X:");
    lcd.print(X);
    lcd.print(" Y:");
    lcd.print(Y);
    lcd.print(" Z:");
    lcd.print(Z);
    delay(10);
    lcd.clear();
  } 
  state = 0;
}
