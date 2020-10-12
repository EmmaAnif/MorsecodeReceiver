/****************************************************************************
 Module
   MyService1.c
 * 
/*----------------------------- Include Files -----------------------------*/
// This module
#include "../ProjectHeaders/MyService1.h"

// debugging printf()
//#include "dbprintf.h"

// Hardware
#include <xc.h>
#include <proc/p32mx170f256b.h>
#include <sys/attribs.h> // for ISR macors

// Event & Services Framework
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "ES_DeferRecall.h"
#include "ES_ShortTimer.h"
#include "ES_Port.h"

/*----------------------------- Module Defines ----------------------------*/
// these times assume a 10.000mS/tick timing
#define ONE_SEC 1000
#define HALF_SEC (ONE_SEC / 2)
#define TWO_SEC (ONE_SEC * 2)
#define FIVE_SEC (ONE_SEC * 5)

#define ENTER_POST     ((MyPriority<<3)|0)
#define ENTER_RUN      ((MyPriority<<3)|1)
#define ENTER_TIMEOUT  ((MyPriority<<3)|2)

#define TEST_INT_POST
/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;
// add a deferral queue for up to 3 pending deferrals +1 to allow for overhead
static ES_Event_t DeferralQueue[3 + 1];

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitMyService1

 Parameters
     uint8_t : the priority of this service

 Returns
     bool, false if error in initialization, true otherwise

 Description
     Saves away the priority, and does any
     other required initialization for this service
 Notes

****************************************************************************/
bool InitMyService1(uint8_t Priority)
{
  ES_Event_t ThisEvent;

  MyPriority = Priority;
  
  // initialize deferral queue for testing Deferral function
  ES_InitDeferralQueueWith(DeferralQueue, ARRAY_SIZE(DeferralQueue));

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
     PostMyService1

 Parameters
     EF_Event ThisEvent ,the event to post to the queue

 Returns
     bool false if the Enqueue operation failed, true otherwise

 Description
     Posts an event to this state machine's queue
 Notes

****************************************************************************/
bool PostMyService1(ES_Event_t ThisEvent)
{
  return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunMyService1

 Parameters
   ES_Event : the event to process

 Returns
   ES_Event, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
   add your description here
 Notes

****************************************************************************/
ES_Event_t RunMyService1(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
  static char DeferredChar = '1';

  switch (ThisEvent.EventType)
  {
    case ES_INIT:
    {
      ES_Timer_InitTimer(SERVICE1_TIMER, HALF_SEC);
      puts("Service 01:");
      printf("\rES_INIT received in Service %d\r\n", MyPriority);
    }
    break;
    case ES_NEW_KEY:   // announce
    {
      if ('e' == ThisEvent.EventParam)
      {
        ES_Event_t MyFirstEvent;
        MyFirstEvent.EventType = ES_MY_EVENT_1;
        MyFirstEvent.EventParam = 'e';
        ES_PostToService(MyPriority, MyFirstEvent); 
      }
      if ('a' == ThisEvent.EventParam)
      {
        ES_Event_t MySecondEvent;
        MySecondEvent.EventType = ES_MY_EVENT_2;
        MySecondEvent.EventParam = 'a';
        ES_PostToService(MyPriority, MySecondEvent); 
      }
    break;
    case ES_MY_EVENT_1:   // announce my first event
    {
      printf("ES_MY_EVENT_1 received with -> %c <- in Service 1\r\n",
          (char)ThisEvent.EventParam);
    }
    break;
    case ES_MY_EVENT_2:   // announce my second event
    {
      printf("ES_MY_EVENT_2 received with -> %c <- in Service 1\r\n",
          (char)ThisEvent.EventParam);
    }
    break; 
    }
    default:
    {}
     break;
  }
  
  return ReturnEvent;
}

/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/

