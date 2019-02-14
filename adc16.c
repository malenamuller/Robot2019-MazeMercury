/***************************************************************************//**
  @file     adc16.c
  @brief    ADC driver for K64F
  @author   Nicolás Magliola
 ******************************************************************************/

#include "adc16.h"

#include "PORT.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define TWO_POW_NUM_OF_CAL		(1 << 4)

#define ADC_RESOLITION_BITS		ADC_b10

#define ADC_PIN_PORT_CONFIG		{0, 0, 0, 0, 0, 0, false, 0, PORT_mAnalog, 0, 0, PORT_eDisabled, 0, 0, 0}


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum { ADC_b8, ADC_b12, ADC_b10, ADC_b16 } ADCBits_t;
typedef enum { ADC_c24, ADC_c16, ADC_c10, ADC_c6, ADC_c4 } ADCCycles_t;

typedef struct {
	PORT_t port;
	uint32_t num;
	PCRstr portConfig;
	ADC_t adc;
	ADCChannel_t ch;
} adcConfig_t;


/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*
static void ADC_SetResolution (ADC_t, ADCBits_t);
static ADCBits_t ADC_GetResolution (ADC_t);
*/
static void ADC_SetCycles (ADC_t adc, ADCCycles_t cycles);
/*
static ADCCycles_t ADC_GetCycles (ADC_t);
static void ADC_SetHardwareAverage (ADC_t, ADCTaps_t);
static ADCTaps_t ADC_GetHardwareAverage (ADC_t);
*/

static bool ADC_Calibrate (ADC_t adc);


/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static const adcConfig_t adcList[CANT_ADCS] = {
	{ PORTB, 2, ADC_PIN_PORT_CONFIG, ADC0, 12 }, // ADC_POTE
};


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

bool ADC_interrupt[2] = {false, false};
static ADCData_t adcLastRead;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void ADC_Init (void)
{
	static bool yaInit = false;
	if (yaInit)
		return;

	const adcConfig_t* pin;

	for (pin=adcList ; pin<adcList+CANT_ADCS ; ++pin)
	{
		PORT_Configure2(pin->port, pin->num, pin->portConfig);
	}

	SIM_SCGC6 |= SIM_SCGC6_ADC0_MASK; // Enable ADC0 module (ADC0 Clock Gate Control)
//	SIM_SCGC3 |= SIM_SCGC3_ADC1_MASK;

//	NVIC_EnableIRQ(ADC0_IRQn);
//	ADC_SetInterruptMode(ADC0, true);

//	NVIC_EnableIRQ(ADC1_IRQn);
//	ADC_SetInterruptMode(ADC1, true);

	ADC0->CFG1 = ADC_CFG1_ADIV(0x00) | ADC_CFG1_MODE(ADC_RESOLITION_BITS); // Configure clock & timing and # conversion's bits
//	ADC1->CFG1 = ADC_CFG1_ADIV(0x00);

	ADC_SetCycles(ADC0, ADC_c10);

	ADC_Calibrate(ADC0); // Calibra una serie de parametros, incluyendo un promedio de 16 veces del ADC para obtener el valor final
}

ADCData_t ADC_ReadBlocking (adcId_t id)
{
	ADCData_t rta = 0;
	if (id < CANT_ADCS)
	{
		rta = ADC_SyncGetData(adcList[id].adc, adcList[id].ch, ADC_mA);
	}
	return rta;
}


void ADC_SetInterruptMode (ADC_t adc, bool mode)
{
	if (adc == ADC0)
		ADC_interrupt[0] = mode;
	else if (adc == ADC1)
		ADC_interrupt[1] = mode;
}

bool ADC_IsInterruptPending (ADC_t adc)
{
	return adc->SC1[0] & ADC_SC1_COCO_MASK;
}

void ADC_ClearInterruptFlag (ADC_t adc)
{
	adc->SC1[0] = 0x00;
}


