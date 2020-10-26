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
#include "MorseDecode.h"
#include "HAL.h" 

/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;

static MorseElementState_t CurrentState; //store current state of this machine

static uint16_t TimeOfLastRise; //time of rising edge
static uint16_t TimeOfLastFall; //time of falling edge
static uint16_t LengthOfDot; //store length of Dot
static uint16_t FirstDelta; //used in calibrating the Morse Code signal
static uint8_t LastInputState; //store last state of data input pin

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

  MyPriority = Priority; //initialize priority of this service
  CurrentState = InitMorseElements; //initialize current state as InitMorseElements
  //disable analog function on all pins
  ANSELA = 0;
  ANSELB = 0;
  pinMode(10,1); //set RB4 as data input
  LastInputState = digitalRead(10); //read from RB4 and use it to initialize LastInputState
  FirstDelta = 0; //initialize first delta as 0 
  InitButtonStatus(); //initialize the ButtonStatus module
  
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
     PostMorseElementsSM

 Parameters
     ES_Event_t ThisEvent ,the event to post to the queue

 Returns
     bool false if the Enqueue operation failed, true otherwise

 Description
     Posts an event to this state machine's queue
 Notes

****************************************************************************/
bool PostMorseElementsSM(ES_Event_t ThisEvent)
{
  return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunmorseElementsSM

 Parameters
   ES_Event_t : the event to process

 Returns
   ES_Event, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
   Calibrates MorseCode signal to detect dots and dashes then sends them to 
   MorseDecode service to be decoded to legal characters
 Notes

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
      
      // section where signal is calibrated
      case CalWait4Rise: 
          switch (ThisEvent.EventType)
          {
              case ES_RISING_EDGE:   // if Rising Edge
              {
                  TimeOfLastRise = ThisEvent.EventParam; //store the time
                  NextState = CalWait4Fall;
              }
              break;
              case ES_CAL_COMPLETED: // if calibration is complete
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
          if (ThisEvent.EventType == ES_FALLING_EDGE) //if Falling Edge
          {
            TimeOfLastFall = ThisEvent.EventParam; //store the time
            NextState = CalWait4Rise;
            TestCalibration(); // calibrate the signal received to get length of dot
          }
      break;
      
      //wait for end of character
      case EOC_WaitRise: 
          switch (ThisEvent.EventType)
          {
            case ES_RISING_EDGE:
            {
              TimeOfLastRise = ThisEvent.EventParam;
              NextState = EOC_WaitFall;
              CharacterizeSpace(); //characterize the space based on last fall and rise
            }
            break;
            case ES_BUTTON_DOWN: //go back to calibration mode
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
            case ES_FALLING_EDGE: 
            {
              TimeOfLastFall = ThisEvent.EventParam;
              NextState = EOC_WaitRise;
            }
            break;
            case ES_BUTTON_DOWN: // go back to calibration mode
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
      
      // loop that keeps decoding characters unless Button is pushed down
      case DecodeWaitFall: 
          switch (ThisEvent.EventType)
          {
            case ES_FALLING_EDGE: 
            {
              TimeOfLastFall = ThisEvent.EventParam;
              NextState = DecodeWaitRise;
              CharacterizePulse(); //characterize the pulse based on last rise and fall
            }
            break;
            case ES_BUTTON_DOWN:   // go back to calibration mode
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
            case ES_RISING_EDGE: 
            {
              TimeOfLastRise = ThisEvent.EventParam;
              NextState = DecodeWaitFall;
              CharacterizeSpace(); //characterize the space based on last fall and rise
            }
            break;
            case ES_BUTTON_DOWN:   // go back to calibration mode
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

/****************************************************************************
 Function
   QueryMorseElements

 Parameters
   None

 Returns
   MorseElementState_t: current state of MorseElements machine

 Description
   returns the current state of the MorseElements machine
 Notes

****************************************************************************/
MorseElementState_t QueryMorseElements(void)
{
  return CurrentState;
}


/****************************************************************************
 Function
   Check4MorseEvent

 Parameters
   None

 Returns
   bool: true if Morse Event was found and false otherwise

 Description
   checks for rising and falling edges in the Morse signal
 Notes

****************************************************************************/
bool Check4MorseEvent(void)
{
  bool returnVal = false; //assume morse event was not found
  uint8_t CurrentInputState;
  CurrentInputState = digitalRead(10);// read input pin state and set as current input state
  
  if (CurrentInputState != LastInputState) //if event found
  {
      if (CurrentInputState == 0) //if falling edge
      {
        ES_Event_t ThisEvent;
        ThisEvent.EventType  = ES_FALLING_EDGE;
        ThisEvent.EventParam  = ES_Timer_GetTime(); //store time of event
        PostMorseElementsSM(ThisEvent); //post to morse element service
        returnVal = true;
      }
      else
      {
        ES_Event_t ThisEvent; //if rising edge
        ThisEvent.EventType   = ES_RISING_EDGE;
        ThisEvent.EventParam  = ES_Timer_GetTime(); //store time time of event
        PostMorseElementsSM(ThisEvent); //post to morse element service
        returnVal = true;
      }
  
  LastInputState = CurrentInputState; //set last input state as current input state
  
  return returnVal;
  }
}

/****************************************************************************
 Function
   TestCalibration

 Parameters
   None

 Returns
   None

 Description
   given the morse signal, it tries to get the length of a valid dot
 Notes

****************************************************************************/
void TestCalibration(void)
{
    uint16_t SecondDelta; //second parameter used in calibration
    if (FirstDelta == 0){ //first pass through TestCalibration
        FirstDelta = TimeOfLastFall - TimeOfLastRise; //length of pulse
    }
    else{ //another pass through TestCalibration
        SecondDelta = TimeOfLastFall - TimeOfLastRise; //length of pulse
        if ((100.0 * FirstDelta / SecondDelta) <= 33.33){ //checks if first delta is a valid length of dot
            LengthOfDot = FirstDelta;
            ES_Event_t ThisEvent;
            ThisEvent.EventType = ES_CAL_COMPLETED;
            PostMorseElementsSM(ThisEvent);
        }
        else if ((100.0 * FirstDelta / SecondDelta) > 300.0){ //checks if second delta is a valid length of dot
            LengthOfDot = SecondDelta;
            ES_Event_t ThisEvent;
            ThisEvent.EventType = ES_CAL_COMPLETED;
            PostMorseElementsSM(ThisEvent);
        }
        else { //length of dot not found, go look for another delta to test
            FirstDelta = SecondDelta;
        }
    }
    return;
}

/****************************************************************************
 Function
   CharacterizeSpace

 Parameters
   None

 Returns
   None

 Description
   checks if space is End of Character, End of Word or Bad Space
 Notes

****************************************************************************/
void CharacterizeSpace(void)
{
  uint16_t LastInterval;
  ES_Event_t ThisEvent;
  LastInterval = TimeOfLastRise - TimeOfLastFall; //length of space
  if ((LastInterval != LengthOfDot) && (LastInterval != LengthOfDot+1)) //ensure it's a valid space
  {
      //check for End of Character space
      if ((LastInterval >= LengthOfDot*3) && (LastInterval <= LengthOfDot*3 + 3))
      {
          ThisEvent.EventType = ES_EOC_DETECTED;
          PostMorseDecode(ThisEvent);
          if (QueryMorseElements() == EOC_WaitFall)
          {
            PostMorseElementsSM(ThisEvent);
          }
      }
      //check for End of Word space
      else if ((LastInterval >= LengthOfDot*7) && (LastInterval <= LengthOfDot*7 + 7))
      {
          ThisEvent.EventType = ES_EOW_DETECTED;
          PostMorseDecode(ThisEvent);
          PostMorseElementsSM(ThisEvent);
      }
      //if it gets here, then it's a bad space
      else{
          ThisEvent.EventType = ES_BAD_SPACE;
          PostMorseDecode(ThisEvent);
          if (CurrentState == EOC_WaitFall)
          {
            PostMorseElementsSM(ThisEvent);
          }
      }
  }
  return;
}

/****************************************************************************
 Function
   CharacterizePulse

 Parameters
   None

 Returns
   None

 Description
   checks if pulse is Dot, Dash or Bad Pulse
 Notes

****************************************************************************/
void CharacterizePulse(void)
{
  uint16_t LastPulseWidth;
  ES_Event_t ThisEvent;
  LastPulseWidth = TimeOfLastFall - TimeOfLastRise; //length of pulse
  
  //check for Dot
  if ((LastPulseWidth == LengthOfDot) || (LastPulseWidth == LengthOfDot+1))
  {
      ThisEvent.EventType   = ES_DOT_DETECTED;
      PostMorseDecode(ThisEvent);
  }
  //check for Dash
  else if ((LastPulseWidth >= LengthOfDot*3) && (LastPulseWidth <= LengthOfDot*3 + 3))
  {
      ThisEvent.EventType   = ES_DASH_DETECTED;
      PostMorseDecode(ThisEvent);
  }
  //if it gets here, then it's a bad pulse
  else
  {
      ThisEvent.EventType   = ES_BAD_PULSE;
      PostMorseDecode(ThisEvent);
  }
  return; 
}

/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/

