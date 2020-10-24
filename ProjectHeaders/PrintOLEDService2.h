/****************************************************************************

  Header file for template Flat Sate Machine
  based on the Gen2 Events and Services Framework

 ****************************************************************************/

#ifndef PrintOLED_H
#define PrintOLED_H

// Event Definitions
#include "ES_Configure.h" /* gets us event definitions */
#include "ES_Types.h"     /* gets bool type for returns */

// typedefs for the states
// State definitions for use with the query function
typedef enum
{
  InitPState2, SendingChar
}PrintOLEDState_t;

// Public Function Prototypes

bool InitPrintOLED(uint8_t Priority);
bool PostPrintOLED(ES_Event_t ThisEvent);
ES_Event_t RunPrintOLED(ES_Event_t ThisEvent);
PrintOLEDState_t QueryPrintOLED(void);

#endif /* FSMTemplate_H */

