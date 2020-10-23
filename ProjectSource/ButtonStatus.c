
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "ButtonStatus.h"

/*---------------------------- Module Variables ---------------------------*/
static uint8_t LastButtonState;

void InitButtonStatus (void)
{
    TRISBbits.TRISB5 = 1; //set RB5 to input
    LastButtonState = PORTBbits.RB5; //read button status from RB4
}

bool Check4ButtonEvent(void)
{
  uint8_t CurrentButtonState;
  bool ReturnVal = false;

  CurrentButtonState = LastButtonState = PORTBbits.RB5; //read button status from RB4
  // check for pin high or low AND different from last time
  if (CurrentButtonState != LastButtonState) {
      ReturnVal = true;
      if (CurrentButtonState == 0) {
          ES_Event_t ThisEvent;
          ThisEvent.EventType   = ES_BUTTON_DOWN;
          ThisEvent.EventParam  = 0;
          ES_PostAll(ThisEvent);
      }
      else {
          ES_Event_t ThisEvent;
          ThisEvent.EventType   = ES_BUTTON_UP;
          ThisEvent.EventParam  = 1;
          ES_PostAll(ThisEvent);
      }
  }
  LastButtonState = CurrentButtonState; // update the state for next time

  return ReturnVal;
}

