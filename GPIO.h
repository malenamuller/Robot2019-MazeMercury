/***************************************************************************//**
  @file     +Nombre del archivo (ej: template.h)+
  @brief    +Descripcion del archivo+
  @author   +Nombre del autor (ej: Salvador Allende)+
 ******************************************************************************/

#ifndef _GPIO_H_
#define _GPIO_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

//#include "common.h"
#include "Template/LDM.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define GPIO_IN(x) 	(0 << x)
#define GPIO_OUT(x) (1 << x)

#define GPIO__IN 	0x00000000u
#define GPIO__OUT   0xFFFFFFFFu

#define GPIO__LO 	0x00000000u
#define GPIO__HI   	0xFFFFFFFFu


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef GPIO_Type *GPIO_t; 		  /* PTA, PTB, PTC, PTD, PTE */
typedef uint32_t GPIODirection_t; /* Input: 0, Output: 1 */
typedef uint32_t GPIOData_t;
typedef uint32_t GPIOMask_t;
typedef uint32_t GPIOPin_t;

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
void GPIO_Init (void);

void GPIO_SetDirPort (GPIO_t, GPIOMask_t, GPIODirection_t);
GPIODirection_t GPIO_GetDirPort (GPIO_t, GPIOMask_t);
void GPIO_WritePort (GPIO_t, GPIOMask_t, GPIOData_t);
GPIOData_t GPIO_ReadPort (GPIO_t, GPIOMask_t);
void GPIO_SetPort (GPIO_t, GPIOMask_t);
void GPIO_ClearPort (GPIO_t, GPIOMask_t);
void GPIO_TogglePort (GPIO_t, GPIOMask_t);

void GPIO_SetDirPin (GPIO_t, GPIOPin_t, GPIODirection_t);
GPIODirection_t GPIO_GetDirPin (GPIO_t, GPIOPin_t);
void GPIO_WritePin (GPIO_t, GPIOPin_t, GPIOData_t);
GPIOData_t GPIO_ReadPin (GPIO_t, GPIOPin_t);
void GPIO_SetPin (GPIO_t, GPIOPin_t);
void GPIO_ClearPin (GPIO_t, GPIOPin_t);
void GPIO_TogglePin (GPIO_t, GPIOPin_t);


/*******************************************************************************
 ******************************************************************************/

#endif // _GPIO_H_
