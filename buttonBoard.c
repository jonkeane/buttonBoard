/* Keyboard example for Teensy USB Development Board
 * http://www.pjrc.com/teensy/usb_keyboard.html
 * Copyright (c) 2008 PJRC.COM, LLC
 * Modified and extended by Jonathan Keane 2013
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usb_keyboard.h"

#define LED_CONFIG	(DDRD |= (1<<6))
#define LED_ON		(PORTD &= ~(1<<6))
#define LED_OFF		(PORTD |= (1<<6))
#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))
#define length(x) (sizeof x / sizeof *x)


// responses here, in the same order as the pins, for easier refrence:
// {left-ring, left-tip, right-tip, right-ring}
uint8_t buttons[]=
{KEY_A,KEY_SPACE,KEY_ENTER,KEY_S};

// these are the pins that will be checked for button connections on B,
// and will be lit up on D for notification of active ports. 
uint8_t pns[]=
{(1<<2),(1<<3),(1<<6),(1<<7)};

// initialize an array for the pns/ports that are available.
int pnsActive[sizeof(pns)] = {};

int main(void)
{
	uint8_t b, d, mask, i, reset_idle;
	uint8_t b_prev=0xFF;

	// set for 16 MHz clock
	CPU_PRESCALE(0);

	// Configure all port B and port D pins as inputs with pullup resistors.
	// See the "Using I/O Pins" page for details.
	// http://www.pjrc.com/teensy/pins.html
	DDRB = 0x00;
	PORTB = 0xFF;
	DDRD = 0xFF;
	PORTD = 0x00;

	// Initialize the USB, and then wait for the host to set configuration.
	// If the Teensy is powered without a PC connected to the USB port,
	// this will wait forever.
	usb_init();
	while (!usb_configured()) /* wait */ ;

	// Wait an extra second for the PC's operating system to load drivers
	// and do whatever it does to actually be ready for input
	_delay_ms(1000);
    
    // Check if the last two switches are already shorted:
    // read all port B pins
    b = PINB;
    // check if any pins are low, if so mark them as shorted (0), if they are still high, mark them as open (1)
   
    for (i=0; i<sizeof(pns); i++) {
        if (((b & pns[i]) == 0)) {
            pnsActive[i] = 0;
        } else {
            pnsActive[i] = 1;
        }
    }

    // for ports that are active (not shorted) on B, light corresponding LEDs on D
    for (i=0; i<sizeof(pns); i++) {
        if (pnsActive[i] == 0) {
            PORTD &= ~(pns[i]);
        } else {
            PORTD |= (pns[i]);
        }
    }
    

	while (1) {
		// read all port B pins
		b = PINB;
		// if the pins were not shorted on boot, check if any pins are low, but were high previously
		for (i=0; i<sizeof(pns); i++) {
			if (pnsActive[i] && (((b & pns[i]) == 0) && (b_prev & pns[i]) != 0)) {
				usb_keyboard_press(buttons[i], 0);
			}
		}

		// now the current pins will be the previous, and
		// wait a short delay so we're not highly sensitive
		// to mechanical "bounce".
		b_prev = b;
		_delay_ms(2);
	}
}


