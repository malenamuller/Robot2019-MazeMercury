/***************************************************************************//**
  @file     step.h
  @brief    Driver for stepper motors
  @author   Nicolás Magliola
 ******************************************************************************/

#ifndef _STEP_H_
#define _STEP_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "common.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


#define STEP_PERIOD_MIN_US		100
#define STEP_PERIOD_MAX_US		165000

 
#define STEP_NCYCLES_MIN	1
#define STEP_NCYCLES_MAX	65534       
#define STEP_NCYCLES_INF    65535

 



/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum {
	STEPID_MOT_IZQ,
	STEPID_MOT_DER,
	CANT_STEPIDS
} stepId_t;


typedef enum {
	STEPDIR_ADELANTE,
	STEPDIR_ATRAS,
	CANT_STEPDIR,
} stepDir_t;

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
void Step_Init (void);


void Step_Run(stepId_t id, uint32_t period_us, stepDir_t dir);
uint16_t Step_GetPeriod(stepId_t id);

void Step_Stop(stepId_t id);



/*******************************************************************************
 ******************************************************************************/

#endif // _STEP_H_
