/***************************************************************************//**
  @file     Comm.c
  @brief    Communication parser
  @author   Daniel Jacoby, Nicolás Magliola
 ******************************************************************************/


/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "Comm.h"

#include "UART.h"

#include <string.h>


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define COMM_ECHO_ENA	1

#define STX 0x02
#define ETX 0x03
#define ENQ 0x05
#define ACK 0x06
#define NACK 0x15

#define COMM_JOY_MSG_LEN_MAX	64


#define UART_debugPrintStr(str)	UART_WriteMsg(UART_TERMINAL, str, strlen(str))


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static uint8_t parse_incomming_data(char data);


/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static char rx_msg[COMM_JOY_MSG_LEN_MAX];
static char tx_msg[] = {STX, 'A', 'A', ETX};

static char joy_msg[COMM_JOY_MSG_LEN_MAX];
static bool new_joy_msg = FALSE;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


void Comm_Init (void)
{
	UART_Init();
	//UART_debugPrintStr("ARRANCAMOS TERMINAL UART\n");
	//UART_WriteMsg(UART_WIFI, "ARRANCAMOS WIFI UART\n", 21);
}


void Comm_Update (void)
{
	char c;

	//UART_WriteMsg(UART_WIFI, "ACTUALIZANDO...\n", 16);


	while(UART_ReadMsg(UART_WIFI, &c, 1))
	{ // New data arrived
		if (parse_incomming_data(c))
		{ // Complete msg arrived!
			{ //Send an answer
				if (tx_msg[2] < 'Z')
					++(tx_msg[2]);
				else
					tx_msg[2] = 'A';
				UART_WriteMsg(UART_WIFI, tx_msg, 4);
			}

			if (*rx_msg == 'J')
			{ // Joystick message
				strcpy(joy_msg, rx_msg);	 // Copy received msg
				new_joy_msg = TRUE;			 //Se indica que se recibió un mensaje del joystick
			}
			else if (*rx_msg == 'D')
			{ // Debug message
				UART_debugPrintStr(">>> NodeMCU: "); //Send an answer
				UART_debugPrintStr(rx_msg+1);
				UART_debugPrintStr("\n");
			}
			else
			{
				// mensaje desconocido, no hago nada
			}
		}
	}
}



uint8_t Comm_IsNewJoyMsg (void)
{
	return new_joy_msg;
}


uint8_t Comm_ReadJoyMsg (char * str)
{
	uint8_t msg_copied = FALSE;
	if (new_joy_msg && str)		//new_joy_msg se puso previamente en TRUE, si se recibió un mensaje.
	{
		strcpy(str, joy_msg);	//Ahora el mensaje recibido está también en str
		new_joy_msg = FALSE;
		msg_copied = TRUE;		//Si había un new_joy_msg, se lo copia y se avisa que se copió.
	}
	return msg_copied;
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


static uint8_t parse_incomming_data(char data)		// tiny FSM
{
	enum STATE {S_ILDE,S_GET_DATA};
	static enum STATE rx_state=S_ILDE;
	static int i=0;
	uint8_t message_arrived = FALSE;


	if(data==STX)
	{ // forced FSM reset
		rx_state=S_GET_DATA;
		i=0;
	}
	else if(rx_state==S_GET_DATA)
	{
		if(data==ETX)			//End of Frame
		{
			rx_state=S_ILDE;
			rx_msg[i]='\0';		//Payload Terminator
			message_arrived=TRUE;
		}
		else // valid ASCII
		{
			rx_msg[i]=data;			//Store Payload
			if (i<COMM_JOY_MSG_LEN_MAX)
				++i;
		}
	}

	return message_arrived;
}

