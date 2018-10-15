#define INTERVAL 400
#define DELAY1 300
#define DELAY0 30
#define SLEEPTIME (INTERVAL * 2)

void printInterval(int t) {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(t);
  digitalWrite(LED_BUILTIN, LOW);
  delay(INTERVAL-t);
}

void print0() {
  printInterval(DELAY0);
}
void print1() {
  printInterval(DELAY1);
}
void printSleep() {
  delay(SLEEPTIME);
}
void printDelim(int i) {
  printSleep();
  for (; i > 0; i--) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }
  printSleep();
}

#define printType(v) \
  do { \
    printDelim(sizeof(v)); \
    for (int i = sizeof(v) * 8; i > 0; i--) { \
      ((unsigned long) (v)) & (0x1 << (i - 1)) ? print1() : print0(); \
    printDelim(sizeof(v)); \
    } \
  } while(0)

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  printDelim(10);
  //void *v = loop;
  int v = 0x8000;
  printType(v);

  printDelim(20);
}
