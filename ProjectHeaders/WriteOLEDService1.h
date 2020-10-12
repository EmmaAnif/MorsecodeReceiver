/****************************************************************************

  Header file for template Flat Sate Machine
  based on the Gen2 Events and Services Framework

 ****************************************************************************/

#ifndef WriteOLED_H
#define WriteOLED_H

// Event Definitions
#include "ES_Configure.h" /* gets us event definitions */
#include "ES_Types.h"     /* gets bool type for returns */

// typedefs for the states
// State definitions for use with the query function
typedef enum
{
  InitPState, NotWriting, Writing,
}WriteOLEDState_t;

// Public Function Prototypes

bool InitWriteOLED(uint8_t Priority);
bool PostWriteOLED(ES_Event_t ThisEvent);
ES_Event_t RunWriteOLED(ES_Event_t ThisEvent);
WriteOLEDState_t QueryWriteOLED(void);

#endif /* FSMTemplate_H */

