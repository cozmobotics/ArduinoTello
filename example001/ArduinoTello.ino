
/* A minimal program to fly a Tello Quadrocopter with a controller connected to the USB
  Xou need some software on the PC, too. */ 

#include <string.h>


// push-button connected to a digital pin 
#define PIN_LAND    12  

// potentiometrs of the joysticks connected to analog pins 
#define PIN_ROLL    A4  // left-right
#define PIN_PITCH   A5  // forward-back
#define PIN_GAS     A6  // up-down
#define PIN_YAW     A7  // cw-ccw


#define PIN_MAX     700 // roughly 700 if you power the controller with 3V, 1023 if powered with 5V

void setup() 
{
  pinMode(PIN_LAND, INPUT);
  Serial.begin(115200 ); // set the baud rate
  Serial.println("command"); 
}
void loop() 
{
  static bool LandingRequested = false;
  static String OutStringOld = "";  
  String OutString;
  int Axes[4];
  int Count;
  int Pins[4];

  //Pins = {PIN_ROLL, PIN_PITCH, PIN__GAS, PIN_YAW};  // in standard-C this should work, on Arduino it doesn*t. 
  Pins[0] = PIN_ROLL;
  Pins[1] = PIN_PITCH;
  Pins[2] = PIN_GAS;
  Pins[3] = PIN_YAW;

  if (digitalRead(PIN_LAND) == 0) 
  {
    if (not LandingRequested)
    {
      Serial.println ("land");
    }
    LandingRequested = true;
    
  }
  
  //Axes[0] = analogRead(PIN_ROLL);
  //Axes[1] = analogRead(PIN_PITCH);
  //Axes[2] = analogRead(PIN_GAS);
  //Axes[3] = analogRead(PIN_YAW);

  if (not LandingRequested)
  {
    OutString = "rc";
    for (Count=0; Count < 4; Count++) 
    {
      Axes[Count] = analogRead(Pins[Count]);
      //Axes[Count] = (int)(Axes[Count] - PIN_MAX / 2) * 200 / PIN_MAX;
      //Axes[Count] = (Axes[Count] - PIN_MAX / 2);
      Axes[Count] = map (Axes[Count], 0, PIN_MAX, -100, 100);
      OutString = OutString + " " + String(Axes[Count]);
    }
  
    if (OutString != OutStringOld) 
    {
      Serial.println (OutString);
    }
    OutStringOld = OutString; 
  } 
  delay (100);
}
