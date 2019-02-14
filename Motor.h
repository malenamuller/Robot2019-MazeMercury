/***************************************************************************//**
  @file     Motor.h
  @brief    Motor Driver
  @author   Nicolás Magliola
 ******************************************************************************/

#ifndef _MOTOR_H_
#define _MOTOR_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "common.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define MOT_DUTY_MAX_POS	256
#define MOT_DUTY_OFF		0
#define MOT_DUTY_MAX_NEG	-256


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum {
	MOT_ID_IZQ = IZQUIERDA,
	MOT_ID_DER = DERECHA,

	MOT_ID_CANT = LADOS_CANT
} motId_t;

typedef int16_t motDuty_t;


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
void Mot_Init (void);

void Mot_WriteDutyNow(motId_t id, motDuty_t duty);
motDuty_t Mot_ReadDutyNow(motId_t id);

void Mot_WriteDutyDesired(motId_t id, motDuty_t duty);
motDuty_t Mot_ReadDutyDesired(motId_t id);

void Mot_UpdateDuty(void);


/*******************************************************************************
 ******************************************************************************/

#endif // _MOTOR_H_
