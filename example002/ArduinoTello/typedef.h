typedef struct
{
  bool LandingRequested;
  long int WaitUntil; 
  int LedCount;
  int LedDutyCycle;
  bool Hover;
} tMachineState;

typedef struct 
{
  int CountPressed;
  bool  PressedNot;
  bool  PressedOld;
  long int WaitUntil;
} tButtonState;

typedef struct 
{
  int Pin;  // which pin is connected to the button
  String Text[2];
  bool  UpAndDown;
  void  (*Func)(tButtonState *MyButtonState, tMachineState *MyMachineState);
} tButtonDesc; 

#define UP_AND_DOWN true
#define DOWN_ONLY false

typedef struct
{
  int Pin;
  int Reading; 
  int ReadingOld;  
  int Offset;
  long int Time;
} tAxesState;
