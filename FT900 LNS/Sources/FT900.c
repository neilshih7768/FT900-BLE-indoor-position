#include <ft900.h>
#include "FT900.h"
#include "BLE.h"


/* The Timer Prescaler will divide the 100MHz Master clock down to 2kHz */
#define TIMER_PRESCALER (50000)
#define TIMER_ONE_MILLISECOND (100000/TIMER_PRESCALER)
#define TIMER_ONE_SECOND (1000*TIMER_ONE_MILLISECOND)
#define PERIOD 10
#define RECESIZE 10				// Recently size for average


uint8_t sRxData[100];
int iRxIndex = 0;
uint8_t sName[30];
int iRSSI;

int iRxCounterArray[PERIOD];	// For frequency
double dTimer = 0;				// For frequency
int iRecently[RECESIZE];		// Record recently data for average
int iRIndex = 0;				// Record recently data index


char GetCharToRxData()
{
	uint8_t c;
	uart_read(UART1, &c);

	sRxData[iRxIndex] = c;
	iRxIndex++;

	return c;
}


void CheckRxData()
{
	/* Filter wrong data */
	double dDistance = 0.0f;
	char *pCheckName = strstr (sRxData, "{NAME:");	// Find "{NAME:" in RX data
	char *pChecRSSI  = strstr (sRxData, "RSSI:");	// Find "RSSI:" in RX data

	if(NULL == pCheckName || NULL == pChecRSSI)
	{
		return;										// Not right data
	}

	char *pCheckName2 = strstr (sRxData, "FTDI_iSensor");	// Find target device
	if(NULL == pCheckName2)
	{
		return;										// Not right data
	}

	char sPrintData[20];

	const int iHeadShift = strlen("{NAME:") + 1;	// For delete "{NAME:" from header
	int i = iHeadShift;
	if(':' == sRxData[i])							// Strange bug, "{NAME:" length is floating
		i++;

	while(',' != sRxData[i])						// ',' of ",MAC" is parse signal for name and MAC
	{
		sName[i-iHeadShift] = sRxData[i];			// Parse name data to sName
		i++;
	}
	sName[i] = '\0';

	iRSSI = -1 * ((pChecRSSI[6]-'0') * 10 + (pChecRSSI[7]-'0'));	// Count iRSSI for integer
	if('1' == pChecRSSI[6]){
		iRSSI = iRSSI * 10 - (pChecRSSI[8]-'0');					// if RSSI > 100
	}


	// For frequency
	iRxCounterArray[(int)dTimer]++;

	float iTotalCount = 0;
	for(int i = 0; i < PERIOD; i++)
		iTotalCount += iRxCounterArray[i];

	dDistance = ConvertRSSI2toDist((double)iRSSI);


	// For average
	iRecently[iRIndex] = dDistance;
	iRIndex++;
	if(RECESIZE == iRIndex)
		iRIndex = 0;

	float iTotalRecently = 0;
	for(int i = 0; i < RECESIZE; i++)
		iTotalRecently += iRecently[i];


	sprintf(sPrintData, "RSSI = %d, Distance = %5.1fcm  %5.1f Hz, Average = %5.1fcm\r\n"
			, iRSSI, dDistance, iTotalCount/PERIOD, iTotalRecently/RECESIZE);

	uart_puts(UART0, sPrintData);


}


void ClearRxData()
{
	for(int i = 0; i < iRxIndex; i++){
		sRxData[i] = '\0';
	}

	iRxIndex = 0;
}


void PrintName()
{
	for(int i = 0; i < 20; i++){
		uart_write(UART0, sName[i]);
	}

	uart_write(UART1, '\n');
	uart_write(UART1, '\r');
}


void Setup()
{
	SetBLEData();
	GetLNSData(dReference, &dP0, &dN);

	/* UART part */
	/* Enable the UART0 Device... */
	sys_enable(sys_device_uart0);

	/* Make GPIO48 function as UART0_TXD and GPIO49 function as UART0_RXD... */
	gpio_function(48, pad_uart0_txd); /* UART1 TXD */
	gpio_function(49, pad_uart0_rxd); /* UART1 RXD */
	uart_open(UART0,                    /* Device */
	          1,                        /* Prescaler = 1 */
	          UART_DIVIDER_115200_BAUD, /* Divider = 1302 */
	          uart_data_bits_8,         /* No. Data Bits */
	          uart_parity_none,         /* Parity */
	          uart_stop_bits_1);        /* No. Stop Bits */


    /* Enable the UART1 Device... */
    sys_enable(sys_device_uart1);

    /* Make GPIO52 function as UART1_TXD and GPIO53 function as UART1_RXD... */
    gpio_function(52, pad_uart1_txd); /* UART1 TXD */
    gpio_function(53, pad_uart1_rxd); /* UART1 RXD */
    uart_open(UART1,                    /* Device */
              1,                        /* Prescaler = 1 */
              UART_DIVIDER_115200_BAUD, /* Divider = 1302 */
              uart_data_bits_8,         /* No. Data Bits */
              uart_parity_none,         /* Parity */
              uart_stop_bits_1);        /* No. Stop Bits */


    /* Enable the UART to fire interrupts when receiving data... */
    uart_enable_interrupt(UART1, uart_interrupt_rx);

    /* Attach the interrupt so it can be called... */
    interrupt_attach(interrupt_uart1, (uint8_t) interrupt_uart1, Uart1ISR);

    /* Enable interrupts to be fired... */
    uart_enable_interrupts_globally(UART1);
    interrupt_enable_globally();


    /* Timer part */
    /* Enable Timers... */
	sys_enable(sys_device_timer_wdt);

	/* Set up the Timer tick to be 0.5 ms... */
	timer_prescaler(TIMER_PRESCALER);

	/* Set up Timer A to be triggered after 5 seconds... */
	timer_init(timer_select_a,              /* Device */
			   TIMER_ONE_SECOND,	        /* Initial Value */
			   timer_direction_down,        /* Count Direction */
			   timer_prescaler_select_on,   /* Prescaler Select */
			   timer_mode_continuous);      /* Timer Mode */

	/* Enable the timers... */
	timer_enable_interrupt(timer_select_a);

	/* Start all the timers at the same time... */
	timer_start(timer_select_a);
}


void Loop()
{

	if (timer_is_interrupted(timer_select_a) == 1)	// Per second do once
	{
		dTimer++;
		if(PERIOD == dTimer)
			dTimer = 0;

		iRxCounterArray[(int)dTimer] = 0;			// Reset counter


	}

}

/**
 The Interrupt that allows echoing back over UART1
 */
void Uart1ISR()
{
    if (uart_is_interrupted(UART1, uart_interrupt_rx))
    {


    	uint8_t c = 0;
    	//uart_read(UART1, &c);

    	c = GetCharToRxData();
    	//uart_write(UART0, c);

    	if('}' == c)
    	{
    		CheckRxData();
    		//PrintName();
    		ClearRxData();
    	}
    }
}
