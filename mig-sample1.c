/*
 *	mig-sample1.c - simple demos using Mignonette Graphics Library (miggl)
 *
 *	author: rolf van widenfelt (rolfvw at pizzicato dot com) (c) 2008 - Some Rights Reserved
 *
 *	Note: This source code is licensed under a Creative Commons License, CC-by-nc-sa.
 *		(attribution, non-commercial, share-alike)
 *  	see http://creativecommons.org/licenses/by-nc-sa/3.0/ for details.
 *
 *
 *	hardware requirements:
 *		- Mignonette v1.0
 *
 *
 *	instructions:
 *		xxx
 *
 *	revision history:
 *
 *	- apr 19, 2009 - rolf
 *		separate out "chooser" function.  (might be useful for other demos!)
 *
 *	- apr 18, 2009 - rolf
 *		create "do_simple7" demo... an interactive chooser.
 *
 *	- apr 13, 2009 - rolf
 *		fine tune "do_simple5" demo.
 *
 *	- apr 12, 2009 - rolf
 *		add a few more demos.
 *
 *	- apr 7, 2009 - rolf
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
uint8_t chooser(uint8_t nchoices, uint8_t ndefault);


/* audio stuff - beware, these song tables take up much memory! (so comment out unused tables) */

byte ChirpSong[] = {
N_C5,N_16TH,
N_END,
};


byte IntroScaleSong[] = {
N_C4,N_16TH,
N_D4,N_16TH,
N_E4,N_16TH,
N_F4,N_16TH,
N_G4,N_8TH,
N_END,
};


byte MunchedSong[] = {
N_C4,N_16TH,
N_GS3,N_16TH,
N_FS3,N_16TH,
N_DS3,N_16TH,
N_END,
};


