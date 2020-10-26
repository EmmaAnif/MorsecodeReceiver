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
#include "MorseElements.h"
#include "WriteOLEDService1.h"

/*----------------------------- Module Defines ----------------------------*/
#define MSlength 8 //Morse string length

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;
char MorseString[8]; //container to store dots and dashes
char MorseChar; //store decoded character
uint8_t MSIndex; //keep tract of index of characters in MorseString

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
     uint8_t : the priority of this service

 Returns
     bool, false if error in initialization, true otherwise

 Description
     Saves away the priority, and does any
     other required initialization for this service
 Notes

****************************************************************************/
bool InitMorseDecode(uint8_t Priority)
{
  MyPriority = Priority;
  str_reset(MorseString, MSlength); //clear and reset the MorseString array
  MSIndex = 0; 
  LClength = sizeof(LegalChars)/sizeof(*LegalChars); //get the number of legal characters
  
  return true;
}

/****************************************************************************
 Function
     PostMorseCode

 Parameters
     ES_Event_t ThisEvent ,the event to post to the queue

 Returns
     bool false if the post operation failed, true otherwise

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
    RunMorseCode

 Parameters
   ES_Event_t : the event to process

 Returns
   ES_Event, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
    accepts dots and dashes then decodes each character and sends it to be printed 
    in the OLED
 Notes

****************************************************************************/
ES_Event_t RunMorseDecode(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
  
  switch (ThisEvent.EventType) {
      case ES_DOT_DETECTED: {
          if (MSIndex < MSlength) { //checks to ensure sequence of dots and dashes is valid
              MorseString[MSIndex] = '.';
              MSIndex++; //
          }
          else {
              ReturnEvent.EventType = ES_ERROR;
              ReturnEvent.EventParam = 'y'; //used to represent where error occurred
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
              ReturnEvent.EventParam = 'z';
          }
      }
      break;
      
      case ES_EOC_DETECTED: { //if end of character is detected
          MorseChar = DecodeMorseString(); //save decoded character
          printf("%c",MorseChar); //prints to TeraTerm for debugging
          str_reset(MorseString, MSlength); // reset MorseString
          MSIndex = 0; //reset the character index
          
          ES_Event_t CHAREvent;
          CHAREvent.EventType = ES_OLED_CHAR;
          CHAREvent.EventParam = MorseChar;
          PostWriteOLED(CHAREvent); //post to OLED
      }
      break;
      
      case ES_EOW_DETECTED: { //if end of word is detected
          MorseChar = DecodeMorseString();
          printf("%c ",MorseChar);
          str_reset(MorseString, MSlength);
          MSIndex = 0;
          
          //post space after character
          ES_Event_t CHAREvent;
          CHAREvent.EventType = ES_OLED_CHAR;
          CHAREvent.EventParam = MorseChar;
          PostWriteOLED(CHAREvent);
          
          ES_Event_t SPACEEvent; 
          SPACEEvent.EventType = ES_OLED_CHAR;
          SPACEEvent.EventParam = ' ';
          PostWriteOLED(SPACEEvent);
          
      }
      break;
      
      case ES_BAD_SPACE: { //if a bad space is encountered
          printf("B_S"); //used for debugging
          str_reset(MorseString, MSlength); //clear MorseString
          MSIndex = 0;
      }
      break;
      
      case ES_BAD_PULSE: { //if bad pulse is encountered
          printf("B_P"); //used for debugging
          str_reset(MorseString, MSlength); //clear MorseString
          MSIndex = 0;
      }
      break;
      
      case ES_BUTTON_DOWN: { //if Button down event
          str_reset(MorseString, MSlength); //clear MorseString
          MSIndex = 0;
      }
      break;
      
      default:
      {}
      break;
  }
  
  return ReturnEvent;
}

/****************************************************************************
 Function
   DecodeMorseString

 Parameters
   None

 Returns
   char: character that was decoded based on series of dots and dashes

 Description
    accepts dots and dashes then decodes each character
 Notes

****************************************************************************/
char DecodeMorseString(void) 
{
    bool checkVal; //used to check if legal character was found
    
    uint8_t j,k;
    char DecodedChar = '~'; //assume we did not find legal character
    for (j = 0; j < LClength; j++) { //iterate over all legal characters
        checkVal = true; //assume legal character was found
        for (k = 0; k < MSlength; k++) { //iterate over each element of the MorseString
            if (MorseCode[j][k] != MorseString[k]){ //any disparity means false
                checkVal = false; //legal character not found
                break;
            }
        }
        if (checkVal == true) {
            DecodedChar = LegalChars[j]; //if false not encountered, we found a legal character
            break;
        }
    }
    return DecodedChar;
}

/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/

