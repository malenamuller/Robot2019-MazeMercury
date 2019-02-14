/***************************************************************************//**
  @file     step.c
  @brief    Driver for stepper motors
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "step.h"

#include "board.h"
#include "PORT.h"
#include "GPIO.h"
#include "FTM.h"

#include "Template/LDM.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// Configuration BEGIN

#define STEP_PREESCALER_POW		6

#define STEP_FTM_NUM			3

// Configuration END




#define STEP_PREESCALER_VAL		(1U<<STEP_PREESCALER_POW)
#define STEP_COUNTER_FREQ_HZ	(__CORE_CLOCK__/2/STEP_PREESCALER_VAL)

#if (STEP_COUNTER_FREQ_HZ == 781250U)
#define STEP_US2SEMICOUNT(tus)	((uint16_t)((((uint32_t)(tus))*25+(1<<(STEP_PREESCALER_POW-1)))>>STEP_PREESCALER_POW))
#else
#error mal configurado!
#endif

#define STEP_SEMICOUNT_MIN		STEP_US2SEMICOUNT(STEP_PERIOD_MIN_US)

//Pines de los motores
#define STEP_PIN_PORT_ALT2		{0, 0, 0, 0, 0, 0, true, 0, PORT_mAlt2, 0, 0, PORT_eDisabled, 0, 0, 0}
#define STEP_PIN_PORT_ALT3		{0, 0, 0, 0, 0, 0, true, 0, PORT_mAlt3, 0, 0, PORT_eDisabled, 0, 0, 0}


#define _CONCAT2(a,b)			a##b
#define CONCAT2(a,b)			_CONCAT2(a,b)
#define _CONCAT3(a,b,c)			a##b##c
#define CONCAT3(a,b,c)			_CONCAT3(a,b,c)


#define STEP_FTM_PTR			CONCAT2(FTM, STEP_FTM_NUM)


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
	PORT_t port;
	uint32_t num;
	PCRstr portConfig;
	FTMChannel_t ch;
} stepConfig_t;


/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static const stepConfig_t stepList[CANT_STEPIDS] = {
	{ PORTC, 10, STEP_PIN_PORT_ALT3, 6 },
	{ PORTC, 11, STEP_PIN_PORT_ALT3, 7 },
};

#if PIN_SERVO_DIR != PORTPINNUM2PIN(PC,1)
#error tabla servoList desactualizada!!
#endif


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static uint32_t stepPeriod[CANT_STEPIDS];
static uint16_t stepSemiCount[CANT_STEPIDS];
static uint8_t stepRunning[CANT_STEPIDS];
static uint8_t stepDir[CANT_STEPIDS];

//static uint16_t stepCnV[CANT_STEPIDS];



/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void Step_Init (void)
{
	static bool yaInit = false;
	int i;
	const stepConfig_t* step;

	if (yaInit)
		return;

	FTM_Init();

	// Configure PORT (alt mode)
	for (step=stepList ; step<stepList+CANT_STEPIDS ; ++step)
	{
		PORT_Configure2(step->port, step->num, step->portConfig);
	}


	// Configure FTM: Clock Source, preescaler,
	FTM_SetPrescaler(STEP_FTM_PTR, STEP_PREESCALER_POW); // BusClock=sysclk/2= 50MHz y presccaler = 2^PWM_PREESCALER_POW

	FTM_SetModulus(STEP_FTM_PTR, 0xFFFF); // Free counter


	// Configure Channel
	for (i=0 ; i<CANT_STEPIDS ; ++i)
	{
		step = stepList+i;

		FTM_SetWorkingMode(STEP_FTM_PTR, step->ch, FTM_mOutputCompare); // PWM
		FTM_SetOutputCompareEffect (STEP_FTM_PTR, step->ch, FTM_eClear);

		FTM_SetCounter(STEP_FTM_PTR, step->ch, stepPeriod[i]);

		//FTM_SetInterruptMode(STEP_FTM_PTR, step->ch, true);
		//stepSemiCount[i] = STEP_US2SEMICOUNT(stepPeriod[i]);
	}
	NVIC_EnableIRQ(CONCAT3(FTM, STEP_FTM_NUM, _IRQn)); //FTMx_IRQn

	FTM_StartClock(STEP_FTM_PTR); //Select BusClk
	yaInit = true;
}




void Step_Run(stepId_t id, uint32_t period_us, stepDir_t dir)
{
	if (id < CANT_STEPIDS)
	{
		if (period_us < STEP_PERIOD_MIN_US) // truncate
			period_us = STEP_PERIOD_MIN_US;
		else if (period_us > STEP_PERIOD_MAX_US)
			period_us = STEP_PERIOD_MAX_US;

		stepPeriod[id] = period_us;
		period_us = STEP_US2SEMICOUNT(period_us);
		stepDir[id]=dir;//AGREGO ESTO!
		stepSemiCount[id] = period_us; // esto debe ser atomic!!

		if (!stepRunning[id])
		{
			FTMChannel_t ch = stepList[id].ch;

			FTM_ClearInterruptFlag(STEP_FTM_PTR, ch); // Clear ISR flag
			do {
				STEP_FTM_PTR->CONTROLS[ch].CnV = STEP_SEMICOUNT_MIN + STEP_FTM_PTR->CNT;
			} while ((int16_t)(STEP_FTM_PTR->CONTROLS[ch].CnV - STEP_FTM_PTR->CNT) <= 0); // just-in-case que cayó una ISR en el medio
			stepRunning[id] = true;
			FTM_SetOutputCompareEffect (STEP_FTM_PTR, ch, FTM_eToggle);
			FTM_SetInterruptMode(STEP_FTM_PTR, ch, true);
		}
	}
}



uint16_t Step_GetPeriod(stepId_t id)
{
	return ((id < CANT_STEPIDS)? stepPeriod[id] : 0);
}



void Step_Stop(stepId_t id)
{
	if (id < CANT_STEPIDS)
	{
		stepRunning[id] = false;
		FTM_SetOutputCompareEffect (STEP_FTM_PTR, stepList[id].ch, FTM_eClear); // ORDEN IMPORTANTE: primero eClear y luego disable ISR
		FTM_SetInterruptMode(STEP_FTM_PTR, stepList[id].ch, false);
		stepPeriod[id] = 0;
		stepSemiCount[id] = 0;
	}
}



/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

__ISR__ CONCAT3(FTM, STEP_FTM_NUM, _IRQHandler) (void)
{
	uint8_t id;

	for(id=0 ;  id<CANT_STEPIDS ; ++id)
	{
		if (stepRunning[id])
		{
			FTMChannel_t ch = stepList[id].ch;
			if (FTM_IsInterruptPending(STEP_FTM_PTR, ch))
			{
				FTM_ClearInterruptFlag(STEP_FTM_PTR, ch); // Clear ISR flag
				STEP_FTM_PTR->CONTROLS[ch].CnV += stepSemiCount[id]; // Set new interruption
			}
		}
	}
}

