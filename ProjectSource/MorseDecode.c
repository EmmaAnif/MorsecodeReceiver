/****************************************************************************
 Module
   DecodeMorse.c

 Revision
   1.0.1

 Description
   This is a file for implementing the Decode Morse service under the
   Gen2 Events and Services Framework.

 Notes

****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "MorseDecode.h"

/*----------------------------- Module Defines ----------------------------*/
#define MSlength 8 //Morse string length
#define memreset(arr) for(uint8_t i = 0; i < MSlength; i++) {arr[i] = '0';}

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;
char MorseString[8];
uint8_t MSIndex;

char LegalChars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890?.,:'-/()\"= !$&+;@_";
char MorseCode[][8] ={ ".-","-...","-.-.","-..",".","..-.","--.",
                      "....","..",".---","-.-",".-..","--","-.","---",
                      ".--.","--.-",".-.","...","-","..-","...-",
                      ".--","-..-","-.--","--..",".----","..---",
                      "...--","....-",".....","-....","--...","---..",
                      "----.","-----","..--..",".-.-.-","--..--",
                      "---...",".----.","-....-","-..-.","-.--.-",
                      "-.--.-",".-..-.","-...-","-.-.--","...-..-",
                      ".-...",".-.-.","-.-.-.",".--.-.","..--.-"};

uint8_t LClength; //number of legal characters

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitMorseDecode

 Parameters
     uint8_t : the priorty of this service

 Returns
     bool, false if error in initialization, true otherwise

 Description
     Saves away the priority, and does any
     other required initialization for this service
 Notes

 Author
     J. Edward Carryer, 01/16/12, 10:00
****************************************************************************/
bool InitMorseDecode(uint8_t Priority)
{
  MyPriority = Priority;
  memreset(MorseString);
  MSIndex = 0;
  LClength = sizeof(LegalChars)/sizeof(*LegalChars);
  
  return true;
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

****************************************************************************/
bool PostMorseDecode(ES_Event_t ThisEvent)
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

****************************************************************************/
ES_Event_t RunMorseDecode(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
  
  switch (ThisEvent.EventType) {
      case ES_DOT_DETECTED: {
          if (MSIndex < MSlength) {
              MorseString[MSIndex] = '.';
              MSIndex++;
          }
          else {
              ReturnEvent.EventType = ES_ERROR;
              ReturnEvent.EventParam = ES_DOT_DETECTED;
          }
      }
      break;
      
      case ES_DASH_DETECTED: {
          if (MSIndex < MSlength) {
              MorseString[MSIndex] = '-';
              MSIndex++;
          }
          else {
              ReturnEvent.EventType = ES_ERROR;
              ReturnEvent.EventParam = ES_DASH_DETECTED;
          }
      }
      break;
      
      case ES_EOC_DETECTED: {
          //PrintToOLED(DecodeMorseString())
          printf("%c",DecodeMorseString());
          memreset(MorseString);
          MSIndex = 0;
          
      }
      break;
      
      case ES_EOW_DETECTED: {
          //PrintToOLED(DecodeMorseString())
          //PrintToOLED(Space)
          printf("%c ",DecodeMorseString());
          memreset(MorseString);
          MSIndex = 0;
      }
      break;
      
      case ES_BAD_SPACE: {
          memreset(MorseString);
          MSIndex = 0;
      }
      break;
      
      case ES_BAD_PULSE: {
          memreset(MorseString);
          MSIndex = 0;
      }
      break;
      
      case ES_BUTTON_DOWN: {
          memreset(MorseString);
          MSIndex = 0;
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
char DecodeMorseString(void) 
{
    char CompareString[8];
    char DecodedChar = '~';
    uint8_t j,k;
    for (j = 0; j < LClength; j++) {
        for (k = 0; k < MSlength; k++) {
            CompareString[k] = MorseCode[j][k];
        }
            if (MorseString == CompareString){
                DecodedChar = LegalChars[j];
            }
    }
    return DecodedChar;
}
/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/

