/****************************************************************************
 Module
   WriteOLEDService1.c

 Revision
   1.0.1

 Description
   This is a state machine for writing to the OLED under the Gen2 Events and 
 Services Framework.

 Notes

****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "ES_DeferRecall.h"
#include "../ProjectHeaders/WriteOLEDService1.h"

//include headers for the u8g2 framework
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
// type of state variable should match that of enum in header file

extern uint8_t u8x8_pic32_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
extern uint8_t u8x8_byte_pic32_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

static WriteOLEDState_t CurrentState;
static u8g2_t u8g2;

static uint16_t offset=118; // start at the rightmost character position
static uint16_t width; // used to store width of the string
char PrintChar[1000]; //initialize an array of size 1000 for storing all the MorseCode characters
static uint16_t char_ind = 0; //used to store the index of the current character being written
// with the introduction of Gen2, we need a module level Priority var as well
static uint8_t MyPriority; //store priority of the service in the framework
static uint8_t LastNextPageState; //store the previous NextPage state
// add a deferral queue for up to 3 pending deferrals +1 to allow for overhead
static ES_Event_t DeferralQueue[3 + 1];

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitWriteOLED

 Parameters
     uint8_t : the priority of this service

 Returns
     bool, false if error in initialization, true otherwise

 Description
     Saves away the priority, sets up the initial transition and does any
     other required initialization for this state machine
 Notes

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
     PostWriteOLED

 Parameters
     EF_Event_t ThisEvent , the event to post to the queue

 Returns
     boolean False if the Enqueue operation failed, True otherwise

 Description
     Posts an event to this state machine's queue
 Notes

****************************************************************************/
bool PostWriteOLED(ES_Event_t ThisEvent)
{
  return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunWriteOLED

 Parameters
   ES_Event_t : the event to process

 Returns
   ES_Event_t, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
 Takes a character from the event posted to it and writes it to the OLED using 
 the u8g2 framework.
 Notes
   uses nested switch/case to implement the machine.

****************************************************************************/
ES_Event_t RunWriteOLED(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

  switch (CurrentState)
  {
    case InitPState:  // If current state is initial Pseudo State
    {
        if (ThisEvent.EventType == ES_INIT)    // only respond to ES_Init
        {
            SPI_Init(); //initialize the SPI module
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
            // width is used for the scrolling demo
            width = (u8g2_GetStrWidth(&u8g2, PrintChar)/2);
            // now put the machine into the actual initial state
            CurrentState = NotWriting;
        }
    }
    break;

    case NotWriting:        // If current state is Not Writing
    {
      switch (ThisEvent.EventType)
      {
        case ES_OLED_CHAR:  // if OLED_CHAR event is posted
        {   // 
            PrintChar[char_ind] = ThisEvent.EventParam; //add character to PrintChar array
            char_ind++; //increment character index in the array
            u8g2_FirstPage(&u8g2); //clear the screen
            u8g2_DrawStr(&u8g2, offset, 37, PrintChar); //write to PrintChar string to the screen
            LastNextPageState = 1; //wait for screen to be ready for next character
            CurrentState = Writing;  //Decide what the next state will be
        }
        break;
        case ES_NEW_KEY:  // if NEW_KEY event is posted.. used for debugging
        {   // 
            PrintChar[char_ind] = ThisEvent.EventParam;
            char_ind++;
            u8g2_FirstPage(&u8g2);
            u8g2_DrawStr(&u8g2, offset, 37, PrintChar);
            LastNextPageState = 1;
            CurrentState = Writing;  //Decide what the next state will be
        }
        break;
        case ES_BUTTON_DOWN:  // if BUTTON_DOWN event is posted
        {   // 
            offset = 118; //reset 'cursor' to the rightmost part of the screen
            str_reset(PrintChar,1000); //clear the PrintChar array
            char_ind = 0; //reset character index to 0;
            u8g2_FirstPage(&u8g2); //clear the screen
            LastNextPageState = 1; //wait for screen to be ready for next character
            CurrentState = Writing;  //Decide what the next state will be
        }
        break;
        break;
      }  
    }
    break;
    
    case Writing:// if current state is Writing
    {
      switch (ThisEvent.EventType)
      {
        case ES_NEXT_PAGE:  //If event is NEXT_PAGE
        {   
            offset -= 9; //decrement offset by 9
            if(offset < -width) offset = 0; // reset and start over
            CurrentState = NotWriting;  //Decide what the next state will be
            ES_RecallEvents(MyPriority, DeferralQueue); //recall deferred events
        }
        break;
        //defer all other events
        case ES_OLED_CHAR:
        {   
            ES_DeferEvent(DeferralQueue, ThisEvent);
        }
        break;
        case ES_NEW_KEY:
        {   
            ES_DeferEvent(DeferralQueue, ThisEvent);
        }
      } 
    }
    break;
    
  }                                 
  return ReturnEvent;
}

/****************************************************************************
 Function
     QueryWriteOLED

 Parameters
     None

 Returns
     The current state of the WriteOLEDService1 state machine

 Description
     returns the current state of the WriteOLED state machine
 Notes
****************************************************************************/
WriteOLEDState_t QueryWriteOLED(void)
{
  return CurrentState;
}

/****************************************************************************
 Function
 Check4NextPage

 Parameters
     None

 Returns
    True if the OLED is not busy otherwise False

 Description
    determines if OLED is ready for next character or not
 Notes
****************************************************************************/

bool Check4NextPage(void) 
{
  bool returnVal = false; //assumes the OLED is busy
  
  if (QueryWriteOLED() == Writing){ //checks the state of machine is Writing
      
      uint8_t CurrentNextPageState;
      CurrentNextPageState = u8g2_NextPage(&u8g2); //returns 1 if busy and 0 otherwise

      if ((CurrentNextPageState != LastNextPageState) && (CurrentNextPageState == 0))  // 
      {
        ES_Event_t ThisEvent;
        ThisEvent.EventType   = ES_NEXT_PAGE;
        PostWriteOLED(ThisEvent);

        returnVal = true; //this means the OLED is not busy
      }
  LastNextPageState = CurrentNextPageState;
  
  } 
  return returnVal;
}

/****************************************************************************
 Function
 str_reset

 Parameters
    character array

 Returns
    None

 Description
    clear and reset a character array
 Notes
****************************************************************************/
void str_reset(char arr[], uint16_t arr_len) 
{
    for(uint16_t i = 0; i < arr_len; i++) //iterate over each element
    {
        arr[i] = '\0'; //reset with null character
    }
    return;
}