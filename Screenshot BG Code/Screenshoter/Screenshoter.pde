import java.awt.Robot; //java library that lets us take screenshots //<>//
import java.awt.AWTException;
import java.awt.event.InputEvent;
import java.awt.image.BufferedImage;
import java.awt.Rectangle;
import java.awt.Dimension;
import processing.serial.*; //library for serial communication

Serial port; //creates object "port" of serial class
Robot robby; //creates object "robby" of robot class

final int WIDTH = 1368; // screen width in pixels
final int HEIGHT = 928; // screen height in pixels
final int NUM_THREADS = 5; // Number of threads to be created

// Some calculations we only need once
int area = WIDTH * HEIGHT;
int numPixels = area / NUM_THREADS;
// This will give you the j step
int numRows = numPixels / WIDTH;

// This will be a global to hold our RGB values
// Note each thread gets its own space to work on (this is very important)
float[][] rgbVals = new float[NUM_THREADS][3];

public class Worker implements Runnable {
  final private int startI;
  final private int startJ;
  final private int endI;
  final private int endJ;
  final private int thread;
  final private BufferedImage screenshot;

  // Constructor 
  public Worker(int startJ, int endJ, int thread, BufferedImage screenshot) {
    this.thread = thread;
    this.startI = 0;
    this.startJ = startJ;
    this.endI = WIDTH;
    this.endJ = endJ;
    this.screenshot = screenshot;
  }
  
  // This method will be the worker function that each thread will run in parallel
  public void run() {
    for (int i = startI; i < endI; i++) 
    {
      for (int j = startJ; j < endJ; j++)
      {
        int pixel = screenshot.getRGB(i, j); // The ARGB integer has the colors of pixel (i,j)
        rgbVals[thread][0] += (int)(255&(pixel>>16)); // Add up reds
        rgbVals[thread][1] += (int)(255&(pixel>>8)); // Add up greens
        rgbVals[thread][2] += (int)(255&(pixel)); // Add up blues
      }
    }
  }
}

void setup()
{
  // TODO: Make this more dynamic (depends on the hardware COM port right now)
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
  // Clear the buffer
  rgbVals = new float[NUM_THREADS][3];

  // Array of thread values
  Thread myThreads[] = new Thread[NUM_THREADS];

  BufferedImage screenshot = robby.createScreenCapture(new Rectangle(new Dimension(WIDTH, HEIGHT)));

  int startJ = 0, endJ = 0;
  // Spawn off our threads while dividing the work
  for (int i = 0; i < NUM_THREADS - 1; i++) {
    endJ += numRows;
    myThreads[i] = new Thread(new Worker(startJ, endJ, i, screenshot));
    myThreads[i].start();
    startJ = endJ + 1;
  }

  // Run the last thread with whatever is left over
  myThreads[NUM_THREADS - 1] = new Thread(new Worker(startJ, HEIGHT, NUM_THREADS - 1, screenshot));
  myThreads[NUM_THREADS - 1].start();

  // And wait for them to all finish
  for (int i = 0; i < NUM_THREADS - 1; i++) {
    // Attempt to join the threads
    try {
      myThreads[i].join();
    }
    // For some reason, we need this or java will be unhappy
    catch (Throwable err) {
      System.out.println(err);
    }
  }

  try {
    myThreads[NUM_THREADS - 1].join();
  }
  catch(Throwable err) {
    System.out.println(err);
  }

  float r = 0, g = 0, b = 0;
  for (int i = 0; i < NUM_THREADS; i++) {
    // Now we need to add each thread's rgb values
    r += rgbVals[i][0];
    g += rgbVals[i][1];
    b += rgbVals[i][2];
  }

  r /= area; // Average red
  g /= area; // Average green
  b /= area; // Average blue

  port.write(0xff); // Write marker (0xff) for synchronization
  port.write((byte)(r)); // Write red value
  port.write((byte)(g)); // Write green value
  port.write((byte)(b)); // Write blue value

  background(r, g, b); // Make window background average color
}