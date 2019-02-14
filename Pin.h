/***************************************************************************//**
  @file     Pin.h
  @brief    Simple GPIO Pin services
  @author   Nicolás Magliola
 ******************************************************************************/

#ifndef _PIN_H_
#define _PIN_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "common.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

 typedef enum {
	PIN_LED_RED,
	PIN_LED_BLUE,
	PIN_LED_GREEN,
	PIN_SW2,
	PIN_SW3,
	PIN_MOT_IZQ_MINUS,
	PIN_MOT_DER_MINUS,
	CANT_PINS 
} pinName_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief TODO: completar descripcion
 * @param param1 Descripcion parametro 1
 * @param param2 Descripcion parametro 2
 * @return Descripcion valor que devuelve
*/
void Pin_Init (void);

void Pin_Write (pinName_t pin, bool val);
void Pin_Set (pinName_t pin);
void Pin_Clear (pinName_t pin);
void Pin_Toggle (pinName_t pin);

bool Pin_Read (pinName_t pin);

/*******************************************************************************
 ******************************************************************************/

#endif // _PIN_H_
