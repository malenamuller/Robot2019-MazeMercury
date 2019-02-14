/***************************************************************************//**
  @file     servo.c
  @brief    Driver for servo motors
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "servo.h"

#include "board.h"
#include "PORT.h"
#include "GPIO.h"
#include "FTM.h"

#include "Template/LDM.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// Configuration BEGIN

#define SERVO_PREESCALER_POW	4
#define SERVO_MOD_FREQ_HZ		50U

#define SERVO_FTM_NUM			0

// Configuration END


#define SERVO_PREESCALER_VAL	(1U<<SERVO_PREESCALER_POW)
#define SERVO_COUNTER_FREQ_HZ	(__CORE_CLOCK__/2/SERVO_PREESCALER_VAL)
#define SERVO_MOD_VAL			(SERVO_COUNTER_FREQ_HZ/SERVO_MOD_FREQ_HZ)


#if (SERVO_MOD_FREQ_HZ == 50) && (SERVO_MOD_VAL == 62500)
#endif
#define _CONCAT2(a,b)			a##b
#define CONCAT2(a,b)			_CONCAT2(a,b)
#define _CONCAT3(a,b,c)			a##b##c
#define CONCAT3(a,b,c)			_CONCAT3(a,b,c)

#define SERVO_FTM_PTR			CONCAT2(FTM, SERVO_FTM_NUM)

#define PWM_PIN_PORT_ALT4		{0, 0, 0, 0, 0, 0, true, 0, PORT_mAlt4, 0, 0, PORT_eDisabled, 0, 0, 0}


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
	PORT_t port;
	uint32_t num;
	PCRstr portConfig;
	FTMChannel_t ch;
	uint16_t ton_init;
} servoConfig_t;


/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static const servoConfig_t servoList[CANT_SERVOIDS] = {
	{ PORTC, 1, PWM_PIN_PORT_ALT4, 0, 1500 }, // SERVOID_DIRECTION
};

#if PIN_SERVO_DIR != PORTPINNUM2PIN(PC,1)
#error tabla servoList desactualizada!!
#endif


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static uint16_t servoWidth[CANT_SERVOIDS];


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void Servo_Init (void)
{
	static bool yaInit = false;
	const servoConfig_t* servo;

	if (yaInit)
		return;

	FTM_Init();

	// Configure PORT (alt mode)
	for (servo=servoList ; servo<servoList+CANT_SERVOIDS ; ++servo)
	{
		PORT_Configure2(servo->port, servo->num, servo->portConfig);
	}


	// Configure FTM: Clock Source, preescaler,
	FTM_SetPrescaler(SERVO_FTM_PTR, SERVO_PREESCALER_POW); // BusClock=sysclk/2= 50MHz y presccaler = 2^PWM_PREESCALER_POW

	FTM_SetModulus(SERVO_FTM_PTR, SERVO_MOD_VAL-1); // Valor incial del contador


	// Configure Channel
	for (servo=servoList ; servo<servoList+CANT_SERVOIDS ; ++servo)
	{
		FTM_SetWorkingMode(SERVO_FTM_PTR, servo->ch, FTM_mPulseWidthModulation); // PWM
		FTM_SetOutputCompareEffect (SERVO_FTM_PTR, servo->ch, FTM_eClear);
		//FTM_SetCounter(SERVO_FTM_PTR, servo->ch, SERVO_US2COUNT(servo->ton_init)); //COMENTAMOS ESTO porque no esta definido SERVO_US2COUNT
		servoWidth[servoList-servo] = servo->ton_init;
	}

	FTM_StartClock(SERVO_FTM_PTR); //Select BusClk
	yaInit = true;
}


void Servo_SetWidth(servoId_t id, uint16_t ton_us)
{
	if (id < CANT_SERVOIDS)
	{
		if (ton_us < SERVO_TON_US_MIN) // truncate
			ton_us = SERVO_TON_US_MIN;
		else if (ton_us > SERVO_TON_US_MAX)
			ton_us = SERVO_TON_US_MAX;

		servoWidth[id] = ton_us;
		ton_us  = SERVO_US2COUNT(ton_us);
		FTM_SetCounter(SERVO_FTM_PTR, servoList[id].ch, ton_us);
	}
}


uint16_t Servo_GetWidth(servoId_t id)
{
	return ((id < CANT_SERVOIDS)? servoWidth[id] : 0);
}
