/***************************************************************************//**
  @file     PWM.c
  @brief    PWM Driver
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "PWM.h"

#include "PORT.h"
#include "GPIO.h"
#include "FTM.h"

#include "Template/LDM.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define PWM_PREESCALER_POW	0
#define PWM_MOD_FREQ_HZ		1000U


#define PWM_PREESCALER_VAL	(1U<<PWM_PREESCALER_POW)
#define PWM_COUNTER_FREQ_HZ	(__CORE_CLOCK__/2/PWM_PREESCALER_VAL)
#define PWM_MOD_VAL			(PWM_COUNTER_FREQ_HZ/PWM_MOD_FREQ_HZ)


#define PWM_DUTY_MAX_POW	8
#if PWM_DUTY_MAX_VAL != (1<<PWM_DUTY_MAX_POW)
#error No se correponde PWM_DUTY_MAX_VAL con PWM_DUTY_MAX_POW
#endif // PWM_DUTY_MAX_VAL != (1<<PWM_DUTY_MAX_POW)


#define PWM_PIN_PORT_ALT3		{0, 0, 0, 0, 0, 0, true, 0, PORT_mAlt3, 0, 0, PORT_eDisabled, 0, 0, 0}
#define PWM_PIN_PORT_ALT4		{0, 0, 0, 0, 0, 0, true, 0, PORT_mAlt4, 0, 0, PORT_eDisabled, 0, 0, 0}


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
	PORT_t port;
	uint32_t num;
	PCRstr portConfig;
	FTM_t mod;
	FTMChannel_t ch;
} pwmConfig_t;


/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static const pwmConfig_t pwmList[CANT_PWMID] = {
	{ PORTC, 1, PWM_PIN_PORT_ALT4, FTM0, 0 }, // PWMID_MOT_IZQ_PLUS
	{ PORTC, 3, PWM_PIN_PORT_ALT4, FTM0, 2 }, // PWMID_MOT_DER_PLUS
};


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static int count;


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void PWM_Init (void)
{
	static bool yaInit = false;
	const pwmConfig_t* pwm;

	if (yaInit)
		return;


	FTM_Init();

	// Configure PORT (alt mode)

	for (pwm=pwmList ; pwm<pwmList+CANT_PWMID ; ++pwm)
	{
		PORT_Configure2(pwm->port, pwm->num, pwm->portConfig);
	}


	// Configure FTM: Clock Source, preescaler,
	FTM_SetPrescaler(FTM0, PWM_PREESCALER_POW);	  /// BusClock=sysclk/2= 50MHz y presccaler = 2^PWM_PREESCALER_POW

	count = PWM_MOD_VAL;
	FTM0->CNTIN=0x0000;				  		  /// Valor incial del contador
	FTM0->MOD=PWM_MOD_VAL-1;


	//FTM0 DECAPEN Y COMBINE estan en 0 por defecto, pero por las dudas los inicializo en 0
	FTM3->COMBINE&=~FTM_COMBINE_COMBINE0_MASK;
	FTM3->COMBINE&=~FTM_COMBINE_DECAPEN0_MASK;

	//por defecto tmb esta en 0
	FTM3->SC=(FTM0->SC & ~(FTM_SC_CPWMS_MASK)) | FTM_SC_CPWMS(0);


	// Configure Channel
	for (pwm=pwmList ; pwm<pwmList+CANT_PWMID ; ++pwm)
	{
		FTM_SetWorkingMode(pwm->mod, pwm->ch, FTM_mPulseWidthModulation); // PWM
		FTM_SetOutputCompareEffect (pwm->mod, pwm->ch, FTM_eClear);
		FTM_SetCounter(pwm->mod, pwm->ch, 0);
	}

	FTM_StartClock(FTM0); //Select BusClk
	yaInit = true;
}



void PWM_SetDuty(pwmId_t id, pwmDuty_t duty)
{
	if (id < CANT_PWMID)
	{
		uint32_t count = duty;

		if (count > PWM_DUTY_MAX_VAL)
			count = PWM_DUTY_MAX_VAL;
		count *= PWM_MOD_VAL;
		count += 1<<(PWM_DUTY_MAX_POW-1);
		count >>= PWM_DUTY_MAX_POW;

		FTM_SetCounter(pwmList[id].mod, pwmList[id].ch, count);
	}
}
