/****************************************************************************

  Header file for MorseElements service
  based on the Gen 2 Events and Services Framework

 ****************************************************************************/

#ifndef MorseElements_H
#define MorseElements_H

#include "ES_Types.h"

// Public Function Prototypes

bool InitMorseElements(uint8_t Priority);
bool PostMorseElements(ES_Event_t ThisEvent);
ES_Event_t RunMorseElements(ES_Event_t ThisEvent);
bool Check4MorseInput(void);

#endif /* ServTemplate_H */

