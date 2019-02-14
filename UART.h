/***************************************************************************//**
  @file     UART.c
  @brief    UART Driver for K64F. Non-Blocking and using FIFO feature
  @author   Nicolás Magliola
 ******************************************************************************/

#ifndef _UART_H_
#define _UART_H_

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
	UART_WIFI,
	UART_TERMINAL,

	UART_CANT_ID
} uartId_t;


/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize UART driver
*/
void UART_Init (void);

/**
 * @brief Check if a new byte was received
 * @return A new byte has being received
*/
uint8_t UART_IsRxMsg(uartId_t id);

/**
 * @brief Check how many bytes were received
 * @return Quantity of received bytes
*/
uint8_t UART_GetRxMsgLength(uartId_t id);

/**
 * @brief Read a received message. Non-Blocking
 * @param msg Buffer to paste the received bytes
 * @param cant Desired quantity of bytes to be pasted
 * @return Real quantity of pasted bytes
*/
uint8_t UART_ReadMsg(uartId_t id, char* msg, uint8_t cant);

/**
 * @brief Write a message to be transmitted. Non-Blocking
 * @param msg Buffer with the bytes to be transfered
 * @param cant Desired quantity of bytes to be transfered
 * @return Real quantity of bytes to be transfered
*/
uint8_t UART_WriteMsg(uartId_t id, const char* msg, uint8_t cant);

/**
 * @brief Check if all bytes were transfered
 * @return All bytes were transfered
*/
uint8_t UART_IsTxMsgComplete(uartId_t id);


/*******************************************************************************
 ******************************************************************************/

#endif // _UART_H_