byte EvilEntrySong[] = {
N_C4,N_16TH,
N_CS4,N_16TH,
N_C4,N_16TH,
N_CS4,N_16TH,
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


int do_simple1(void);
int do_simple2(void);
int do_simple3(void);
int do_simple3b(void);
int do_simple4(void);
int do_simple5(void);
int do_simple6(void);
int do_simple7(void);
int do_simple8(void);


static uint16_t __sr = 0x5995;		// starting seed (note: use anything but zero here!)

//
//	random16 - generates a 16-bit pseudo random number each time it is called.
//
//	SPECIAL NOTE: the value 0 is never generated!!
//
//	this uses a linear feedback shift register (LFSR) to generate a non-repeating sequence
//	that sort of appears random (pseudo-random).
//	more info at:
//		http://en.wikipedia.org/wiki/Linear_feedback_shift_register
//
uint16_t random16(void) {
	uint16_t sr, bit;

	sr = __sr;

	//
	// here is the LFSR algorithm with a 16-bit SR, with taps at bits 0, 2, 3, and 5
	//
	bit = ((sr) ^
			(sr >> 2) ^
			(sr >> 3) ^
			(sr >> 5) ) & 0x0001;	// no need to mask because XOR is bitwise

	sr = sr >> 1;

	if (bit) {
		sr = sr | 0x8000;
	}
	
	__sr = sr;

	return sr;
}



//
// once the "Munch" program is chosen, we keep it running until reset/power off
//
int
main(void)
{
	uint8_t n;

    avrinit();

	initswapbuffers();
	swapinterval(10);		// set display refresh to 10hz (10/100.. lower number speeds up game)
	cleardisplay();

	start_timer1();			// this starts display refresh and audio processing
	
	button_init();

	initaudio();			// XXX eventually, we remove this!
	
	while (1) {
		
		n = chooser(7, 0);
		if (n == 1) {
			do_simple8();		// Lukas test game dev
		} else if (n == 2) {
			do_simple2();		// each button plays a "song"
		} else if (n == 3) {
			do_simple4();		// using readpixel function to determine a collision
		} else if (n == 4) {
			do_simple3b();		// use random number generator to fill the screen with dots
		} else if (n == 5) {
			do_simple5();		// simple animation of an "M"
		} else if (n == 6) {
			do_simple6();		// simple animation of some line patterns
		} else {	// (n == 4)
			do_simple1();		// draws a point (pixel) in the corner near the button you press
		}

	}
}


//
// this is a set of simple actions using the Mignonette Game Library
//

//
// do_simple1 - this draws a point (pixel) in the corner near the button you press.
// 
//	note: you can experiment with swapinterval() values to see how
//		it affects the response time to the buttons.
//		for example, swapinterval(5) is very responsive, but a value of 20 will seem sluggish.
//
int do_simple1(void)
{
	int px, py;


	swapinterval(5);		// note: display refresh is 100hz (lower number speeds up game)
	cleardisplay();


	while (1) {					// loop forever
	
		cleardisplay();
		
		handlebuttons();
	
		if (ButtonA) {
			px = 0;
			py = 0;
		} else if (ButtonB) {
			px = 0;
			py = 4;
		} else if (ButtonC) {
			px = 6;
			py = 4;
		} else if (ButtonD) {
			px = 6;
			py = 0;
		} else {
			px = 3;
			py = 2;
		}
		
		setcolor(GREEN);
		drawpoint(px, py);

		swapbuffers();			// wait for next display cycle...
	}
	
	return 0; // note: never reaches this!
}


//
// do_simple2 - this plays a different "song" for each button you press.
//
//	this is an example of using buttons to trigger events, in this case
//	the playing of a song.
// 
//
int do_simple2(void)
{
	//int i;
	int px, py;


	swapinterval(5);		// note: display refresh is 100hz (lower number speeds up game)
	cleardisplay();


	while (1) {					// loop forever
	
		cleardisplay();
		
		handlebuttons();
	
		if (ButtonA && ButtonAEvent) {			// detect "A button pressed" event
			playsong(ChirpSong);				// start song
			ButtonAEvent = 0;					// clear this event
		} else if (ButtonB && ButtonBEvent) {
			playsong(IntroScaleSong);
			ButtonBEvent = 0;
		} else if (ButtonC && ButtonCEvent) {
			playsong(MunchedSong);
			ButtonCEvent = 0;
		} else if (ButtonD && ButtonDEvent) {
			playsong(EvilEntrySong);
			ButtonDEvent = 0;
		} else {
			;
		}
		
		if (isaudioplaying()) {
			setcolor(RED);
			py = 2;
			for (px = 2; px <= 4; px++) {
				drawpoint(px, py);
			}
		} else {
			setcolor(RED);
			px = 3; py = 2;
			drawpoint(px, py);
		}

		swapbuffers();			// wait for next display cycle...
	}
	
	return 0; // note: never reaches this!
}


//
// do_simple3 - use random number generator to fill the screen with dots 
// 
//
int do_simple3(void)
{
	int px, py;
	uint16_t rnd;


	swapinterval(5);		// note: display refresh is 100hz (lower number speeds up game)
	cleardisplay();

	setcolor(RED);

	while (1) {					// loop forever
		
		handlebuttons();
	
		if (ButtonAEvent) {
			cleardisplay();
			ButtonAEvent = 0;
		}
		
		rnd = random16();
		
		px = rnd & 0x7;

		py = (rnd >> 3) & 0x7;

		drawpoint(px, py);		// note: points outside of the screen are automatically "clipped".

		swapbuffers();			// wait for next display cycle...
	}
	
	return 0; // note: never reaches this!
}


//
// do_simple4 - using readpixel function to determine a collision
// 
//
int do_simple4(void)
{
	int bx, by;
	int px, py;
	uint16_t rnd;
	int pixelcount = 18;
	uint8_t value;


	swapinterval(5);		// note: display refresh is 100hz (lower number speeds up game)
	cleardisplay();

	bx = 0;
	by = 2;
	ButtonAEvent = 1;		// hack to start with random pixels

	while (1) {					// loop forever
		
		handlebuttons();
	
		if (ButtonAEvent) {
			cleardisplay();
			pixelcount = 18;
			ButtonAEvent = 0;
		}

		if (ButtonB) {
			bx--;
			if (bx < 0) {
				bx = 0;
			}
		} else if (ButtonC) {
			bx++;
			if (bx > XMAX) {
				bx = XMAX;
			}
		}

		if (pixelcount > 0) {
			//
			//	first, draw a bunch of random pixels (red)
			//
			rnd = random16();
			px = rnd & 0x7;
			py = (rnd >> 3) & 0x7;
			pixelcount--;

			setcolor(RED);
			drawpoint(px, py);		// note: points outside of the screen are automatically "clipped".

		} else {
			//
			//	then, move a dot across the screen and draw yellow wherever there is a collision
			//
			value = readpixel(bx, by);
			if (value == RED) {
				setcolor(YELLOW);
				drawpoint(bx, by);
				playsong(ChirpSong);
			} else if (value == BLACK) {
				setcolor(GREEN);
				drawpoint(bx, by);
			}
			
		}

		swapbuffers();			// wait for next display cycle...
	}
	
	return 0; // note: never reaches this!
}


//
// do_simple3b - use random number generator to fill the screen with dots
//
//	this variation plays a short sound each time a new dot is drawn.
//
int do_simple3b(void)
{
	int px, py;
	uint16_t rnd;


	swapinterval(4);		// note: display refresh is 100hz (lower number speeds up game)
	cleardisplay();

	setcolor(RED);

	while (1) {					// loop forever
		
		handlebuttons();
	
		if (ButtonAEvent) {
			cleardisplay();
			ButtonAEvent = 0;
		}
		
		rnd = random16();
		
		px = rnd & 0x7;

		py = (rnd >> 3) & 0x7;
		
		if ((px < XSCREEN) && (py < YSCREEN)) {
			if (readpixel(px, py) == BLACK) {
				playsong(ChirpSong);
			}
			drawpoint(px, py);
		}

		swapbuffers();			// wait for next display cycle...
	}
	
	return 0; // note: never reaches this!
}


//
// do_simple5 - simple animation of an "M"
// 
//
int do_simple5(void)
{
	int bx, by;
	int px, py;


#define CHAR_M_PTS 13
	static uint8_t charM[CHAR_M_PTS*2] =	{1,4, 1,3, 1,2, 1,1, 1,0, 2,1, 3,2, 4,1,
											 5,0, 5,1, 5,2, 5,3, 5,4 };

	uint8_t initedflag = 0;
	uint8_t clearedflag = 0;
	uint8_t delayframes = 9;
	uint8_t count = 0;
	int nframes = 0;

	swapinterval(5);		// note: display refresh is 100hz (lower number speeds up game)
	cleardisplay();

	bx = 0;
	by = 2;
	
	
	while (1) {					// loop forever
		
		handlebuttons();
	
		if (ButtonAEvent) {		// "catch" button A event
			cleardisplay();
			initedflag = 0;
			clearedflag = 0;
			delayframes = 9;
			count = 0;
			nframes = 0;
			ButtonAEvent = 0;	// clear event

		}

		// step 2a - delay frames (short pause)
		// step 2b - draw again in black (clear it)

		if (initedflag && !clearedflag) {
			if (delayframes > 0) {
				delayframes--;
			} else {
				px = charM[count*2];
				py = charM[count*2+1];
				count++;
				
				if (count >= CHAR_M_PTS) {
					clearedflag = 1;
				}
				
				setcolor(BLACK);
				drawpoint(px, py);
			}
		}

		// step 1 - draw in green

		if (!initedflag && !ButtonA) {
			px = charM[count*2];
			py = charM[count*2+1];
			count++;
			
			if (count >= CHAR_M_PTS) {
				initedflag = 1;
				count = 0;
			}
			
			setcolor(GREEN);
			drawpoint(px, py);
		}
		
		if (nframes >= 400) {	// after a long delay with no activity (i.e. 20 sec)
			ButtonAEvent = 1;	// force "restart" of animation (fakes a button press event)
		}
		nframes++;

		swapbuffers();			// wait for next display cycle...
	}
	
	return 0; // note: never reaches this!
}



void
drawvert(uint8_t x)
{
	uint8_t y;
	for (y = 0; y < YSCREEN; y++) {
		drawpoint(x, y);
	}
}

void
drawhoriz(uint8_t y)
{
	uint8_t x;
	for (x = 0; x < XSCREEN; x++) {
		drawpoint(x, y);
	}
}

// draw a 5 pixel high ">" char
void
drawangle(uint8_t x)
{
	//static uint8_t dx[YSCREEN] = {0,1,2,1,0};
	static uint8_t dx[YSCREEN] = {0xfe,0xff,0,0xff,0xfe};
	uint8_t y;
	for (y = 0; y < YSCREEN; y++) {
		drawpoint(x+dx[y], y);
	}
}



//
// do_simple6 - simple animation of some line patterns
//
//	buttons B and C descrease and increase animation speed, respectively.
// 
//
int do_simple6(void)
{
	int bx, by;
	int px;

	uint8_t delayframes = 0;
	uint8_t startx, starty;

	uint8_t pattern = 0;

	uint8_t interval = 5;

	swapinterval(interval);		// note: display refresh is 100hz (lower number speeds up game)
	cleardisplay();

	bx = 0;
	by = 2;
	
	startx = starty = 0;
	
	
	while (1) {					// loop forever
		
		cleardisplay();

		handlebuttons();
	
		if (ButtonAEvent && ButtonA) {			// "catch" button A event
			cleardisplay();
			delayframes = 0;
			startx = 0;
			starty = 0;
			pattern++;
			if (pattern > 1) {	// only 2 patterns for now...
				pattern = 0;
			}
			ButtonAEvent = 0;	// clear event

		}

		if (ButtonBEvent && ButtonB) {			// button B decreases speed
			interval--;
			if (interval < 1)
				interval = 1;
			swapinterval(interval);
			ButtonBEvent = 0;	// clear event

		} else if (ButtonCEvent && ButtonC) {	// button C increases speed
			interval++;
			if (interval > 20)
				interval = 20;
			swapinterval(interval);
			ButtonCEvent = 0;	// clear event
		}


		if (pattern == 0) {
			setcolor(RED);
			for (px = startx; px < XSCREEN; px += 3) {
				drawvert(px);
			}
	
			if (delayframes == 0) {
				startx++;				// startx counts 0, 1, 2, 0, 1, 2 ...
				if (startx >= 3) {
					startx = 0;
				}
				delayframes = 3;		// delay between vertical "shifts"
	
			} else {
				delayframes--;
			}
		}

		if (pattern == 1) {
			setcolor(RED);
			for (px = startx; px < XSCREEN+2; px += 3) {
				drawangle(px);
			}
	
			if (delayframes == 0) {
				startx++;				// startx counts 0, 1, 2, 0, 1, 2 ...
				if (startx >= 3) {
					startx = 0;
				}
				delayframes = 2;		// delay between vertical "shifts"
	
			} else {
				delayframes--;
			}
		}


		swapbuffers();			// wait for next display cycle...
	}
	
	return 0; // note: never reaches this!
}


//
// do_simple7 - interactive "chooser" - use buttons to choose between 3 options
//
int do_simple7(void)
{
	uint8_t n;
	uint8_t pick;

	while (1) {
		for (n = 2; n <= 4; n++) {
			pick = chooser(n, 0);		// use buttons to choose between 2, 3 or 4 choices
		}
	}
}

//
// do_simple8 - interactive dot on top row, move along top with B and C buttons
//
int do_simple8(void)
{
	uint8_t n;
	uint8_t px, py;
	uint8_t nselect = 0;
	uint8_t leftx;
	uint8_t dx = 1;
	uint8_t selectflag = 0;
	uint8_t blinkframes = 1;
	uint8_t nchoices = 0;
	
	nchoices = XSCREEN; // set to screen width
	
	// start moving from far left
	leftx = 0;

	swapinterval(5);		// XXX this should be restored upon exit!
	cleardisplay();

	while (1) {
		cleardisplay();
		
		handlebuttons();
		if (ButtonB && ButtonBEvent) {			// button B moves selection left
			if (nselect > 0) {
				nselect--;
				playsong(ChirpSong);
			}
			selectflag = 0;
			ButtonBEvent = 0;
		} else if (ButtonC && ButtonCEvent) {	// button C moves selection right
			if (nselect < (nchoices-1)) {
				nselect++;
				playsong(ChirpSong);
			}
			selectflag = 0;
			ButtonCEvent = 0;
		} else if (ButtonD && ButtonDEvent) {	// button D selects it!
			selectflag = 1;
			blinkframes = 8;
			playsong(EvilEntrySong);
			ButtonDEvent = 0;
		}
		
		py = 0; // dropping happens from the top
		if (selectflag) {
			if (blinkframes % 2 == 0) {
				setcolor(YELLOW); // flash the selected pixel
			} else {
				setcolor(GREEN);
			}
			blinkframes--;
			if (blinkframes == 0) {
				selectflag = 0;
				break;				// we're done...
			}
		} else {
			setcolor(GREEN);
		}

		px = leftx + nselect;
		drawpoint(px, py); // draws moving green dot along the top

		swapbuffers();			// wait for next display cycle
		
	}
	
	swapbuffers();				// adds a slight delay

}


//
// chooser - this presents a simple UI to choose between a small number (2-4) of choices
//
//	note:  all the choices "look" the same
//
//	note:  XXX default choice field (ndefault) is not used!  (future expansion)
//
uint8_t chooser(uint8_t nchoices, uint8_t ndefault)
{
	uint8_t n;
	uint8_t px, py;
	uint8_t nselect = 0;
	uint8_t leftx;
	uint8_t dx = 1;
	uint8_t selectflag = 0;
	uint8_t blinkframes = 1;
	
	if (nchoices > XSCREEN) {		// error checking 
		nchoices = XSCREEN;
	}
	
	// start choices on far left -- taking out centering for more choices
	leftx = 0;


	swapinterval(5);		// XXX this should be restored upon exit!
	cleardisplay();


	while (1) {					// loop forever
	
		cleardisplay();
		
		handlebuttons();
// XXX hack until we fix button events
//ButtonBEvent = ButtonCEvent = ButtonDEvent = 1;
	
		if (ButtonB && ButtonBEvent) {			// button B moves selection left
			if (nselect > 0) {
				nselect--;
				playsong(ChirpSong);
			}
			selectflag = 0;
			ButtonBEvent = 0;
		} else if (ButtonC && ButtonCEvent) {	// button C moves selection right
			if (nselect < (nchoices-1)) {
				nselect++;
				playsong(ChirpSong);
			}
			selectflag = 0;
			ButtonCEvent = 0;
		} else if (ButtonD && ButtonDEvent) {	// button D selects it!
			selectflag = 1;
			blinkframes = 8;
			playsong(EvilEntrySong);
			ButtonDEvent = 0;
		}
		
		py = 2;
		for (n = 0, px = leftx; n < nchoices; n++, px += dx) {
			if (selectflag && (n == nselect)) {
				setcolor(YELLOW);
			} else {
				setcolor(RED);
			}
			drawpoint(px, py);
		}

		if (selectflag) {
			if (blinkframes % 2 == 0) {
				setcolor(BLACK);
			} else {
				setcolor(GREEN);
			}
			blinkframes--;
			if (blinkframes == 0) {
				selectflag = 0;
				break;				// we're done...
			}
		} else {
			setcolor(GREEN);
		}

		px = leftx + nselect*dx;
		drawpoint(px, py-1);
		drawpoint(px, py+1);

		swapbuffers();			// wait for next display cycle
		
	}
	
	swapbuffers();				// adds a slight delay

	return nselect + 1;
}

