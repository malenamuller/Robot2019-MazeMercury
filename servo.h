/***************************************************************************//**
  @file     servo.h
  @brief    Driver for servo motors
  @author   Nicolás Magliola
 ******************************************************************************/

#ifndef _SERVO_H_
#define _SERVO_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "common.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define SERVO_TON_US_MIN	500
#define SERVO_TON_US_MAX	2500


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum {
	SERVOID_DIRECTION,
	CANT_SERVOIDS
} servoId_t;


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
void Servo_Init (void);

/**
 * @brief TODO: completar descripcion
 * @param param1 Descripcion parametro 1
 * @param param2 Descripcion parametro 2
 * @return Descripcion valor que devuelve
*/
void Servo_SetWidth(servoId_t id, uint16_t ton_us);

/**
 * @brief TODO: completar descripcion
 * @param param1 Descripcion parametro 1
 * @param param2 Descripcion parametro 2
 * @return Descripcion valor que devuelve
*/
uint16_t Servo_GetWidth(servoId_t id);


/*******************************************************************************
 ******************************************************************************/

#endif // _SERVO_H_
