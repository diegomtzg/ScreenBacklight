#define BLUEPIN 9
#define REDPIN 10
#define GREENPIN 11

int red, green, blue;

void setup()
{
  Serial.begin(9600);
  //initial values (no significance)
  int red = 255;
  int blue = 255;
  int green = 255;
}
void loop()
{
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
  analogWrite (REDPIN, red);
  analogWrite (GREENPIN, green);
  analogWrite (BLUEPIN, blue);
}
