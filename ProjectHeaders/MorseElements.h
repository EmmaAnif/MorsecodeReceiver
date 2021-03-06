/****************************************************************************

  Header file for MorseElements service
  based on the Gen 2 Events and Services Framework

 ****************************************************************************/

#ifndef MorseElements_H
#define MorseElements_H

#include "ES_Types.h"

// State definitions for use with the query function
typedef enum
{
  InitMorseElements, CalWait4Rise, CalWait4Fall, EOC_WaitRise,
  EOC_WaitFall, DecodeWaitRise, DecodeWaitFall
}MorseElementState_t;

// Public Function Prototypes
bool InitMorseElementsSM(uint8_t Priority);
bool PostMorseElementsSM(ES_Event_t ThisEvent);
ES_Event_t RunMorseElementsSM(ES_Event_t ThisEvent);
MorseElementState_t QueryMorseElements(void);
bool Check4MorseEvent(void);
void TestCalibration(void);
void CharacterizeSpace(void);
void CharacterizePulse(void);

#endif /* MorseElements_H */

