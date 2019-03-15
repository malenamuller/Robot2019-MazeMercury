



#include "joy.h"
#include "Comm.h"
#include "Motor.h"

//#include "Pin.h"
//#include "adc16.h"
#include "Timer.h"
#include "UART.h"

#include "step.h" //AGREGO ESTO!
#include "servo.h" //AGREGO ESTO!

//#include <stdio.h>
//#include <string.h>



#define DEBUG_APP_ENA


static char joy_msg[64];
static uint8_t timerJoy;

//Callback functions declarations
static void fbut1(int value);
static void fbut3(int value);
static void fbut4(int value);
static void fbutPadX(int value);


void App_Init (void)
{
#ifdef DEBUG_APP_ENA
	UART_Init();
	UART_WriteMsg(UART_TERMINAL, "Sarah is ALIVE!!\n", 17);
#endif // DEBUG_APP_ENA
	Timer_Init();
	timerJoy = Timer_GetId();
	Timer_SetTimeout(timerJoy, 1000);


	Mot_Init();
	joy_Init();
	Comm_Init();
	Step_Init();
	Servo_Init();


	   //Callbacks Joy
	static void (*pfbut)(int);

	pfbut = fbut1;
	joy_RegisterCallbackAction(JOY_ID_BUT_GREEN1, pfbut);

	pfbut = fbut3;
	joy_RegisterCallbackAction(JOY_ID_BUT_BLUE3, pfbut);

	pfbut = fbut4;
	joy_RegisterCallbackAction(JOY_ID_BUT_PINK4, pfbut);

	pfbut = fbutPadX;
		joy_RegisterCallbackAction(JOY_ID_AXIS_PADX, pfbut);
}


/*
  void Step_Run(stepId_t id, uint32_t period_us);
uint16_t Step_GetPeriod(stepId_t id);

void Step_Stop(stepId_t id);
 */

void App_Run (void) //Esto vendría a ser como un main
{
	Comm_Update();	//Si se recibe algo del joystick, pone new_joy_msg en true.

	//Step_Run(STEPID_MOT_IZQ, 2500, STEPDIR_ADELANTE);
	//Step_Run(STEPID_MOT_DER, 2500, STEPDIR_ADELANTE);
	if (Comm_ReadJoyMsg(joy_msg) && joy_ParseMessage(joy_msg)) //Si new_joy_msg == true, ReadyJoyMsg copia el msg recibido y devuelve true.
	{
#ifdef DEBUG_APP_ENA
		UART_WriteMsg(UART_TERMINAL, joy_msg, strlen(joy_msg));
		UART_WriteMsg(UART_TERMINAL, "\n", 1);
#endif // DEBUG_APP_ENA
		Timer_SetTimeout(timerJoy, 1000);
	}
	else if (Timer_IsExpired(timerJoy))
	{
#ifdef DEBUG_APP_ENA
		UART_WriteMsg(UART_TERMINAL, "TIMEOUT\n", 8);
#endif // DEBUG_APP_ENA

		joy_ResetCtrlVal();
		Timer_SetTimeout(timerJoy, 1000);
	}


	//////////////////AGREGO DESDE ACA PARA PROBAR!!!!
	//Step_Run(stepId_t id, uint32_t period_us);
	//STEPID_MOT_IZQ, //esto es un id
	//STEPID_MOT_DER,
	//void Servo_SetWidth(servoId_t id, uint16_t ton_us);
	//SERVOID_DIRECTION



	//////////////////HASTA ACA PARA PROBAR!!!!
	Timer_DelayMS(20);
}


static void fbut1(int value)			// Boton 1 verde: retrocede.
{
	//USAR EL value que se recibe, en lugar de 2500, para hacerlo analogico
 Step_Run(STEPID_MOT_IZQ, 2500, STEPDIR_ATRAS);
 Step_Run(STEPID_MOT_DER, 2500, STEPDIR_ATRAS);
}

static void fbut3(int value)			// Boton 3 Celeste: avanza hacia adelante.
{
	 Step_Run(STEPID_MOT_IZQ, 2500, STEPDIR_ADELANTE);
	 Step_Run(STEPID_MOT_DER, 2500, STEPDIR_ADELANTE);
}

static void fbut4(int value)			// Boton 4 Rosa: frena.
{
	 Step_Run(STEPID_MOT_IZQ, 0, STEPDIR_ADELANTE);
	 Step_Run(STEPID_MOT_DER, 0, STEPDIR_ADELANTE);
}


//Recibe valor correspondiente a izq o der en value
//0: izq
//127: adelante (no se presionó nada)
//255: derecha
static void fbutPadX(int value)			// Boton 4 Rosa: frena.
{
	switch (value)
	{
		case 0: //HACER UN DEFINE PARA ESTOS VALORES ASI LOS PODEMOS MODIFICAR FACILMENTE:
			Servo_SetWidth(SERVOID_DIRECTION, 2270); //2,27ms = 2270us (HACER DEFINE PARA ESTOS VALORES!!!!!!)
			break;
		case 127:
			Servo_SetWidth(SERVOID_DIRECTION, 1330); //1,33ms = 1330us
			break;
		case 255:
			Servo_SetWidth(SERVOID_DIRECTION, 460); //0,46ms = 460us
			break;
	}
}


