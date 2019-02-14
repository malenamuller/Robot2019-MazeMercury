/***************************************************************************//**
  @file     UART.c
  @brief    UART Driver for K64F. Non-Blocking and using FIFO feature
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "UART.h"

#include "PORT.h"
#include "Template/LDM.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// DRIVER configuration
//#define UART_FIFO_FEATURE_ENA


// UART definitions
#define UART_HAL_DEFAULT_BAUDRATE 115200

#define UART0_USED			1
#define UART3_USED			1


// PORT definitions

#define UART_RX_PORT_CONFIG	(PCRstr) {1, 1, 0, 0, 0, 0, 0, 0, PORT_mAlt3, 0, 0, PORT_eDisabled, 0, 0, 0} // pull-up, Alternative 3, Interrupt/DMA request disabled.
#define UART_TX_PORT_CONFIG	(PCRstr) {0, 0, 0, 0, 0, 0, 0, 0, PORT_mAlt3, 0, 0, PORT_eDisabled, 0, 0, 0} // no-pull, Alternative 3, Interrupt/DMA request disabled.


// QUEUE definitions
#define WIFI_RX_QUEUE_SIZE		64
#define WIFI_TX_QUEUE_SIZE		64

#define TERM_RX_QUEUE_SIZE		128
#define TERM_TX_QUEUE_SIZE		128


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
	uint8_t iPut;
	uint8_t iGet;
	uint8_t count;
	uint8_t size;
	uint8_t *buffer;
} uartQueue_t;


typedef struct {
	UART_Type *base;
	PORT_Type *port;
	char rxPin;
	char txPin;
	uint16_t rxFifoSize;
	uint16_t txFifoSize;
	uint32_t baudrate;
	uartQueue_t *rxQueue;
	uartQueue_t *txQueue;
} uartConfig_t;


/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void UART_SetBaudRate (UART_Type *uart, uint32_t baudrate);

static void UART_FlushRxFifoIntoQueue(UART_Type * base, uartQueue_t * queue);
static void UART_FlushTxQueueIntoFifo(UART_Type * base, uartQueue_t * queue, uint32_t fifoSize);


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static char wifi_rx_buffer[WIFI_RX_QUEUE_SIZE];
static char wifi_tx_buffer[WIFI_TX_QUEUE_SIZE];
static char term_rx_buffer[TERM_RX_QUEUE_SIZE];
static char term_tx_buffer[TERM_TX_QUEUE_SIZE];


static uartQueue_t wifi_rx_queue = {0, 0, 0, NUMEL(wifi_rx_buffer), wifi_rx_buffer};
static uartQueue_t wifi_tx_queue = {0, 0, 0, NUMEL(wifi_rx_buffer), wifi_tx_buffer};
static uartQueue_t term_rx_queue = {0, 0, 0, NUMEL(term_rx_buffer), term_rx_buffer};
static uartQueue_t term_tx_queue = {0, 0, 0, NUMEL(term_rx_buffer), term_tx_buffer};



/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/


static const uartConfig_t uartList[UART_CANT_ID] = {
	{UART3, PORTC, 16, 17, 1, 1,  57600, &wifi_rx_queue, &wifi_tx_queue}, // UART_WIFI
	{UART0, PORTB, 16, 17, 8, 8, 115200, &term_rx_queue, &term_tx_queue}  // UART_TERMINAL
};


#define UART3_CONFIG	(uartList+UART_WIFI)
#define UART0_CONFIG	(uartList+UART_TERMINAL)



/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void UART_Init (void)
{
	static bool yaInit = FALSE;
	if (yaInit)
		return;

	const uartConfig_t *uart;

#if defined(UART0_USED)
	SIM_SCGC4 |= SIM_SCGC4_UART0_MASK; // Enable Clock Gating for UART0
	NVIC_EnableIRQ(UART0_RX_TX_IRQn); // Enable UART0's IRQ in NVIC
#endif // UART0_USED
#if defined(UART1_USED)
	SIM_SCGC4 |= SIM_SCGC4_UART1_MASK; // Enable Clock Gating for UART1
	NVIC_EnableIRQ(UART1_RX_TX_IRQn); // Enable UART0's IRQ in NVIC
#endif // UART1_USED
#if defined(UART2_USED)
	SIM_SCGC4 |= SIM_SCGC4_UART2_MASK; // Enable Clock Gating for UART2
	NVIC_EnableIRQ(UART2_RX_TX_IRQn); // Enable UART0's IRQ in NVIC
#endif // UART2_USED
#if defined(UART3_USED)
	SIM_SCGC4 |= SIM_SCGC4_UART3_MASK; // Enable Clock Gating for UART3
	NVIC_EnableIRQ(UART3_RX_TX_IRQn); // Enable UART0's IRQ in NVIC
#endif // UART3_USED
#if defined(UART4_USED)
	SIM_SCGC1 |= SIM_SCGC1_UART4_MASK; // Enable Clock Gating for UART4
	NVIC_EnableIRQ(UART4_RX_TX_IRQn); // Enable UART0's IRQ in NVIC
#endif // UART4_USED
#if defined(UART5_USED)
	SIM_SCGC1 |= SIM_SCGC1_UART5_MASK; // Enable Clock Gating for UART5
	NVIC_EnableIRQ(UART5_RX_TX_IRQn); // Enable UART0's IRQ in NVIC
#endif // UART5_USED

	for (uart=uartList ; uart<uartList+UART_CANT_ID ; ++uart)
	{
		PORT_Configure2(uart->port, uart->rxPin, UART_RX_PORT_CONFIG); //Setup PORT for TX and RX pins
		PORT_Configure2(uart->port, uart->txPin, UART_TX_PORT_CONFIG);

		UART_SetBaudRate(uart->base, uart->baudrate); // Configure baudrate

		if (uart->rxFifoSize > 1)
		{ // Configure FIFO
			uart->base->C2 = 0; // Disable RX and TX
			uart->base->TWFIFO = 0; /* Set tx/rx FIFO watermark */
			uart->base->RWFIFO = (uart->rxFifoSize)-1;
			uart->base->PFIFO |= UART_PFIFO_TXFE_MASK | UART_PFIFO_RXFE_MASK; /* Enable TX/RX FIFO */
			uart->base->CFIFO = UART_CFIFO_TXFLUSH_MASK | UART_CFIFO_RXFLUSH_MASK; /* Flush FIFO */
		}

		uart->base->C2 = UART_C2_TE_MASK | UART_C2_RE_MASK | UART_C2_RIE_MASK; // Enable RX, TX and RX IRQ
	}
}


