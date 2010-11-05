/*
 *	testbuttonsnew.c - test for Mignonette V.2 hardware (PROTOTYPE ONLY!)
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
 *	- dec 27, 2009 - rolf
 *		stripped down version of our munch+test program, just kept the do_testbuttons() part.
 *		created.
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


void do_testbuttons(void);


/* audio stuff - beware, these song tables take up much memory! (so comment out unused tables) */

// needed for testbuttons() function
byte TestSong[] = {
N_C4,N_WHOLE,
N_END,
};

// needed for testbuttons() function
byte ChirpSong[] = {
N_C5,N_8TH,
N_END,
};


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

	initswapbuffers();
	swapinterval(10);		// note: display refresh is 100hz (lower number speeds up game)
	cleardisplay();

	start_timer1();			// this starts display refresh and audio processing
	
	button_init();

	initaudio();			// XXX eventually, we remove this!
	
	while (1) {

		handlebuttons();
		do_testbuttons();

	}
}


//
// this is a simple test of the completed Mignonette hardware
//
//	note: this function never returns!
//
void do_testbuttons(void)
{
	int i;
	int bx, by;
	int dx, dy;
	byte playonce;


	swapinterval(20);		// note: display refresh is 100hz (lower number speeds up game)
	cleardisplay();

	playsong(TestSong);		// make a tone to test speaker


	// now, let's draw something!
	
	//
	// we draw a pixel that is "pulled" in the direction
	// of the pressed button(s).
	// pressing no buttons tends to center the shape.
	// the coord range is (0 0) to (6 4)
	//
	

	// first, test that drawpoint hits every location!
	drawpoint(0, 0);
	swapbuffers();

	for (i = 0; i < 2; i++) {
		uint8_t x, y;
	
		for (y = 0; y <= YMAX; y++) {
	
			for (x = 0; x <= XMAX; x++) {
			
				cleardisplay();
				drawpoint(x, y);
				swapbuffers();
			}
	
		}
break;
		swapinterval(15);		// repeat with slightly higher rate (affect audio?)
		setcolor(GREEN);
	}

	playsong(TestSong);


	// now use a faster refresh to improve response to buttons
	swapinterval(10);
	delay_sec(1);



	bx = 3;
	by = 2;
	playonce = 1;

	while (1) {					// loop forever
	
		cleardisplay();
		
		handlebuttons();
	
		if (ButtonA) {
			dx = -1;
			dy = -1;
		} else if (ButtonB) {
			dx = -1;
			dy = 1;
		} else if (ButtonC) {
			dx = 1;
			dy = 1;
		} else if (ButtonD) {
			dx = 1;
			dy = -1;
		} else {
			dx = 0;
			dy = 0;
		}
		
		bx += dx;
		if (bx < 0) bx = 0;
		else if (bx > XMAX) bx = XMAX;
		
		by += dy;
		if (by < 0) by = 0;
		else if (by > YMAX) by = YMAX;

		if (bx == 0) {
			setcolor(RED);
		} else if  (bx == XMAX) {
			setcolor(GREEN);
		} else if ((bx == 3) && (by == 2)) {	// screen center sets pixel to yellow
			setcolor(YELLOW);
			if (playonce) {
				playsong(ChirpSong);			// play a quick note (chirp) in center position
				playonce = 0;
			}
		}
		
		if ((bx != 3) || (by != 2)) {			// once we move away from center, we can "play" again
			playonce = 1;
		}

		drawpoint(bx, by);

		swapbuffers();			// wait for next display cycle...
	}
}

