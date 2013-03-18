#define F_CPU 1000000UL  /* 1 MHz CPU clock */

#include <util/delay.h>
#include <avr/io.h>

#include "BMP085.h"
#include "USI_TWI.h"


void BuzzInit() {
	DDRB = (1<<PB4);
}

void BuzzOn() {
	PORTB |= (1<<PB4); 
}

void BuzzOff() {
	PORTB &= (0<<PB4); 
}


void Buzz100ms() {
	BuzzOn();
	_delay_ms(100);
	BuzzOff();
	_delay_ms(1000);
}

int main(void) {
	float curr;
	BuzzInit();

	BMP085_begin(BMP085_STANDARD);

	for(;;) {		
		curr = BMP085_readAltitude(101500);

		if ((curr > 100) & (curr < 250)) {
			Buzz100ms();
		}		

		TWI_begin();
	}

	return 0;
}
