
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
    
    TRISAbits.TRISA3 = 1; //set RA3 to input
    LastButtonState = PORTAbits.RA3; //read button status from RA3
}

bool Check4ButtonEvent(void)
{
  uint8_t CurrentButtonState;
  bool ReturnVal = false;

  CurrentButtonState = PORTAbits.RA3; //read button status from RA3

  // check for pin high or low AND different from last time
  if (CurrentButtonState != LastButtonState) {
      
      printf("\r POST BUTTON_DOWN\r\n");
      
      if (CurrentButtonState == 0) {
          ES_Event_t ThisEvent;
          ThisEvent.EventType = ES_BUTTON_DOWN;
          ThisEvent.EventParam  = 0;
          ES_PostAll(ThisEvent);
          ReturnVal = true;
      }
  }
  LastButtonState = CurrentButtonState; // update the state for next time

  return ReturnVal;
}

