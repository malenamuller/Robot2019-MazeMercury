/***************************************************************************//**
  @file     board.h
  @brief    Board management
  @author   Nicolás Magliola
 ******************************************************************************/

#ifndef _BOARD_H_
#define _BOARD_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


enum {
	PA,
	PB,
	PC,
	PD,
	PE
};


#define PORTPINNUM2PIN(po, pi)  ((po)<<8 | (pi)) // Ex: PB23 -> 0x0217

#define PIN2PINNUM(id)  ((id) & 0x1F)
#define PIN2PORT(id)  	(((id)>>8) & 0x07)


#ifndef INPUT
#define INPUT           0
#define OUTPUT          1
#define INPUT_PULLUP    2
#define INPUT_PULLDOWN  3
#endif // INPUT

#ifndef LOW
#define LOW     0
#define HIGH    1
#endif // LOW



/***** BOARD defines **********************************************************/


// UART TERMINAL
#define PIN_UART0_RXD   PORTPINNUM2PIN(PC,16) // PTC16
#define PIN_UART0_TXD   PORTPINNUM2PIN(PC,17) // PTC17

// UART WIFI
#define PIN_UART3_RXD   PORTPINNUM2PIN(PB,16) // PTB16
#define PIN_UART3_TXD   PORTPINNUM2PIN(PB,17) // PTC17



// LEDs
#define PIN_LED_RED     PORTPINNUM2PIN(PB,22) // PTB22
#define PIN_LED_GREEN   PORTPINNUM2PIN(PE,26) // PTE26
#define PIN_LED_BLUE    PORTPINNUM2PIN(PB,21) // PTB21

#define LED_ACTIVE      LOW
#define VAL2LED(v)      (!(v) == !LED_ACTIVE)



// Switches
#define PIN_SW2         PORTPINNUM2PIN(PC,6) // PTC6
#define PIN_SW3         PORTPINNUM2PIN(PA,4) // PTA4

#define SW_INPUT        INPUT_PULLUP
#define SW_ACTIVE       LOW
#define SW2VAL(sw)      ((sw) == SW_ACTIVE)



// Servo motor signals
#define PIN_SERVO_DIR   PORTPINNUM2PIN(PC,1) // PTC1


// Servo motor signals
#define PIN_STEPPER_IZQ PORTPINNUM2PIN(PC,10) // PTC10
#define PIN_STEPPER_DER PORTPINNUM2PIN(PC,11) // PTC11






/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/



/*******************************************************************************
 ******************************************************************************/

#endif // _BOARD_H_
