/*
 *	testlowlevel.c - test for Mignonette V.2 hardware (PROTOTYPE ONLY!) that avoids ISR - junky code
 *
 *	author(s): rolf van widenfelt (rolfvw at pizzicato dot com) (c) 2009 - Some Rights Reserved
 *
 *	author(s): mitch altman (c) 2008 - Some Rights Reserved
 *
 *	Note: This source code is licensed under a Creative Commons License, CC-by-nc-sa.
 *		(attribution, non-commercial, share-alike)
 *  	see http://creativecommons.org/licenses/by-nc-sa/3.0/ for details.
 *
 *
 *	hardware requirements:
 *		- Mignonette v2.0 PROTOTYPE!!
 *
 *
 *	instructions:
 *
 *
 *	revision history:
 *
 *	- dec 30, 2009 - rolf
 *		created.  (note that the ISR is disabled by not starting timers)
 *
 *
 */

#include <inttypes.h>
#include <avr/io.h>			/* this takes care of definitions for our specific AVR */
#include <avr/pgmspace.h>	/* needed for printf_P, etc */
#include <avr/interrupt.h>	/* for interrupts, ISR macro, etc. */
#include <stdio.h>			// for sprintf, etc.
#include <stdlib.h>			// for abs(), etc.
//#include <string.h>			// for strcpy, etc.

#include "uart.h"			// we keep this here only to define F_CPU (uart.c not needed)

// for _delay_us() macro  (note: this gets F_CPU define from uart.h)
#include <util/delay.h>

#include "mydefs.h"
#include "iodefs.h"

#include "miggl.h"		/* Mignonette Game Library */


void do_testbuttons_low(void);



//
// crude delay of 1 to 255 us
//
void
delay_us(byte usec)
{
	usec++;
	
	while (--usec) {
		_delay_us(1);		// get 1us delay from library macro (see <util/delay.h>)
	}
}


//
// crude delay of 1 to 255 ms
//
void
delay_ms(uint8_t ms)
{
	
	ms++;
	
	while (--ms) {
		_delay_ms(1);		// get 1ms delay from library macro (see <util/delay.h>)
	}
}


//
// crude "sleep" function for 0 to 255 seconds
//
void
delay_sec(uint8_t sec)
{
	uint8_t i;
	for (i = 0; i < sec; i++) {
		delay_ms(250);
		delay_ms(250);
		delay_ms(250);
		delay_ms(250);
	}
}


#define XMAX (XSCREEN-1)
#define YMAX (YSCREEN-1)




//
// once the "Munch" program is chosen, we keep it running until reset/power off
//
int
main(void)
{

    avrinit();

	//initswapbuffers();
	//swapinterval(10);		// note: display refresh is 100hz (lower number speeds up game)
	//cleardisplay();

	//start_timer1();			// this starts display refresh and audio processing
	
	//button_init();

	//initaudio();			// XXX eventually, we remove this!
	
	do_testbuttons_low();
	
	return 0;	// silence compiler warning
}


//
// this is a simple low level test of the Mig V.2 button logic
//
//	note: this function never returns!
//
void do_testbuttons_low(void)
{
	//int i;

	// get started...
	output_low(SPKR);
	delay_sec(1);


	// set ROW1-7 low (to avoid lighting any pixels accidentally when touching GC1-4)
	PORTD = 0x0;

	// set pins to input (DDRx = 0) and disable pullups on those pins
	_output_low(DDRC,PC1);
	_output_low(PORTC,PC1);
	_output_low(DDRC,PC2);
	_output_low(PORTC,PC2);
	_output_low(DDRC,PC3);
	_output_low(PORTC,PC3);
	_output_low(DDRC,PC4);
	_output_low(PORTC,PC4);
	
	// force diode common line high
	output_high(SWCOM);
	

	while (1) {					// loop forever
#ifdef NOTDEF
		if (input_test(GC1)==0) {		// switch pin is "active high"
			output_low(SPKR);
		} else {
			output_high(SPKR);			// use SPKR pin to indicate switch is pressed
		}
#endif
		if ((input_test(GC1)!=0) || (input_test(GC2)!=0)) {		// switch pin is "active high"
			output_high(SPKR);
		} else {
			output_low(SPKR);
		}
		
	}
}
