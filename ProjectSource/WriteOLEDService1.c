/****************************************************************************
 Module
   TemplateFSM.c

 Revision
   1.0.1

 Description
   This is a template file for implementing flat state machines under the
   Gen2 Events and Services Framework.

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 01/15/12 11:12 jec      revisions for Gen2 framework
 11/07/11 11:26 jec      made the queue static
 10/30/11 17:59 jec      fixed references to CurrentEvent in RunTemplateSM()
 10/23/11 18:20 jec      began conversion from SMTemplate.c (02/20/07 rev)
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "ES_DeferRecall.h"
#include "../ProjectHeaders/WriteOLEDService1.h"
#include "../u8g2Headers/u8g2TestHarness_main.h"
#include "../u8g2Headers/common.h"
#include "../u8g2Headers/spi_master.h"
#include "../u8g2Headers/u8g2.h"
#include "../u8g2Headers/u8x8.h"

/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match htat of enum in header file

extern uint8_t u8x8_pic32_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
extern uint8_t u8x8_byte_pic32_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

static WriteOLEDState_t CurrentState;
static u8g2_t u8g2;

static uint16_t offset=118; // start at the rightmost character position
static uint16_t width;
char PrintChar[1000];
static uint16_t char_ind = 0;
// with the introduction of Gen2, we need a module level Priority var as well
static uint8_t MyPriority;
static uint8_t LastNextPageState;
// add a deferral queue for up to 3 pending deferrals +1 to allow for overhead
static ES_Event_t DeferralQueue[3 + 1];

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitTemplateFSM

 Parameters
     uint8_t : the priorty of this service

 Returns
     bool, false if error in initialization, true otherwise

 Description
     Saves away the priority, sets up the initial transition and does any
     other required initialization for this state machine
 Notes

 Author
     J. Edward Carryer, 10/23/11, 18:55
****************************************************************************/
bool InitWriteOLED(uint8_t Priority)
{
  ES_Event_t ThisEvent;

  MyPriority = Priority;
  LastNextPageState = 0;
  // put us into the Initial PseudoState
  CurrentState = InitPState;
  // post the initial transition event
  ThisEvent.EventType = ES_INIT;
  // initialize deferral queue
  ES_InitDeferralQueueWith(DeferralQueue, ARRAY_SIZE(DeferralQueue));
  
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
     PostTemplateFSM

 Parameters
     EF_Event_t ThisEvent , the event to post to the queue

 Returns
     boolean False if the Enqueue operation failed, True otherwise

 Description
     Posts an event to this state machine's queue
 Notes

 Author
     J. Edward Carryer, 10/23/11, 19:25
****************************************************************************/
bool PostWriteOLED(ES_Event_t ThisEvent)
{
  return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunTemplateFSM

 Parameters
   ES_Event_t : the event to process

 Returns
   ES_Event_t, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
   add your description here
 Notes
   uses nested switch/case to implement the machine.
 Author
   J. Edward Carryer, 01/15/12, 15:23
****************************************************************************/
ES_Event_t RunWriteOLED(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

  switch (CurrentState)
  {
    case InitPState:        // If current state is initial Psedudo State
    {
        if (ThisEvent.EventType == ES_INIT)    // only respond to ES_Init
        {
            // sysInit is used for timing setup for the test harness, the framework will
            // give you the timing that you need for Lab 3
            SPI_Init(); // hmm, I wonder who will write this function :-)
            // build up the u8g2 structure with the proper values for our display
            // use the next 5 lines verbatim in your initialization
            u8g2_Setup_ssd1306_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_pic32_hw_spi, 
                                               u8x8_pic32_gpio_and_delay);
            // pass all that stuff on to the display to initialize it
            u8g2_InitDisplay(&u8g2);
            // turn off power save so that the display will be on
            u8g2_SetPowerSave(&u8g2, 0);
            // choose the font. this one is mono-spaced and has a reasonable size
            u8g2_SetFont(&u8g2, u8g2_font_t0_18_mr);
            // overwrite the background color of newly written characters
            u8g2_SetFontMode(&u8g2, 0);
            // width is used only for the scrolling demo
            width = (u8g2_GetStrWidth(&u8g2, PrintChar)/2);
            // this is where you would put any actions associated with the
            // transition from the initial pseudo-state into the actual
            // initial state

            // now put the machine into the actual initial state
            CurrentState = NotWriting;
        }
    }
    break;

    case NotWriting:        // If current state is state one
    {
      switch (ThisEvent.EventType)
      {
        case ES_OLED_CHAR:  //
        {   // 
            PrintChar[char_ind] = ThisEvent.EventParam;
            char_ind++;
            u8g2_FirstPage(&u8g2);
            u8g2_DrawStr(&u8g2, offset, 37, PrintChar);
            LastNextPageState = 1;
            CurrentState = Writing;  //Decide what the next state will be
        }
        break;
        case ES_NEW_KEY:  //
        {   // 
            PrintChar[char_ind] = ThisEvent.EventParam;
            char_ind++;
            u8g2_FirstPage(&u8g2);
            u8g2_DrawStr(&u8g2, offset, 37, PrintChar);
            LastNextPageState = 1;
            CurrentState = Writing;  //Decide what the next state will be
        }
        break;
        case ES_BUTTON_DOWN:  //
        {   // 
            offset = 118;
            str_reset(PrintChar);
            char_ind = 0;
            PrintChar[char_ind] = ' ';
            char_ind++;
            u8g2_FirstPage(&u8g2);
            u8g2_DrawStr(&u8g2, offset, 37, PrintChar);
            LastNextPageState = 1;
            CurrentState = Writing;  //Decide what the next state will be
        }
        break;
        break;
      }  // end switch on CurrentEvent
    }
    break;
    
    case Writing://
    {
      switch (ThisEvent.EventType)
      {
        case ES_NEXT_PAGE:  //If event is event one
        {   // Execute action function for state one : event one
            offset -= 9; 
            if(offset < -width) offset = 0; // reset and start over
            CurrentState = NotWriting;  //Decide what the next state will be
            ES_RecallEvents(MyPriority, DeferralQueue);
        }
        break;
        case ES_OLED_CHAR:  //
        {   //
            ES_DeferEvent(DeferralQueue, ThisEvent);
        }
        break;
        case ES_NEW_KEY:  //
        {   //
            ES_DeferEvent(DeferralQueue, ThisEvent);
        }
      }  // end switch on CurrentEvent
    }
    break;
    
  }                                   // end switch on Current State
  return ReturnEvent;
}

/****************************************************************************
 Function
     QueryTemplateSM

 Parameters
     None

 Returns
     TemplateState_t The current state of the Template state machine

 Description
     returns the current state of the Template state machine
 Notes

 Author
     J. Edward Carryer, 10/23/11, 19:21
****************************************************************************/
WriteOLEDState_t QueryWriteOLED(void)
{
  return CurrentState;
}

/***************************************************************************
 private functions
 ***************************************************************************/

bool Check4NextPage(void)
{
  bool returnVal = false;
  
  if (QueryWriteOLED() == Writing){
      
      uint8_t CurrentNextPageState;
      CurrentNextPageState = u8g2_NextPage(&u8g2);

      if ((CurrentNextPageState != LastNextPageState) && (CurrentNextPageState == 0))  // 
      {
        ES_Event_t ThisEvent;
        ThisEvent.EventType   = ES_NEXT_PAGE;
        PostWriteOLED(ThisEvent);

        returnVal = true;
      }
  LastNextPageState = CurrentNextPageState;
  
  } 
  return returnVal;
}

void str_reset(char arr[]) 
{
    for(uint16_t i = 0; i < 1000; i++) //iterate over each element
    {
        arr[i] = '\0'; //reset with null character
    }
    return;
}