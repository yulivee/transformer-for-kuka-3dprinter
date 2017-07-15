#include <pins.h>
#include <pid.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#ifdef __cplusplus
extern "C" {
#endif

void timer_init() {
	TCCR0A=(1<<WGM01);
	TCCR0B=(5<<CS00);
	OCR0A=F_CPU/1024/GOVERNOR_FREQ;
	TIMSK0=(1<<OCIE0A);
}

ISR(TIMER0_COMPA_vect) {

	DDRB|=0x20;
	PORTB|=0x20;
        positionDelta = target_position - motor_cnt.cnt0;
        positionDiff = positionDelta - positionLastDelta;
        positionLastDelta = positionDelta;
        positionInt += positionDelta;
        BOUNDS ( positionInt , MOVEIMAX );
	if ( target_position == motor_cnt.cnt0 ) {
		positionSpeed = 0;
	} else {
		positionSpeed = positionDelta * MOVEP + positionInt * MOVEI + positionDiff * MOVED;
	}
        BOUNDS ( positionSpeed , MOVEMAX );

        if ( positionSpeed >= 0 ) {
            analogWrite(motor_pins.right, positionSpeed);
            analogWrite(motor_pins.left, 0);
        } else {
            analogWrite(motor_pins.left, 0 - positionSpeed);
            analogWrite(motor_pins.right, 0);
        }
	PORTB&=~0x20;

}

#ifdef __cplusplus
	}
#endif
