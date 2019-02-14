/*
 * joy.c
 *
 *  Created on: 21/8/2017
 *      Author: dany
 */


/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "joy.h"

#include <string.h>
#include <stdlib.h>

// For default actions
#include "Pin.h"
#include "Motor.h"
#include "servo.h"
#include "step.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

//#define JOY_DEBUG_ENA


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

//Lo siguiente es un arreglo de punteros a funciones
//porque joyActionFun_t es un tipo de datos
//definido como un puntero a una func que recibe int y no devuelve nada:
static joyActionFun_t joyActionList[JOY_CANT_IDS] = { // default actions, can be changed
	NULL,	// JOY_ID_AXIS_LX, EJE X DEL ANALOGICO IZQUIERDO
	NULL,		// JOY_ID_AXIS_LY, EJE Y DEL ANALOGICO IZQUIERDO
	NULL,		// JOY_ID_AXIS_RX,
	NULL,		// JOY_ID_AXIS_RY,
	NULL,		// JOY_ID_AXIS_PADX,  FLECHAS IZQ Y DERECHA
	NULL, 		// JOY_ID_AXIS_PADY, FLECHAS ARRIBA Y ABAJO
	NULL,		// JOY_ID_BUT_GREEN1,
	NULL, 		// JOY_ID_BUT_RED2,
	NULL,		// JOY_ID_BUT_BLUE3,
	NULL,		// JOY_ID_BUT_PINK4,
	NULL,		// JOY_ID_BUT_L1,
	NULL,		// JOY_ID_BUT_R1,
	NULL,		// JOY_ID_BUT_L2,
	NULL,		// JOY_ID_BUT_R2,
	NULL,		// JOY_ID_BUT_SELECT,
	NULL,		// JOY_ID_BUT_START,
	NULL,		// JOY_ID_BUTT10,
	NULL		// JOY_ID_BUTT11,
};

static int16_t joyCtrlVal[JOY_CANT_IDS];



/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void joy_Init(void)
{
	Mot_Init();
	Pin_Init();

#ifdef JOY_DEBUG_ENA
	UART_Init();
#endif // JOY_DEBUG_ENA

	joy_ResetCtrlVal();
}


void joy_RegisterCallbackAction(joyCtrId_t id, joyActionFun_t funPtr)
{
	if (id < JOY_CANT_IDS && funPtr != NULL)
	{
		joyActionList[id] = funPtr;
	}
}


/*
 * Message must be:
 * "Jk;n;n;...;n"
 * donde
 * - J es el ASCII de 'J'
 * - k es una letra ASCII mayúscula
 */
uint8_t joy_ParseMessage(unsigned char *ptr_joy_message) //Recibe el msg que fue previamente recibido proveniente del joystick
{
	const char *delimiter=";:,";
	static char lastLetterCnt = 'a';
	char *pt;
	int joyMsgValues[JOY_CANT_IDS];
	joyCtrId_t i;


	// Parse received string
	pt = strtok(ptr_joy_message, delimiter); //Control type: Axis or Button

	if (!pt || pt[0] != 'J' || strlen(pt) != 2) // Validate message header
		return FALSE;

	if (pt[1] < 'A' || pt[1] > 'Z' || pt[1] == lastLetterCnt) // Validate message counter
		return FALSE;
	lastLetterCnt = pt[1];

	for (i=0 ; i<JOY_CANT_IDS ; ++i)
	{
		pt = strtok(NULL, delimiter);
		if (!pt || strlen(pt) == 0) // Validate integer number
			return FALSE;
		joyMsgValues[i] = atoi(pt);
	}

	pt = strtok(NULL, delimiter);
	//if ( !((pt == NULL) || (*pt == '\0')) ) // Validate end-of-message
	if (pt && *pt != '\0') // Validate end-of-message
		return FALSE;

	// Message is valid! copy it and trigger callback if event occurred
	for (i=0 ; i<JOY_CANT_IDS ; ++i)
	{
		if (joyCtrlVal[i] != joyMsgValues[i])
		{
			joyCtrlVal[i] = joyMsgValues[i];
			if (joyActionList[i])
				joyActionList[i](joyMsgValues[i]);	//Llama al callback correspondiente al botón presionado en el joywtick.
		}
	}

	return TRUE;

#ifdef JOY_DEBUG_ENA
			printf("JOYSTICK >>>>>> Call Action %d\n", joyId);
#endif // JOY_DEBUG_ENA
}


void joy_ResetCtrlVal(void)
{
	int i;
	for (i=JOY_ID_BUTT0 ; i<JOY_ID_BUTT0+JOY_BUTTONS_CANT ; ++i)
		joyCtrlVal[i] = JOY_BUTT_RELEASE;
	for (i=JOY_ID_AXIS0 ; i<JOY_ID_AXIS0+JOY_AXIS_CANT ; ++i)
		joyCtrlVal[i] = JOY_AXIS_VAL_RELEASE;
	
	//desplazamiento = 0;//COMENTO ESTO
	//direccion = 0;//COMENTO ESTO
	//velocidad = 0;//COMENTO ESTO

	//updateMotorsConfig(); //COMENTO ESTO
}


int16_t joy_ReadCtrlVal(joyCtrId_t id)
{
	int16_t val = 0;
	if (id < JOY_CANT_IDS)
	{
		val = joyCtrlVal[id];
	}
	return val;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


