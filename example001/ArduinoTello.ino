
/* A minimal program to fly a Tello Quadrocopter with a controller connected to the USB
  Xou need some software on the PC, too. */ 

#include <string.h>


// push-buttons connected to digital pins 
#define PIN_LAND    12

// potentiometrs of the joysticks connected to analog pins 
#define PIN_ROLL    A4  // left-right
#define PIN_PITCH   A5  // forward-back
#define PIN_GAS     A6  // up-down
#define PIN_YAW     A7  // cw-ccw

#define VOLT_MAX  1023  // roughly 700 if you power the controller with 3V, 1023 if powered with 5V
#define HEARTBEAT  100  // milliseconds between rc commands
#define TIMEOUT_SEC 10  // seconds of inactivity before a keepalive signel ("command") is sent
#define TIMEOUT   (TIMEOUT_SEC * 1000 / 100)  // timeout counter preset 

void setup() 
{
  pinMode(PIN_LAND, INPUT);
  digitalWrite (PIN_LAND, HIGH);    // internal pullup (remove this line if you have a pullup in your circuit)
  Serial.begin(115200); // set the baud rate
  Serial.println("command"); 
}
void loop() 
{
  static bool LandingRequested = true;
  static String OutStringOld = "";  
  String OutString;
  static int Timeout = TIMEOUT;
  int Axes[4];
  int Count;
  int Pins[4] = {PIN_ROLL, PIN_PITCH, PIN_GAS, PIN_YAW};

  if (digitalRead(PIN_LAND) == 0) 
  {
    if (LandingRequested)
    {
      Serial.println ("command");
      Timeout = TIMEOUT;
      delay (1000);
      LandingRequested = false;
    }
    else
    {
      Serial.println ("land");
      Timeout = TIMEOUT;
      delay (1000);
      LandingRequested = true;
    }
  } // land pin pressed 
  
  if (not LandingRequested)
  {
    OutString = "rc";
    for (Count=0; Count < 4; Count++) 
    {
      Axes[Count] = analogRead(Pins[Count]);
      Axes[Count] = (long int)(Axes[Count] - VOLT_MAX / 2) * 200 / VOLT_MAX;  // note the long int, otherwise we get garbage
      //Axes[Count] = map (Axes[Count], 0, VOLT_MAX, -100, 100);             // this does the same 
      OutString = OutString + " " + String(Axes[Count]);
    } // all axes 
  
    // see if values have changed and send if so
    if (OutString != OutStringOld) 
    {
      Serial.println (OutString);
      Timeout = TIMEOUT;
      OutStringOld = OutString; 
    }
  } // not landed
  
  // see if we run into a timeout and sen a keepalive signal 
  if (Timeout > 0)
  {
    Timeout--;
  }
  else
  {
      Serial.println ("command");
      Timeout = TIMEOUT;
  }
  
  
  delay (100);
}
