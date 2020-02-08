
/* A program to fly a Tello Quadrocopter with a controller connected to the USB
  You need some software on the PC, too. */ 

#include <string.h>
//#include <math.h>
#include "typedef.h"
#include "hardware.h"

#define MAX_NUM_BUTTONS 10
#define HEARTBEAT  100  // milliseconds between rc commands
#define LED_PERIOD 10   // HEARTBEAT * LED_PERIOD = period for LED blink
#define BAUDRATE    115200


tButtonState ButtonState[MAX_NUM_BUTTONS];
tMachineState MachineState;
int Pins[4] = {PIN_ROLL, PIN_PITCH, PIN_GAS, PIN_YAW};
tAxesState AxesState[NUM_AXES];

/***********************************************************************************************/
int toScale (int InValue)
{
  long int OutValue;

  OutValue = (long int)(InValue - VOLT_MAX / 2) * 200 / VOLT_MAX;  // note the long int, otherwise we get garbage
  return (OutValue);
}
/**************************************************************************************************************/
void funcButtonLand (tButtonState *MyButtonState, tMachineState *MyMachineState)
{
  //Serial.println("hello LandButton!");
  if (MyButtonState->CountPressed % 2)
  {
    MachineState.LandingRequested = false;
    MachineState.LedDutyCycle = 1;
  }
  else
  {
    MachineState.LandingRequested = true;
    MachineState.LedDutyCycle = (int)(LED_PERIOD / 2);
  }
  MachineState.LedCount = 0; // start a new cycle
} // funcButtonLand

/**************************************************************************************************************/
void funcHoverSwitch (tButtonState *MyButtonState, tMachineState *MyMachineState)
{
  //Serial.println("hello HoverSwitch!");
  if (MyButtonState->PressedNot) 
  {
    MachineState.Hover = true;
  }
  else
  {
    MachineState.Hover = false;
  }
  
} // funcHoverSwitch

/**************************************************************************************************************/
tButtonDesc ButtonDesc[] = {
  {PIN_TRIM_R,  {"command",    "land"},       DOWN_ONLY, &funcButtonLand},
  {PIN_TRIM_L,  {"battery?",   "wifi?"},      DOWN_ONLY, 0},
  {PIN_PUSH_R,  {"emergency",  "emergency"},  DOWN_ONLY, 0},
  {PIN_TRIM_U,  {"flip f",     "flip b"},     DOWN_ONLY, 0},
  {PIN_TRIM_D,  {"flip l",     "flip r"},     DOWN_ONLY, 0},
  {PIN_PUSH_L,  {"takeoff",    "takeoff"},    DOWN_ONLY, 0},
  {PIN_SWITCH,  {"",   ""},                   UP_AND_DOWN, funcHoverSwitch}
  };

/**************************************************************************************************************/
void setup() 
{
  int NumButtons; 
  int Count;
  
  Serial.begin(BAUDRATE); // set the baud rate
  Serial.println("command"); 
  

  // initialize the buttons
  NumButtons = sizeof(ButtonDesc) / sizeof(ButtonDesc[0]);
  for (Count = 0; Count < NumButtons; Count++)
  {
    Serial.println ("Initializing Pin " + String(ButtonDesc[Count].Pin));
    pinMode(ButtonDesc[Count].Pin, INPUT);
    digitalWrite (ButtonDesc[Count].Pin, HIGH);    // internal pullup (remove this line if you have a pullup in your circuit)
  }

  memset (ButtonState, sizeof(ButtonState), 0);

  for (Count = 0; Count < NumButtons; Count++)
  {
    ButtonState[Count].PressedOld = ButtonState[Count].PressedNot;  // set old state so we do not fire all messeges when we start the loop 
    ButtonState[Count].WaitUntil = millis() + DEBOUNCE_MS;
  }

  // take the readings at startup and later subtract them from the actual readings 
  // +++ does not yet work. Need to map them to -100...100 before
  for (Count=0; Count < 4; Count++) 
//    if (Pins[Count] != PIN_GAS)
//      AxesState[Count].Offset = analogRead(Pins[Count]);
//    else
      AxesState[Count].Offset = 0;
  
  
  MachineState.LandingRequested = true;
  MachineState.WaitUntil = millis() + HEARTBEAT;
  MachineState.LedCount = 0;
  MachineState.LedDutyCycle = (int)(LED_PERIOD / 2);
  MachineState.Hover = false;
  
  pinMode (PIN_LED, OUTPUT);
} // setup


/**************************************************************************************************************/
void loop() 
{
  static String OutStringOld = "";  
  String OutString;
  int Axes[4];
  int Count;
  int NumButtons; 
  long int TimeNow; 
  bool Changed;

  TimeNow = millis();

  if (TimeNow >= MachineState.WaitUntil)
  {
    // handle LED
    // **********
    if (MachineState.LedCount == 0)
      digitalWrite (PIN_LED, true);

    if (MachineState.LedCount >= MachineState.LedDutyCycle)
      digitalWrite (PIN_LED, false);
  
    MachineState.LedCount++;
    if (MachineState.LedCount >= LED_PERIOD)
      MachineState.LedCount = 0;
    
    // handle joysticks
    // ****************
    if (not MachineState.LandingRequested)
    {
      Changed = false;
      OutString = "rc";
      for (Count=0; Count < 4; Count++) 
      {
        AxesState[Count].Reading = analogRead(Pins[Count]) - AxesState[Count].Offset;
        AxesState[Count].Reading = (long int)(AxesState[Count].Reading - VOLT_MAX / 2) * 200 / VOLT_MAX;  // note the long int, otherwise we get garbage
        //AxesState[Count].Reading = map (AxesState[Count].Reading, 0, VOLT_MAX, -100, 100);             // this does the same 
        
        if (MachineState.Hover && (Pins[Count] == PIN_GAS))
          AxesState[Count].Reading = 0;

        if (abs (AxesState[Count].ReadingOld - AxesState[Count].Reading) > 2)
          Changed = true;

        AxesState[Count].ReadingOld = AxesState[Count].Reading;

        // +++ we could do the expo here 
        
        OutString = OutString + " " + String(AxesState[Count].Reading); 
        
      } // all axes 
    
      // see if values have changed and send if so
      if (Changed) 
      {
        Serial.println (OutString);
      }
    } // not landed


    // Set timeout
    MachineState.WaitUntil += HEARTBEAT;
  }
  
  // handle buttons
  // **************
  NumButtons = sizeof(ButtonDesc) / sizeof(ButtonDesc[0]);
  for (Count = 0; Count < NumButtons; Count++)
  {
    if (TimeNow >= ButtonState[Count].WaitUntil)
    {
      
      ButtonState[Count].PressedNot = (bool)digitalRead(ButtonDesc[Count].Pin);
      if (ButtonState[Count].PressedNot != ButtonState[Count].PressedOld)
      {
        if((!ButtonState[Count].PressedNot) || ButtonDesc[Count].UpAndDown)  // when it is pressed (not released) or it is active on press and release (switch)  
        {
          Serial.println (ButtonDesc[Count].Text[ButtonState[Count].CountPressed % 2]);
          ButtonState[Count].CountPressed++;

          if (ButtonDesc[Count].Func != 0)
            ButtonDesc[Count].Func(&ButtonState[Count], &MachineState);
        }        
        ButtonState[Count].WaitUntil = TimeNow + DEBOUNCE_MS;
      } // changed
    } // timeout
    ButtonState[Count].PressedOld = ButtonState[Count].PressedNot;
  } // all buttons  
} // loop
