/*
 *	mydefs.h - project-specific types, etc - to be included in every file
 *
 * revision history:
 *
 *	jan 14, 2010 - rolf
 *		remove button_pressed() macro, since it is not used for Mig V.2
 *
 *	jan 10, 2007 - rolf
 *		restore to 4/24/2006 version.
 *
 *	4/24/2006 - rolf
 *		add button functions.
 *
 *	3/22/2006 - rolf
 *		created.
 *
 */

#define NOP()	__asm__ volatile("nop"::)

typedef unsigned char boolean;		// wasteful?
typedef unsigned char byte;
typedef uint16_t uint16;
typedef uint32_t uint32;

extern void delay_ms(uint8_t ms);
extern void delay_us(byte usec);
