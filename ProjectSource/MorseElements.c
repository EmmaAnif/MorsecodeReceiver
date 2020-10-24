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
#include "ES_DeferRecall.h"
#include "ButtonStatus.h"

/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;

static MorseElementState_t CurrentState;

static uint16_t TimeOfLastRise;
static uint16_t TimeOfLastFall;
static uint16_t LengthOfDot;
static uint16_t FirstDelta;
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
  //disable analog function on all pins
  ANSELA = 0;
  ANSELB = 0;
  TRISBbits.TRISB4 = 1; //set RB4 to input
  LastInputState = PORTBbits.RB4; //read from RB4
  FirstDelta = 0;
  InitButtonStatus();
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
  MorseElementState_t NextState;
  
  switch (CurrentState)          
  { 
      case InitMorseElements: 
          if (ThisEvent.EventType == ES_INIT)
          {
            puts("\nMorse Elements Service:");
            printf("\rES_INIT received in Service %d\r\n", MyPriority);
            NextState = CalWait4Rise;
          }
      break;
          
      case CalWait4Rise: 
          switch (ThisEvent.EventType)
          {
              case ES_RISING_EDGE:   // announce rise event
              {
                  printf("%c",'*');
                  TimeOfLastRise = ThisEvent.EventParam;
                  NextState = CalWait4Fall;
              }
              break;
              case ES_CAL_COMPLETED: 
              {
                  NextState = EOC_WaitRise;
              }
              break;
              default:
              {}
              break;
          }
      break;
      
      case CalWait4Fall: 
          if (ThisEvent.EventType == ES_FALLING_EDGE)
          {
            TimeOfLastFall = ThisEvent.EventParam;
            NextState = CalWait4Rise;
            TestCalibration();
          }
      break;
            
      case EOC_WaitRise: 
          switch (ThisEvent.EventType)
          {
            case ES_RISING_EDGE:   // announce rise event
            {
              TimeOfLastRise = ThisEvent.EventParam;
              NextState = EOC_WaitFall;
              CharacterizeSpace();
            }
            break;
            case ES_BUTTON_DOWN:   // announce button down event
            {
              NextState = CalWait4Rise;
              FirstDelta = 0;
            }
            break;
            default:
            {}
             break;
          }
      break;
      
      case EOC_WaitFall: 
          switch (ThisEvent.EventType)
          {
            case ES_FALLING_EDGE:   // announce rise event
            {
              TimeOfLastFall = ThisEvent.EventParam;
              NextState = EOC_WaitRise;
            }
            break;
            case ES_BUTTON_DOWN:   // announce button down event
            {
              NextState = CalWait4Rise;
              FirstDelta = 0;
            }
            break;
            case ES_EOC_DETECTED:  
            {
              NextState = DecodeWaitFall;
            }
            break;
            case ES_EOW_DETECTED:  
            {
              NextState = DecodeWaitFall;
            }
            break;
            default:
            {}
            break;
          }
      break;
      
      case DecodeWaitFall: 
          switch (ThisEvent.EventType)
          {
            case ES_FALLING_EDGE:   // announce fall event
            {
              TimeOfLastFall = ThisEvent.EventParam;
              NextState = DecodeWaitRise;
              CharacterizePulse();
            }
            break;
            case ES_BUTTON_DOWN:   // announce button down event
            {
              NextState = CalWait4Rise;
              FirstDelta = 0;
            }
            break;
            default:
            {}
            break;
          }
      break;
      
      case DecodeWaitRise: 
          switch (ThisEvent.EventType)
          {
            case ES_RISING_EDGE:   // announce rise event
            {
              TimeOfLastRise = ThisEvent.EventParam;
              NextState = DecodeWaitFall;
              CharacterizeSpace();
            }
            break;
            case ES_BUTTON_DOWN:   // announce button down event
            {
              NextState = CalWait4Rise;
              FirstDelta = 0;
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
  CurrentState = NextState;
  return ReturnEvent;
}

/***************************************************************************
 private functions
 ***************************************************************************/
bool Check4MorseEvent(void)
{
  bool returnVal = false;
  uint8_t CurrentInputState;
  CurrentInputState = PORTBbits.RB4;// readPinState();
  
  if (CurrentInputState != LastInputState)
  {
      if (CurrentInputState == 0) 
      {
        ES_Event_t ThisEvent;
        ThisEvent.EventType  = ES_FALLING_EDGE;
        ThisEvent.EventParam  = ES_Timer_GetTime();
        PostMorseElementsSM(ThisEvent);
        returnVal = true;
      }
      else
      {
        ES_Event_t ThisEvent;
        ThisEvent.EventType   = ES_RISING_EDGE;
        ThisEvent.EventParam  = ES_Timer_GetTime();
        PostMorseElementsSM(ThisEvent);
        returnVal = true;
      }
  
  LastInputState = CurrentInputState;
  
  return returnVal;
  }
}

void TestCalibration(void)
{
    uint16_t SecondDelta;
    if (FirstDelta == 0){
        FirstDelta = TimeOfLastFall - TimeOfLastRise;
    }
    else{
        SecondDelta = TimeOfLastFall - TimeOfLastRise;
        if ((100.0 * FirstDelta / SecondDelta) <= 33.33){
            LengthOfDot = FirstDelta;
            ES_Event_t ThisEvent;
            ThisEvent.EventType = ES_CAL_COMPLETED;
            PostMorseElementsSM(ThisEvent);
        }
        else if ((100.0 * FirstDelta / SecondDelta) > 300.0){
            LengthOfDot = SecondDelta;
            ES_Event_t ThisEvent;
            ThisEvent.EventType = ES_CAL_COMPLETED;
            PostMorseElementsSM(ThisEvent);
        }
        else {
            FirstDelta = SecondDelta;
        }
    }
    return;
}

void CharacterizeSpace(void)
{
  uint16_t LastInterval;
  ES_Event_t ThisEvent;
  LastInterval = TimeOfLastRise - TimeOfLastFall;
  if ((LastInterval != LengthOfDot) && (LastInterval != LengthOfDot+1))
  {
      if ((LastInterval >= LengthOfDot*3) && (LastInterval <= LengthOfDot*3 + 3))
      {
          ThisEvent.EventType = ES_EOC_DETECTED;
          ES_PostAll(ThisEvent);
      }
      else if ((LastInterval >= LengthOfDot*7) && (LastInterval <= LengthOfDot*7 + 7))
      {
          ThisEvent.EventType = ES_EOW_DETECTED;
          ES_PostAll(ThisEvent);
      }
      else{
          ThisEvent.EventType = ES_BAD_SPACE;
          ES_PostAll(ThisEvent);
      }
  }
  return;
}

void CharacterizePulse(void)
{
  uint16_t LastPulseWidth;
  ES_Event_t ThisEvent;
  LastPulseWidth = TimeOfLastFall - TimeOfLastRise;

  if ((LastPulseWidth == LengthOfDot) || (LastPulseWidth == LengthOfDot+1))
  {
      ThisEvent.EventType   = ES_DOT_DETECTED;
      ES_PostAll(ThisEvent);
  }
  else if ((LastPulseWidth >= LengthOfDot*3) && (LastPulseWidth <= LengthOfDot*3 + 3))
  {
      ThisEvent.EventType   = ES_DASH_DETECTED;
      ES_PostAll(ThisEvent);
  }
  else
  {
      ThisEvent.EventType   = ES_BAD_PULSE;
      ES_PostAll(ThisEvent);
  }
  return; 
}

/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/