void ADC_Start (ADC_t adc, ADCChannel_t channel, ADCMux_t mux)
{
	adc->CFG2 = (adc->CFG2 & ~ADC_CFG2_MUXSEL_MASK) | ADC_CFG2_MUXSEL(mux);

	if (adc == ADC0)
		adc->SC1[0] = ADC_SC1_AIEN(ADC_interrupt[0]) | ADC_SC1_ADCH(channel);
	else if (adc == ADC1)
		adc->SC1[0] = ADC_SC1_AIEN(ADC_interrupt[1]) | ADC_SC1_ADCH(channel);
}

bool ADC_IsReady (ADC_t adc)
{
	return adc->SC1[0] & ADC_SC1_COCO_MASK;
}

ADCData_t ADC_GetData (ADC_t adc)
{
	return adc->R[0];
}


ADCData_t ADC_SyncGetData (ADC_t adc, ADCChannel_t channel, ADCMux_t mux)
{
	ADCData_t data;

	ADC_Start(adc, channel, mux);

	while(!ADC_IsReady(adc)) {};

	data = ADC_GetData(adc);

	return data;
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

__ISR__ ADC0_IRQHandler (void)
{
	adcLastRead = ADC0->R[0];
	++adcLastRead;
}


/*
static void ADC_SetResolution (ADC_t adc, ADCBits_t bits)
{
	adc->CFG1 = (adc->CFG1 & ~ADC_CFG1_MODE_MASK) | ADC_CFG1_MODE(bits);
}

static ADCBits_t ADC_GetResolution (ADC_t adc)
{
	return adc->CFG1 & ADC_CFG1_MODE_MASK;
}
*/

static void ADC_SetCycles (ADC_t adc, ADCCycles_t cycles)
{
	if (cycles & ~ADC_CFG2_ADLSTS_MASK)
	{
		adc->CFG1 &= ~ADC_CFG1_ADLSMP_MASK;
	}
	else
	{
		adc->CFG1 |= ADC_CFG1_ADLSMP_MASK;
		adc->CFG2 = (adc->CFG2 & ~ADC_CFG2_ADLSTS_MASK) | ADC_CFG2_ADLSTS(cycles);
	}
}

/*
static ADCCycles_t ADC_GetSCycles (ADC_t adc)
{
	if (adc->CFG1 & ADC_CFG1_ADLSMP_MASK)
		return ADC_c4;
	else
		return adc->CFG2 & ADC_CFG2_ADLSTS_MASK;
}


static void ADC_SetHardwareAverage (ADC_t adc, ADCTaps_t taps)
{
	if (taps & ~ADC_SC3_AVGS_MASK)
	{
		adc->SC3 &= ~ADC_SC3_AVGE_MASK;
	}
	else
	{
		adc->SC3 |= ADC_SC3_AVGE_MASK;
		adc->SC3 = (adc->SC3 & ~ADC_SC3_AVGS_MASK) | ADC_SC3_AVGS(taps);
	}
}

static ADCTaps_t ADC_GetHardwareAverage (ADC_t adc)
{
	if (adc->SC3 & ADC_SC3_AVGE_MASK)
		return ADC_t1;
	else
		return adc->SC3 & ADC_SC3_AVGS_MASK;
}
*/

static bool ADC_Calibrate (ADC_t adc)
{
	int32_t  Offset		= 0;
	uint32_t Minus	[7] = {0,0,0,0,0,0,0};
	uint32_t Plus	[7] = {0,0,0,0,0,0,0};
	uint8_t  i;
	uint32_t scr3;

	/// SETUP
	adc->SC1[0] = 0x1F;
	scr3 = adc->SC3;
	adc->SC3 &= (ADC_SC3_AVGS(0x03) | ADC_SC3_AVGE_MASK);

	/// INITIAL CALIBRATION
	adc->SC3 &= ~ADC_SC3_CAL_MASK;
	adc->SC3 |=  ADC_SC3_CAL_MASK;
	while (!(adc->SC1[0] & ADC_SC1_COCO_MASK));
	if (adc->SC3 & ADC_SC3_CALF_MASK)
	{
		adc->SC3 |= ADC_SC3_CALF_MASK;
		return false;
	}
	adc->PG  = (0x8000 | ((adc->CLP0+adc->CLP1+adc->CLP2+adc->CLP3+adc->CLP4+adc->CLPS) >> (1 + TWO_POW_NUM_OF_CAL)));
	adc->MG  = (0x8000 | ((adc->CLM0+adc->CLM1+adc->CLM2+adc->CLM3+adc->CLM4+adc->CLMS) >> (1 + TWO_POW_NUM_OF_CAL)));

	// FURTHER CALIBRATIONS
	for (i = 0; i < TWO_POW_NUM_OF_CAL; i++)
	{
		adc->SC3 &= ~ADC_SC3_CAL_MASK;
		adc->SC3 |=  ADC_SC3_CAL_MASK;
		while (!(adc->SC1[0] & ADC_SC1_COCO_MASK));
		if (adc->SC3 & ADC_SC3_CALF_MASK)
		{
			adc->SC3 |= ADC_SC3_CALF_MASK;
			return 1;
		}
		Offset += (short)adc->OFS;
		Plus[0] += (unsigned long)adc->CLP0;
		Plus[1] += (unsigned long)adc->CLP1;
		Plus[2] += (unsigned long)adc->CLP2;
		Plus[3] += (unsigned long)adc->CLP3;
		Plus[4] += (unsigned long)adc->CLP4;
		Plus[5] += (unsigned long)adc->CLPS;
		Plus[6] += (unsigned long)adc->CLPD;
		Minus[0] += (unsigned long)adc->CLM0;
		Minus[1] += (unsigned long)adc->CLM1;
		Minus[2] += (unsigned long)adc->CLM2;
		Minus[3] += (unsigned long)adc->CLM3;
		Minus[4] += (unsigned long)adc->CLM4;
		Minus[5] += (unsigned long)adc->CLMS;
		Minus[6] += (unsigned long)adc->CLMD;
	}
	adc->OFS = (Offset >> TWO_POW_NUM_OF_CAL);
	adc->PG  = (0x8000 | ((Plus[0] +Plus[1] +Plus[2] +Plus[3] +Plus[4] +Plus[5] ) >> (1 + TWO_POW_NUM_OF_CAL)));
	adc->MG  = (0x8000 | ((Minus[0]+Minus[1]+Minus[2]+Minus[3]+Minus[4]+Minus[5]) >> (1 + TWO_POW_NUM_OF_CAL)));
	adc->CLP0 = (Plus[0] >> TWO_POW_NUM_OF_CAL);
	adc->CLP1 = (Plus[1] >> TWO_POW_NUM_OF_CAL);
	adc->CLP2 = (Plus[2] >> TWO_POW_NUM_OF_CAL);
	adc->CLP3 = (Plus[3] >> TWO_POW_NUM_OF_CAL);
	adc->CLP4 = (Plus[4] >> TWO_POW_NUM_OF_CAL);
	adc->CLPS = (Plus[5] >> TWO_POW_NUM_OF_CAL);
	adc->CLPD = (Plus[6] >> TWO_POW_NUM_OF_CAL);
	adc->CLM0 = (Minus[0] >> TWO_POW_NUM_OF_CAL);
	adc->CLM1 = (Minus[1] >> TWO_POW_NUM_OF_CAL);
	adc->CLM2 = (Minus[2] >> TWO_POW_NUM_OF_CAL);
	adc->CLM3 = (Minus[3] >> TWO_POW_NUM_OF_CAL);
	adc->CLM4 = (Minus[4] >> TWO_POW_NUM_OF_CAL);
	adc->CLMS = (Minus[5] >> TWO_POW_NUM_OF_CAL);
	adc->CLMD = (Minus[6] >> TWO_POW_NUM_OF_CAL);

	/// UN-SETUP
	adc->SC3 = scr3;

	return true;
}

