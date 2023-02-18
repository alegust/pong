#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */

#define TMR2PERIOD ((80000000 / 256) / 10 / 4)
#if TMR2PERIOD > 0xffff
#error "Timer period is too big."
#endif

void timer_init() {
	T2CON = 0x70; // set prescale, i.e how many cycles that is needed to increment the time clock
  PR2 = TMR2PERIOD; // Set the peroid, i.e how many cycles divided by prescale
  TMR2 = 0; // Rest the timer
  T2CONSET = 0x8000;  // Start the timer

  TRISE = TRISE & 0xff00;
  TRISD = TRISD | (0x7f << 5); 
}

int timeoutcount = 0;

int timertest(void) {
  if (IFS(0) & 0x100) { /* Test time-out event flag */
   timeoutcount++;
   IFS(0) = 0; /* Reset all events  */
   return(1);
  } 
  else {
   return(0);
  }
}


int main(void) {
  /*
	  This will set the peripheral bus clock to the same frequency
	  as the sysclock. That means 80 MHz, when the microcontroller
	  is running at 80 MHz. Changed 2017, as recommended by Axel.
	*/
	SYSKEY = 0xAA996655;  /* Unlock OSCCON, step 1 */
	SYSKEY = 0x556699AA;  /* Unlock OSCCON, step 2 */
	while(OSCCON & (1 << 21)); /* Wait until PBDIV ready */
	OSCCONCLR = 0x180000; /* clear PBDIV bit <0,1> */
	while(OSCCON & (1 << 21));  /* Wait until PBDIV ready */
	SYSKEY = 0x0;  /* Lock OSCCON */
	
	/* Set up output pins */
	AD1PCFG = 0xFFFF;
	ODCE = 0x0;
	TRISECLR = 0xFF;
	PORTE = 0x0;
	
	/* Output pins for display signals */
	PORTF = 0xFFFF;
	PORTG = (1 << 9);
	ODCF = 0x0;
	ODCG = 0x0;
	TRISFCLR = 0x70;
	TRISGCLR = 0x200;
	
	/* Set up input pins */
	TRISDSET = (1 << 8);
	TRISFSET = (1 << 1);
	
	/* Set up SPI as master */
	SPI2CON = 0;
	SPI2BRG = 4;
	/* SPI2STAT bit SPIROV = 0; */
	SPI2STATCLR = 0x40;
	/* SPI2CON bit CKP = 1; */
  SPI2CONSET = 0x40;
	/* SPI2CON bit MSTEN = 1; */
	SPI2CONSET = 0x20;
	/* SPI2CON bit ON = 1; */
	SPI2CONSET = 0x8000;
	
	display_init();
	timer_init();
	
  game_init();

	while( 1 )
	{
		if (timertest()) {
			game_tick();
		}
	}
	return 0;
}

/* Non-Maskable Interrupt; something bad likely happened, so hang */
void _nmi_handler() {
	for(;;);
}

/* This function is called upon reset, before .data and .bss is set up */
void _on_reset() {
	
}

/* This function is called before main() is called, you can do setup here */
void _on_bootstrap() {
	
}
