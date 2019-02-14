/***************************************************************************//**
  @file     adc16.h
  @brief    ADC driver for K64F
  @author   Nicolás Magliola
 ******************************************************************************/

#ifndef _ADC16_H_
#define _ADC16_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "common.h"
#include "Template/LDM.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum {
	ADC_POTE,

	CANT_ADCS
} adcId_t;

typedef enum { ADC_t4, ADC_t8, ADC_t16, ADC_t32, ADC_t1 } ADCTaps_t;
typedef enum { ADC_mA, ADC_mB } ADCMux_t;

typedef ADC_Type *ADC_t;
typedef uint8_t ADCChannel_t; /* Channel 0-23 */
typedef uint16_t ADCData_t;


/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize ADC driver
*/
void ADC_Init (void);

ADCData_t ADC_ReadBlocking (adcId_t);


void ADC_SetInterruptMode (ADC_t, bool);
bool ADC_IsInterruptPending (ADC_t);
void ADC_ClearInterruptFlag (ADC_t);

void ADC_Start (ADC_t, ADCChannel_t, ADCMux_t);
bool ADC_IsReady (ADC_t);
ADCData_t ADC_GetData (ADC_t);

ADCData_t ADC_SyncGetData (ADC_t, ADCChannel_t, ADCMux_t);


/*******************************************************************************
 ******************************************************************************/

#endif // _ADC16_H_
