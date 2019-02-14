/***************************************************************************//**
  @file     PWM.h
  @brief    PWM Driver
  @author   Nicolás Magliola
 ******************************************************************************/

#ifndef _PWM_H_
#define _PWM_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "common.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define PWM_DUTY_MIN_VAL	0
#define PWM_DUTY_MAX_VAL	256


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum {
	PWMID_MOT_IZQ_PLUS,
	PWMID_MOT_DER_PLUS,

	CANT_PWMID
} pwmId_t;

typedef uint16_t pwmDuty_t;


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
void PWM_Init (void);
void PWM_SetDuty(pwmId_t id, pwmDuty_t duty);
pwmDuty_t PWM_GetDuty(pwmId_t id);


/*******************************************************************************
 ******************************************************************************/

#endif // _PWM_H_
