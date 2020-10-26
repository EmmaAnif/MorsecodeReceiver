
/*----------------------------- Include Files -----------------------------*/
/* include header files for ButtonStatus as well as any other relevant framework
 header files */
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "ButtonStatus.h"
#include "HAL.h" //hardware abstraction layer

/*---------------------------- Module Variables ---------------------------*/
static uint8_t LastButtonState;

void InitButtonStatus (void)
{
    pinMode(4,1); //set RA3 to input
    LastButtonState = digitalRead(4); //read button status from RA3 and use it to initialize LastButtonstate
}

bool Check4ButtonEvent(void)
{
  uint8_t CurrentButtonState;
  bool ReturnVal = false; //assume no button event initially

  CurrentButtonState = digitalRead(4); //read button status from RA3

  // check for pin high or low AND different from last time
  if (CurrentButtonState != LastButtonState) {
      if (CurrentButtonState == 0) {
          ES_Event_t ThisEvent;
          ThisEvent.EventType = ES_BUTTON_DOWN;
          ThisEvent.EventParam  = 0;
          ES_PostAll(ThisEvent); //post to all services in the framework
          ReturnVal = true;
      }
  }
  LastButtonState = CurrentButtonState; // update the state for next time

  return ReturnVal;
}

