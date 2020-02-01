/*
 * 
*/

#define PPM_IN 11
#define NUM_IN_CHANNELS 8
#define BAUDRATE 115200
#define POLARITY HIGH

typedef struct
{
  int IndexInChannel;
  bool Invert;
  int Min;
  int Max;
} tOutChannel;

tOutChannel OutChannel[5] =
{
  { 0,  false,  680,  1500},
  { 2,  true,   680,  1500},
  { 1,  false,  680,  1500},
  { 3,  false,  680,  1500},
  { 4,  false,  680,  1500}
};

int InChannel[NUM_IN_CHANNELS];


/***********************************************************************************/
int toScale(int Index)
{
  int Val;
  int Raw;
  
  Raw = InChannel[OutChannel[Index].IndexInChannel];
  Val = map (Raw, OutChannel[Index].Min, OutChannel[Index].Max, -100, 100);
  if (Val > 100) Val = 100;
  if (Val <-100) Val =-100;
  if (OutChannel[Index].Invert)
  {
    Val = -Val;
  } // Invert
  return (Val);
} // toScale

/***********************************************************************************/
void setup()
{
  Serial.begin (BAUDRATE);
  pinMode (PPM_IN, INPUT);
} // setup

/***********************************************************************************/

typedef  enum {WAIT_FOR_PAUSE, COUNTING, FINISHED, TIMEOUT} tStatus;

/***********************************************************************************/
void loop()
{
  unsigned long int PulsLength;
  static int Counter;
  int CountTello;
  String OutString;
  static tStatus Status = WAIT_FOR_PAUSE;
  static bool Landing = true;

  PulsLength = pulseIn (PPM_IN, POLARITY, 30000);
  if (PulsLength > 0)
  {

    if (PulsLength > 5000)
    {
      switch (Status)
      {
        case WAIT_FOR_PAUSE:
        {
          //Serial.print ("\n");
          Counter = 0;
          Status = COUNTING;
        } // WAIT_FOR_PAUSE
        break;
        
        case COUNTING:
          {
            Status = FINISHED;
          }
          break;
      } // switch
    } // pause

    else 
    {
      if (Status == COUNTING)
      {
        //      Serial.print (String(Counter+1) + ":" + String (PulsLength) + "\t");
        if (Counter < NUM_IN_CHANNELS)
        {
          InChannel[Counter] = PulsLength;
          //Serial.print (String(Counter + 1) + ":" + String (InChannel[Counter]) + "\t");
        } // Counter OK
  
        Counter ++;
      }  // COUNTING
    }

    if (Status == TIMEOUT)
    {
      Status = WAIT_FOR_PAUSE;
      Serial.println ("command");
    } // TIMEOUT


  } // pulse
  else
  {
    if (Status != TIMEOUT)
    {
      Serial.println ("land");
      Status = TIMEOUT;
    }
  } // no pulse

  if (Status == FINISHED)
  {
    if (!Landing)
    {
      OutString = "rc";
      for (CountTello=0; CountTello<4; CountTello++)
      {
        OutString = OutString + (" " + String(int(toScale(CountTello))));
      }
      Serial.println (OutString);
      delay (100);
    } // !Landing
    Status = WAIT_FOR_PAUSE;
  } // STATUS == DONE

  if (toScale(4) < -50)
  {
    if (!Landing)
    {
      Serial.println ("land");
      Landing = true;
    }
  } 
  else
  {
    if (Landing) 
    {
      Serial.println ("command");
      Landing = false;
    }  
  }





  
} // loop
