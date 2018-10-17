/*
 * This toy program simulates two concurrent, synchronous processes that are
 * each performing the Collatz operation from a different seed. In between
 * operations, they sleep for as long as their running value.
 * 
 * Imperatively, this is what each process is doing:
 *
 *  int c = INIT;
 *  while (c != 1) {
 *      if (c % 2)
 *          c = c * 3 + 1;
 *      else
 *          c = c / 2;
 *      sleep(c);
 *  }
 *
 * Once terminated (i.e. reached 1), each process will toggle the LED.
 * Because there are 2 processes in this example, the LED will blink at 2Hz
 * between the completion of the first process and the completion of the second
 * process. After that, the nearly instantaneous toggling will cancel out,
 * signaling that both processes have terminated.
 */

/*
 * Each process some of its own local state to do whatever with.
 *
 * TODO: processes should be able to peek into the _previous_ states of itself
 * and of other concurrent processes (as long as it doesn't modify it). We can
 * implement this using a circular buffer.
 */
struct state {
    uint8_t _mem[512]; // each process gets 512B of memory
};

typedef uint32_t vtime_t;   // virtual unit of time representing one tick
struct proc {
    vtime_t sleep_time;     // time remaining to sleep
    vtime_t (*transition) (struct state *);
    struct state state;
};

/*
 * transition function for our collatz process
 */
vtime_t transitionCollatz(struct state *state) {
    uint32_t *c = (uint32_t *) state; // we're really only using 4B
    if (*c == 1) {
        return 0; // we are done, busy wait and trigger toggle
    }

    *c = *c % 2 == 0 ? *c / 2 : *c * 3 + 1; // collatz conditional

    return (vtime_t) *c; // sleep for as long as c
}


void initCollatz(struct proc *proc, uint32_t x) {
    *(uint32_t *) &proc->state = proc->sleep_time = x;
    proc->transition = transitionCollatz;
}

// two concurrent processes
struct proc ps[2];

// 4Hz interrupt rate
void setup_timer(void) {
    noInterrupts();

    // bit num:     7       6       5       4       3       2       1       0

    // initialize timer 1 control registers:
    TCCR1A = 0; //  COM1A1  COM1A0  COM1B1  COM1B0  -       -       WGM11   WGM10 
    TCCR1B = 0; //  ICNC1   ICES1   -       WGM13   WGM12   CS12    CS11    CS10

    // initialize Timer/Counter Interrupt Mask Register:
    TIMSK1 = 0; //  -       -       ICIE1   -       -       OCIE1B  OCIE1A  TOIE0

    // enable CTC mode 4 (comparing with OCR1A)
    //  WGM13=0     WGM12=1     WGM11=0     WGM10
    TCCR1B |= _BV(WGM12);

    // set prescalar as 256
    //  CS12=1  CS11=0  CS10=0
    TCCR1B |= _BV(CS12);

    // once OCR1A reaches this value, interrupt triggers, and the timer resets
    OCR1A = 15625; // 16 MHz (clock) / 1024 (prescalar) / 4Hz (desired rate)
    TIMSK1 |= _BV(OCIE1A);

    TCNT1 = 0;  // initialize timer 1 counter
    TIFR1 = 0;  // clear interrupt flag register

    interrupts();
}


void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

    // these will take too long
    /*initCollatz(ps,     97); // should take 118 steps*/
    /*initCollatz(ps + 1, 27); // should take 111 steps*/

    initCollatz(ps,      9); // should take 19 steps
    initCollatz(ps + 1, 12); // should take 9 steps

    setup_timer();
}


// Interrupt Service Routine for when timer hits OCR1A
ISR(TIMER1_COMPA_vect) {
    int i;
    for (i = 0; i < sizeof(ps) / sizeof(*ps); i++) { // for each process
        struct proc *p = ps + i;

        if (p->sleep_time) {
            p->sleep_time--;
            // continue to sleep
        } else {
            // wake up and transition
            p->sleep_time = p->transition(&p->state);

            if (!p->sleep_time)
                // we are done; toggle LED pin
                digitalWrite(LED_BUILTIN, digitalRead(LED_BUILTIN) ^ 1);
        }
    }
}

void loop() {
}
