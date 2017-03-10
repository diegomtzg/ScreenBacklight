import java.awt.Robot; //<>//
import java.awt.AWTException;
import java.awt.event.InputEvent;
import java.awt.image.DataBufferInt;
import java.awt.image.BufferedImage;
import java.awt.Rectangle;
import java.awt.Dimension;
import processing.serial.*;

Serial port; // Creates object "port" of serial class
Robot robby; // Creates object "robby" of robot class

final int WIDTH = 1368; // Screen width in pixels
final int HEIGHT = 928; // Screen height in pixels

// We only need to create these objects once
Dimension dim = new Dimension(WIDTH, HEIGHT);
Rectangle rec = new Rectangle(dim);
BufferedImage screenshot;

// Some calculations we only need once
int area = WIDTH * HEIGHT;

void setup()
{
  // Exit as this is a fatal error
  String[] list = Serial.list();
  if(list.length <= 0) {
    println("Failed to receive a serial port"); 
    exit();
  }
  String portName; 
  for(int i = 0; i < list.length; i++) {
    try {
      portName = list[i];
      port = new Serial(this, portName, 9600); // Set baud rate
      if(port != null) {
        break;
      }
    }
    catch (Throwable err) {
      println("Error opening port");
      exit();
    }
  }
  
  size(100, 100); // Sindow size (doesn't matter)
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
  // Capture the screen image (Buffered Image object)
  screenshot = robby.createScreenCapture(rec);

  // Now loop throught he pixel data all at once
  float r = 0, g = 0, b = 0;
  final int[] pixels = ((DataBufferInt) screenshot.getRaster().getDataBuffer()).getData();
  for (int i = 0; i < pixels.length; i++) {
    r = r+(int)(255&(pixels[i]>>16)); // Add up reds
    g = g+(int)(255&(pixels[i]>>8)); // Add up greens
    b = b+(int)(255&(pixels[i])); // Add up blues
  }
  
  // Write the data to the port
  port.write(0xff); // Write marker (0xff) for synchronization
  port.write((byte)(r)); // Write red value
  port.write((byte)(g)); // Write green value
  port.write((byte)(b)); // Write blue value
   
  // Average the values
  r /= area; // Average red
  g /= area; // Average green
  b /= area; // Average blue
  
  background(r, g, b); // Make window background average color
  
  // 10ms delay for improved performance
  delay(10);
}