// UART RX QUEUED

uint8_t UART_IsRxMsg(uartId_t id)
{
	uint8_t rta = FALSE;
	if (id < UART_CANT_ID)
	{
		const uartConfig_t *uart = uartList + id;
		rta = (uart->rxQueue->count > 0);
		if (!rta && (uart->rxFifoSize > 1))
		{
			rta = (uart->base->RCFIFO != 0);
		}
	}
	return rta;
}


uint8_t UART_GetRxMsgLength(uartId_t id)
{
	uint8_t len = 0;
	if (id < UART_CANT_ID)
	{
		const uartConfig_t *uart = uartList + id;
		len = uart->rxQueue->count;
		if (!len && (uart->rxFifoSize > 1))
		{
			len += uart->base->RCFIFO;
		}
	}
	return len;
}


uint8_t UART_ReadMsg(uartId_t id, char* msg, uint8_t cant)
{
	if (id >= UART_CANT_ID)
		return 0;

	uint8_t i, qCant;
	const uartConfig_t *uart = uartList + id;
	uartQueue_t * queue = uart->rxQueue;

	qCant = queue->count;
	if (cant > qCant)
	{
		if ((uart->rxFifoSize > 1) && (uart->base->RCFIFO > 0))
		{
			__LDM_DisableInterrupts();
			UART_FlushRxFifoIntoQueue(uart->base, queue); // Esto debe ser ATOMIC!!!
			__LDM_EnableInterrupts();

			qCant = queue->count;
			if (cant > qCant)
			{
				cant = qCant;
			}
		}
		else
		{
			cant = qCant;
		}
	}

	if (cant)
	{
		for (i=0 ; i<cant ; ++i)
		{
			msg[i] = queue->buffer[queue->iGet];
			INCMOD(queue->iGet, queue->size);
		}

		__LDM_DisableInterrupts();
		queue->count -= cant; // Esto debe ser ATOMIC!!!
		__LDM_EnableInterrupts();
	}

	return cant;
}


uint8_t UART_WriteMsg(uartId_t id, const char* msg, uint8_t cant)
{
	if (id >= UART_CANT_ID)
		return 0;

	uint8_t i, qFree;
	const uartConfig_t *uart = uartList + id;
	uartQueue_t * queue = uart->txQueue;

	qFree = queue->size - queue->count;

	if (cant > qFree)
	{
		cant = qFree;
	}

	if (cant)
	{
		for (i=0 ; i<cant ; ++i)
		{
			queue->buffer[queue->iPut] = msg[i];
			INCMOD(queue->iPut, queue->size);
		}

		__LDM_DisableInterrupts();
		queue->count += cant; // Esto debe ser ATOMIC!!!
		__LDM_EnableInterrupts();
	}

	uart->base->C2 |= UART_C2_TIE_MASK; // Enable TDRE IRQ

	return cant;
}


