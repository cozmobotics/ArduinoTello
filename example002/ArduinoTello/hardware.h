// describe your hardware here 
#define PIN_LED 2             // where a LED is connected
#define PIN_SWITCH 3          // connect a toggle switch here

// where pushbuttons are connected 
#define PIN_TRIM_R      15
#define PIN_TRIM_L      14
#define PIN_PUSH_R      16
#define PIN_TRIM_U      10
#define PIN_TRIM_D       9
#define PIN_PUSH_L       8

// potentiometrs of the joysticks connected to analog pins 
#define PIN_ROLL    A0  // left-right
#define PIN_PITCH   A1  // forward-back
#define PIN_GAS     A2  // up-down
#define PIN_YAW     A3  // cw-ccw

#define NUM_AXES  4

#define VOLT_MAX  1023  // max. voltage of ADC. Roughly 700 if you power the controller with 3V, 1023 if powered with 5V
#define DEBOUNCE_MS 100 // milliseconds to wait before we can read a button again after a change
