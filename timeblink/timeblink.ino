void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

    noInterrupts();

    // initialize timer 1 control registers:
                //  7       6       5       4       3       2       1       0
    TCCR1A = 0; //  COM1A1  COM1A0  COM1B1  COM1B0  -       -       WGM11   WGM10 
    TCCR1B = 0; //  ICNC1   ICES1   -       WGM13   WGM12   CS12    CS11    CS10

    // initialize timer 1 counter
    TCNT1 = 0;

    // initialize Timer/Counter Interrupt Mask Register
    TIMSK1 = 0; //  -       -       ICIE1   -       -       OCIE1B  OCIE1A  TOIE0
    
    // clear interrupt flag register
    TIFR1 = 0;

    // enable CTC mode 4 (comparing with OCR1A)
    //  WGM13=0     WGM12=1     WGM11=0     WGM10
    TCCR1A |= 0;
    TCCR1B |= _BV(WGM12);

    /*
    // set prescalar as 1024
    //  CS12=1  CS11=0  CS10=1
    TCCR1A |= 0;
    TCCR1B |= _BV(CS12) | _BV(CS10);
    */

    // set prescalar as 256
    //  CS12=1  CS11=0  CS10=0
    TCCR1A |= 0;
    TCCR1B |= _BV(CS12);


    // once OCR1A reaches this value, interrupt triggers, and the timer resets
    OCR1A = 15625; // 16 MHz (clock) / 1024 (prescalar) / 4Hz (desired rate)
    TIMSK1 |= _BV(OCIE1A);

    interrupts();
}


// Interrupt Service Routine for when timer hits OCR1A
ISR(TIMER1_COMPA_vect) {
    digitalWrite(LED_BUILTIN, digitalRead(LED_BUILTIN) ^ 1); // toggle LED pin
}


void loop() {

}
