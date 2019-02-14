/***************************************************************************//**
  @file     Timer.c
  @brief    Timer services for application layer
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "Timer.h"
#include "Template/LDM.h"

#include "SysTick.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define INTERNAL_TIMER_ID	0


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void Timer_ISR(void);


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static uint32_t timers[TIMERS_MAX_CANT+1];
static uint8_t timers_cant = 1;


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void Timer_Init(void)
{
	static uint8_t ya_init = FALSE;
	if (ya_init)
	    return;
	
	SysTick_Init();
	SysTick_RegisterCallBack(Timer_ISR);

	SIM_SCGC6 |= SIM_SCGC6_RTC_MASK;
    
    ya_init = TRUE;
}



uint8_t Timer_GetId(void)
{
	if (timers_cant < TIMERS_MAX_CANT)
	{
		return timers_cant++;
	}
	else
	{
		return TIMER_INVALID_ID;
	}
}


void Timer_SetTimeout(uint8_t id, uint32_t timeout_ms)
{
	if (id < timers_cant)
	{
		timers[id] = timeout_ms;
	}
}


uint8_t Timer_IsExpired(uint8_t id)
{
	uint8_t rta = FALSE;

	if (id < timers_cant)
	{
		rta = (timers[id] == 0);
	}

	return rta;
}


void Timer_DelayMS(uint32_t timeout_ms)
{
	Timer_SetTimeout(INTERNAL_TIMER_ID, timeout_ms); // set delay
    while (!Timer_IsExpired(INTERNAL_TIMER_ID)) { }; // and wait
}



/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


static void Timer_ISR(void) //acá llego cada 1 mseg
{
	uint8_t id;
    // decremento los timers activos
    for (id=0 ; id<timers_cant ; ++id)
    {
    	if (timers[id] > 0)
    		timers[id]--;
    }
}


/*******************************************************************************
 ******************************************************************************/
