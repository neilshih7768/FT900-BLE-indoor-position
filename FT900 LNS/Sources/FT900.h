#ifndef FT900_H
#define FT900_H


/* The Timer Prescaler will divide the 100MHz Master clock down to 2kHz */
#define TIMER_PRESCALER (50000)
#define TIMER_ONE_MILLISECOND (100000/TIMER_PRESCALER)
#define TIMER_ONE_SECOND (1000*TIMER_ONE_MILLISECOND)


void Setup(void);
void Loop(void);
void Uart1ISR(void);

#endif
