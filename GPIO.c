
/***************************************************************************//**
  @file     GPIO.c
  @brief    GPIO Driver
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "GPIO.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void GPIO_Init (void)
{

}



void GPIO_SetDirPort (GPIO_t gpio, GPIOMask_t mask, GPIODirection_t direction)
{
	gpio->PDDR = (gpio->PDDR & ~mask) | (direction & mask);
}

GPIODirection_t GPIO_GetDirPort (GPIO_t gpio, GPIOMask_t mask)
{
	return gpio->PDDR & mask;
}

void GPIO_WritePort (GPIO_t gpio, GPIOMask_t mask, GPIOData_t data)
{
	gpio->PDOR = (gpio->PDOR & ~mask) | (data & mask);
}

GPIOData_t GPIO_ReadPort (GPIO_t gpio, GPIOMask_t mask)
{
	return gpio->PDIR & mask;
}

void GPIO_SetPort (GPIO_t gpio, GPIOMask_t mask)
{
	gpio->PSOR = mask;
}

void GPIO_ClearPort	(GPIO_t gpio, GPIOMask_t mask)
{
	gpio->PCOR = mask;
}

void GPIO_TogglePort (GPIO_t gpio, GPIOMask_t mask)
{
	gpio->PTOR = mask;
}





void GPIO_SetDirPin (GPIO_t gpio, GPIOPin_t pin, GPIODirection_t direction)
{
	gpio->PDDR = (gpio->PDDR & ~(1<<pin) ) | (direction & (1 << pin));
}

GPIODirection_t GPIO_GetDirPin (GPIO_t gpio, GPIOPin_t pin)
{
	return gpio->PDDR & (1<<pin);
}

void GPIO_WritePin (GPIO_t gpio, GPIOPin_t pin, GPIOData_t data)
{
	if (data)
		gpio->PSOR = (1<<pin);
	else
		gpio->PCOR = (1<<pin);
}

GPIOData_t GPIO_ReadPin (GPIO_t gpio, GPIOPin_t pin)
{
	return gpio->PDIR & (1<<pin);
}

void GPIO_SetPin (GPIO_t gpio, GPIOPin_t pin)
{
	gpio->PSOR = (1<<pin);
}

void GPIO_ClearPin (GPIO_t gpio, GPIOPin_t pin)
{
	gpio->PCOR = (1<<pin);
}

void GPIO_TogglePin (GPIO_t gpio, GPIOPin_t pin)
{
	gpio->PTOR = (1<<pin);
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

