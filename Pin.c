
/***************************************************************************//**
  @file     Pin.c
  @brief    Simple GPIO Pin services
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "Pin.h"

#include "PORT.h"
#include "GPIO.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

//#define PIN_PORT_SIMPLE		.portConfig.FIELD={0, 0, 0, 0, 0, 0, true, 0, PORT_mGPIO, 0, 0, PORT_eDisabled, 0, 0, 0}
#define PIN_PORT_SIMPLE		{0, 0, 0, 0, 0, 0, true, 0, PORT_mGPIO, 0, 0, PORT_eDisabled, 0, 0, 0}

#define PIN_OUT_PUSHPULL	PIN_PORT_SIMPLE

#define PIN_IN_PULLUP		{1, 1, 0, 0, 0, 0, false, 0, PORT_mGPIO, 0, 0, PORT_eDisabled, 0, 0, 0}
#define PIN_IN_PULLDOWN		{0, 1, 0, 0, 0, 0, false, 0, PORT_mGPIO, 0, 0, PORT_eDisabled, 0, 0, 0}
#define PIN_IN_NOPULL		{0, 0, 0, 0, 0, 0, false, 0, PORT_mGPIO, 0, 0, PORT_eDisabled, 0, 0, 0}


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
	PORT_t port;
	GPIO_t gpio;
	uint32_t num;
	PCRstr portConfig;
	GPIODirection_t dir;
	GPIOData_t onLevel;
	GPIOData_t initLevel;
} pinConfig_t;


/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static const pinConfig_t pinList[CANT_PINS] = {
	{ PORTB, PTB, 22, PIN_PORT_SIMPLE, GPIO__OUT, GPIO__LO, GPIO__HI }, // PIN_LED_RED
	{ PORTB, PTB, 21, PIN_PORT_SIMPLE, GPIO__OUT, GPIO__LO, GPIO__HI }, // PIN_LED_BLUE
	{ PORTE, PTE, 26, PIN_PORT_SIMPLE, GPIO__OUT, GPIO__LO, GPIO__HI }, // PIN_LED_GREEN

	{ PORTC, PTC, 6, PIN_IN_PULLUP, GPIO__IN, GPIO__LO, GPIO__HI }, // PIN_SW2
	{ PORTA, PTA, 4, PIN_IN_PULLUP, GPIO__IN, GPIO__LO, GPIO__HI }, // PIN_SW3

	{ PORTC, PTC, 2, PIN_PORT_SIMPLE, GPIO__OUT, GPIO__HI, GPIO__LO }, // PIN_MOT_IZQ_MINUS
	{ PORTC, PTC, 8, PIN_PORT_SIMPLE, GPIO__OUT, GPIO__HI, GPIO__LO }, // PIN_MOT_DER_MINUS
};

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void Pin_Init (void)
{
	const pinConfig_t* pin;
	
	for (pin=pinList ; pin<pinList+CANT_PINS ; ++pin)
	{
		PORT_Configure2(pin->port, pin->num, pin->portConfig);
		GPIO_SetDirPin(pin->gpio, pin->num, pin->dir);
		if (pin->dir == GPIO__OUT)
		{
			GPIO_WritePin(pin->gpio, pin->num, pin->initLevel);
		}
	}
}


void Pin_Write (pinName_t pin, bool val)
{
	if (pin<CANT_PINS && pinList[pin].dir==GPIO__OUT)
	{
		GPIO_WritePin(pinList[pin].gpio, pinList[pin].num, !val == !(pinList[pin].onLevel));
	}
}

void Pin_Set (pinName_t pin)
{
	if (pin<CANT_PINS && pinList[pin].dir==GPIO__OUT)
	{
		if (pinList[pin].onLevel)
			GPIO_SetPin(pinList[pin].gpio, pinList[pin].num);
		else
			GPIO_ClearPin(pinList[pin].gpio, pinList[pin].num);
	}
}

void Pin_Clear (pinName_t pin)
{
	if (pin<CANT_PINS && pinList[pin].dir==GPIO__OUT)
	{
		if (pinList[pin].onLevel)
			GPIO_ClearPin(pinList[pin].gpio, pinList[pin].num);
		else
			GPIO_SetPin(pinList[pin].gpio, pinList[pin].num);
	}
}

void Pin_Toggle (pinName_t pin)
{
	if (pin<CANT_PINS && pinList[pin].dir==GPIO__OUT)
	{
		GPIO_TogglePin(pinList[pin].gpio, pinList[pin].num);
	}
}

bool Pin_Read (pinName_t pin)
{
	bool rta = false;
	if (pin<CANT_PINS)
	{
		rta = !(pinList[pin].onLevel) == !(GPIO_ReadPin(pinList[pin].gpio, pinList[pin].num));
	}
	return rta;
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

