import java.awt.Robot; //java library that lets us take screenshots
import java.awt.AWTException;
import java.awt.event.InputEvent;
import java.awt.image.BufferedImage;
import java.awt.Rectangle;
import java.awt.Dimension;
import processing.serial.*; //library for serial communication

Serial port; //creates object "port" of serial class
Robot robby; //creates object "robby" of robot class

int PRECISION = 5; // lower this to make more precise and less efficient
int WIDTH = 1368; // screen width in pixels
int HEIGHT = 928; // screen height in pixels

void setup()
{
  String portName = Serial.list()[1];
  port = new Serial(this, portName, 9600); //set baud rate
  size(100, 100); //window size (doesn't matter)
  try 
  {
    robby = new Robot();
  }
  catch (AWTException e)
  {
    println("Robot class not supported by your system!");
    exit();
  }
}

void draw()
{
  int pixel; //ARGB variable with 32 int bytes where
  //sets of 8 bytes are: Alpha, Red, Green, Blue
  float r=0;
  float g=0;
  float b=0;
  
  //get screenshot into object "screenshot" of class BufferedImage
  BufferedImage screenshot = robby.createScreenCapture(new Rectangle(new Dimension(WIDTH, HEIGHT)));
  //1368*928 is the screen resolution
  
  
  int i=0;
  int j=0;
  //1368*928
  //I skip every alternate pixel making my program 4 times faster
  for(i =0;i<WIDTH/PRECISION; i=i+1)
  {
    for(j=0; j<HEIGHT/PRECISION;j=j+1)
    {
      pixel = screenshot.getRGB(i,j); //the ARGB integer has the colors of pixel (i,j)
      r = r+(int)(255&(pixel>>16)); //add up reds
      g = g+(int)(255&(pixel>>8)); //add up greens
      b = b+(int)(255&(pixel)); //add up blues
    }
  }
  
  for(i = (WIDTH/PRECISION)*(PRECISION - 1); i<WIDTH; i = i+1)
  {
    for(j= (HEIGHT/PRECISION)*(PRECISION - 1); j<HEIGHT; j = j+1)
    {
      pixel = screenshot.getRGB(i,j); //the ARGB integer has the colors of pixel (i,j)
      r = r+(int)(255&(pixel>>16)); //add up reds
      g = g+(int)(255&(pixel>>8)); //add up greens
      b = b+(int)(255&(pixel)); //add up blues
    }
  }
  r = r/((WIDTH/2)*(HEIGHT/2)); //average red (skipping every alternate pixel)
  g = g/((WIDTH/2)*(HEIGHT/2)); //average green
  b = b/((WIDTH/2)*(HEIGHT/2)); //average blue
  
  
  port.write(0xff); //write marker (0xff) for synchronization
  port.write((byte)(r)); //write red value
  port.write((byte)(g)); //write green value
  port.write((byte)(b)); //write blue value
  
  background(r,g,b); //make window background average color
}