/****************************************************************************

  Header file for Test Harness Service0
  based on the Gen 2 Events and Services Framework

 ****************************************************************************/

#ifndef MyService1_H
#define MyService1_H

#include <stdint.h>
#include <stdbool.h>

#include "ES_Events.h"
#include "ES_Port.h"                // needed for definition of REENTRANT
// Public Function Prototypes

bool InitMyService1(uint8_t Priority);
bool PostMyService1(ES_Event_t ThisEvent);
ES_Event_t RunMyService1(ES_Event_t ThisEvent);

#endif /* ServTemplate_H */

