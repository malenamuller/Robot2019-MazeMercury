/***************************************************************************//**
  @file     Motor.c
  @brief    Motor driver
  @author   Nicolás Magliola
 ******************************************************************************/


/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "Motor.h"

#include "PWM.h"
#include "Pin.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define MOT_SMOOTH_DUTY_UPDATE_STEP (MOT_DUTY_MAX_POS/8)


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

enum {MOT_TERM_PLUS, MOT_TERM_MINUS, MOT_TERMS_CANT};


typedef struct {
	motDuty_t dutyNow;
	motDuty_t dutyDesired;
	const char *pinNum;
} motConfig_t;


/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void Mot_WriteSyncDuty(motConfig_t *mot, motDuty_t duty);


/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static const char motPn[LADOS_CANT][MOT_TERMS_CANT] = {
	{PWMID_MOT_IZQ_PLUS, PIN_MOT_IZQ_MINUS},  // MOT_ID_IZQ
	{PWMID_MOT_DER_PLUS, PIN_MOT_DER_MINUS}   // MOT_ID_DER
};


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static motConfig_t motConfig[LADOS_CANT] = { 
	{ MOT_DUTY_OFF, 0, motPn[IZQUIERDA] },
	{ MOT_DUTY_OFF, 0, motPn[DERECHA] }
};


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void Mot_Init(void)
{
	static bool yaInit = false;
	if (yaInit)
		return;

	PWM_Init();
}


void Mot_WriteDutyNow(motId_t id, motDuty_t duty)
{
	if (id < LADOS_CANT)
	{
		motConfig[id].dutyDesired = duty;
		Mot_WriteSyncDuty(motConfig+id, duty);
	}
}


void Mot_WriteDutyDesired(motId_t id, motDuty_t duty)
{
	if (id < LADOS_CANT)
	{
		motConfig[id].dutyDesired = duty;
	}
}


motDuty_t Mot_ReadDutyNow(motId_t id)
{
	motDuty_t dutyNow = MOT_DUTY_OFF;
	if (id < LADOS_CANT)
	{
		dutyNow = motConfig[id].dutyNow;
	}
	return dutyNow;
}


motDuty_t Mot_ReadDutyDesired(motId_t id)
{
	motDuty_t dutyDesired = MOT_DUTY_OFF;
	if (id < LADOS_CANT)
	{
		dutyDesired = motConfig[id].dutyDesired;
	}
	return dutyDesired;
}


void Mot_UpdateDuty(void)
{
	motDuty_t dutyDesired, dutyNow;
	motConfig_t *mot;

	for(mot=motConfig ; mot<motConfig+LADOS_CANT ; ++mot)
	{
		dutyDesired = mot->dutyDesired;
		dutyNow = mot->dutyNow;

		if (mot->dutyDesired != dutyNow)
		{ // necesita actualizar
			dutyNow += (dutyDesired > dutyNow)? MOT_SMOOTH_DUTY_UPDATE_STEP : -MOT_SMOOTH_DUTY_UPDATE_STEP;
			Mot_WriteSyncDuty(mot, dutyNow);
		}
	}
}



/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


static void Mot_WriteSyncDuty(motConfig_t *mot, motDuty_t duty)
{
	if (duty >= 0)
	{
		if (duty > MOT_DUTY_MAX_POS)
			duty = MOT_DUTY_MAX_POS;
		PWM_SetDuty(mot->pinNum[MOT_TERM_PLUS], duty);
		Pin_Clear(mot->pinNum[MOT_TERM_MINUS]);
	}
	else
	{
		if (duty < MOT_DUTY_MAX_NEG)
			duty = MOT_DUTY_MAX_NEG;
		PWM_SetDuty(mot->pinNum[MOT_TERM_PLUS], PWM_DUTY_MAX_VAL+duty);
		Pin_Set(mot->pinNum[MOT_TERM_MINUS]);
	}
	mot->dutyNow = duty;
}
