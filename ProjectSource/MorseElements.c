/****************************************************************************
 Module
   MorseElements.c

 Revision
   1.0.1

 Description
   This is a file for implementing the MorseElements Service under the
   Gen2 Events and Services Framework.

 Notes

****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "MorseElements.h"

/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;

static MorseElementState_t CurrentState;

static uint8_t TimeOfLastRise;
static uint8_t TimeOfLastFall;
static uint8_t LengthOfDot;
static uint8_t FirstDelta;
static uint8_t LastInputState;


/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitMorseElementsSM

 Parameters
     uint8_t : the priority of this service

 Returns
     bool, false if error in initialization, true otherwise

 Description
     Saves away the priority, and does any
     other required initialization for this service
 Notes

 Author
     J. Edward Carryer, 01/16/12, 10:00
****************************************************************************/
bool InitMorseElementsSM(uint8_t Priority)
{
  ES_Event_t ThisEvent;

  MyPriority = Priority;
  CurrentState = InitMorseElements;
  /********************************************
   Initialize the port line to receive Morse Code
   Sample port line and use it to initialize LastInputState
   Set FirstDelta to 0
   Call InitButtonStatus to initialize local var in button checking module
   *******************************************/
  
  // post the initial transition event
  ThisEvent.EventType = ES_INIT;
  if (ES_PostToService(MyPriority, ThisEvent) == true)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/****************************************************************************
 Function
     PostTemplateService

 Parameters
     EF_Event_t ThisEvent ,the event to post to the queue

 Returns
     bool false if the Enqueue operation failed, true otherwise

 Description
     Posts an event to this state machine's queue
 Notes

 Author
     J. Edward Carryer, 10/23/11, 19:25
****************************************************************************/
bool PostMorseElementsSM(ES_Event_t ThisEvent)
{
  return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunTemplateService

 Parameters
   ES_Event_t : the event to process

 Returns
   ES_Event, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
   add your description here
 Notes

 Author
   J. Edward Carryer, 01/15/12, 15:23
****************************************************************************/
ES_Event_t RunMorseElementsSM(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
  switch (CurrentState)          
  { 
      case InitMorseElements: 
          switch (ThisEvent.EventType)
          {
            case ES_INIT:
            {
              puts("Service 01:");
              printf("\rES_INIT received in Service %d\r\n", MyPriority);
            }
            break;
            case ES_RISING_EDGE:   // announce rise event
            {
              printf("R ");
            }
            break;
            case ES_FALLING_EDGE:   // announce fall event
            {
              printf("F ");
            }
            break;
            default:
            {}
             break;
          }
      break;
    
      default:
      {}
      break;
  }

  return ReturnEvent;
}

/***************************************************************************
 private functions
 ***************************************************************************/
bool Check4MorseEvent(void)
{
  bool returnVal = false;
  uint8_t CurrentInputState;// = readPinState();
  
  if (CurrentInputState != LastInputState)
  {
      if (CurrentInputState == 0) 
      {
        ES_Event_t ThisEvent;
        ThisEvent.EventType   = ES_RISING_EDGE;
        ES_PostAll(ThisEvent);
        returnVal = true;
      }
      else
      {
        ES_Event_t ThisEvent;
        ThisEvent.EventType   = ES_FALLING_EDGE;
        ES_PostAll(ThisEvent);
        returnVal = true;
      }
  
  LastInputState = CurrentInputState;
  
  return returnVal;
  }
}

/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/

