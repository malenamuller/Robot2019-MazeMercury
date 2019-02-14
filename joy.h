/*
 * joy.h
 *
 *  Created on: 21/8/2017
 *      Author: Daniel Jacoby, Nicolas Magliola
 */

#ifndef SOURCES_JOY_H_
#define SOURCES_JOY_H_


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
	JOY_ID_AXIS_LX,
	JOY_ID_AXIS_LY,
	JOY_ID_AXIS_RX,
	JOY_ID_AXIS_RY,
	JOY_ID_AXIS_PADX,
	JOY_ID_AXIS_PADY,
	JOY_ID_BUT_GREEN1,
	JOY_ID_BUT_RED2,
	JOY_ID_BUT_BLUE3,
	JOY_ID_BUT_PINK4,
	JOY_ID_BUT_L1,
	JOY_ID_BUT_R1,
	JOY_ID_BUT_L2,
	JOY_ID_BUT_R2,
	JOY_ID_BUT_SELECT,
	JOY_ID_BUT_START,
	JOY_ID_BUTT10,
	JOY_ID_BUTT11,

	JOY_CANT_IDS
}joyCtrId_t;

#define JOY_ID_BUTT0		JOY_ID_BUT_GREEN1
#define JOY_ID_AXIS0		JOY_ID_AXIS_LX

#define JOY_AXIS_CANT		6
#define JOY_BUTTONS_CANT	12


#define JOY_AXIS_VAL_FULL_UPLEFT	0
#define JOY_AXIS_VAL_FULL_DOWNRIGTH	255
#define JOY_AXIS_VAL_RELEASE		127

#define JOY_BUTT_PRESS				1
#define JOY_BUTT_RELEASE			0

//AGREGO ESTO: completar con diametro de la rueda y los pasos con los que se va a usar el motor
//(ver si estas dos lineas quedan aca o en otro archivo)
#define DIAMETRO_RUEDA			7.5
#define PASOS					1
//HASTA ACA!!

typedef void(*joyActionFun_t)(int value);


/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void joy_Init(void);
void joy_RegisterCallbackAction(joyCtrId_t id, joyActionFun_t funPtr);

uint8_t joy_ParseMessage(unsigned char *ptr_joy_message);
void joy_ResetCtrlVal(void);
int16_t joy_ReadCtrlVal(joyCtrId_t id);


/*******************************************************************************
 ******************************************************************************/


#endif /* SOURCES_JOY_H_ */