uint8_t UART_IsTxMsgComplete(uartId_t id)
{
	uint8_t rta = FALSE;
	if (id < UART_CANT_ID)
	{
		const uartConfig_t *uart = uartList + id;
		rta = (uart->txQueue->count == 0) && (uart->base->S1 & UART_S1_TC_MASK) && (uart->rxFifoSize > 1) && (uart->base->TCFIFO == 0);
	}
	return rta;
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


static void UART_SetBaudRate (UART_Type *uart, uint32_t baudrate)
{
	uint16_t sbr, brfa;
	uint32_t clock;

	clock = ((uart == UART0) || (uart == UART1))?(__CORE_CLOCK__):(__CORE_CLOCK__ >> 1);

	baudrate = (baudrate == 0)? UART_HAL_DEFAULT_BAUDRATE : baudrate;

	sbr = clock / (baudrate << 4);               // sbr = clock/(Baudrate x 16)
	brfa = (clock << 1) / baudrate - (sbr << 5); // brfa = 2*Clock/baudrate - 32*sbr

	uart->BDH = UART_BDH_SBR(sbr >> 8);
	uart->BDL = UART_BDL_SBR(sbr);
	uart->C4 = (uart->C4 & ~UART_C4_BRFA_MASK) | UART_C4_BRFA(brfa);
}


#if defined(UART0_USED)
__ISR__ UART0_RX_TX_IRQHandler (void)
{
	uint8_t s1 = UART0->S1;
	if (s1 & UART_S1_RDRF_MASK) // RX IRQ
	{
		UART_FlushRxFifoIntoQueue(UART0, UART0_CONFIG->rxQueue);
	}
	else // if (s1 & UART_S1_TDRE_MASK) // TX IRQ
	{
		UART_FlushTxQueueIntoFifo(UART0, UART0_CONFIG->txQueue, UART0_CONFIG->txFifoSize);
	}
}
#endif // defined(UART0_USED)


#if defined(UART3_USED)
__ISR__ UART3_RX_TX_IRQHandler (void)
{
	uint8_t s1 = UART3->S1;
	if (s1 & UART_S1_RDRF_MASK) // RX IRQ
	{
		static volatile uint8_t rx_data;
		rx_data = UART3->D;
		uartQueue_t *queue = UART3_CONFIG->rxQueue;

		if (queue->count < queue->size)
		{
			queue->buffer[queue->iPut] = rx_data;
			INCMOD(queue->iPut, queue->size);
			++(queue->count);
		}
	}
	else // if (s1 & UART_S1_TDRE_MASK) // TX IRQ
	{
		uartQueue_t *queue = UART3_CONFIG->txQueue;
		uint8_t dummy_read, tx_data;

		tx_data = queue->buffer[queue->iGet];
		dummy_read = UART3->S1;
		UART3->D = tx_data;
		INCMOD(queue->iGet, queue->size);

		if (!(--(queue->count)))
		{
			UART3->C2 &= ~UART_C2_TIE_MASK; // Disable TDRE IRQ
		}

	}
}
#endif // defined(UART3_USED)


#if 0
static void UARTN_RX_TX_IRQHandler (const uartConfig_t * uart)
{
	if (uart->base->S1 & UART_S1_RDRF_MASK) // RX IRQ
	{
		uartQueue_t *queue = uart->rxQueue;
		if(uart->rxFifoSize > 1)
		{
			UART_FlushRxFifoIntoQueue(uart->base, queue);
		}
		else
		{
			uint8_t dummy_read = uart->base->S1;
			uint8_t rx_data = uart->base->D;

			if (queue->count < queue->size)
			{
				++(queue->count);
				queue->buffer[queue->iPut] = rx_data;
				INCMOD(queue->iPut, queue->size);
			}
		}
	}
	else // if (s1 & UART_S1_TDRE_MASK) // TX IRQ
	{
		uartQueue_t *queue = uart->txQueue;
		if(uart->txFifoSize > 1)
		{
			UART_FlushTxQueueIntoFifo(uart->base, queue, uart->txFifoSize);
		}
		else
		{
			uint8_t tx_data = queue->buffer[queue->iGet];
			uint8_t dummy_read = uart->base->S1;
			uart->base->D = tx_data;
			INCMOD(queue->iGet, queue->size);
			if (!(--(queue->count)))
			{
				uart->base->C2 &= ~UART_C2_TIE_MASK; // Disable TDRE IRQ
			}

		}
	}
}
#endif





static void UART_FlushRxFifoIntoQueue(UART_Type * base, uartQueue_t * queue)
{
	uint8_t dummy_read, rx_data;

	while (base->RCFIFO)
	{
		dummy_read = base->S1;
		rx_data = base->D;
		if (queue->count < queue->size)
		{
			++(queue->count);
			queue->buffer[queue->iPut] = rx_data;
			INCMOD(queue->iPut, queue->size);
		}
	}
}


static void UART_FlushTxQueueIntoFifo(UART_Type * base, uartQueue_t * queue, uint32_t fifoSize)
{
	uint8_t dummy_read, tx_data;

	while ((queue->count > 0) && (base->TCFIFO < fifoSize))
	{
		tx_data = queue->buffer[queue->iGet];
		dummy_read = base->S1;
		base->D = tx_data;
		INCMOD(queue->iGet, queue->size);
		--(queue->count);
	}
	if (!(queue->count))
	{
		base->C2 &= ~UART_C2_TIE_MASK; // Disable TDRE IRQ
	}
}



