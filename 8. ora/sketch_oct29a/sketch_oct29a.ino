#define LED 13

volatile uint8_t global_counter = 0;
void InitTimer5();

void setup() {
  pinMode(LED, OUTPUT);
  InitTimer5();
}

void InitTimer5() {
  TCCR5A = 0;
  TCCR5B = 0;
  TCCR5B = B00000101; // 1024 prescaler
  TCNT5 = 65535-15625;
  TIMSK5 |= (1<<TOIE5); 
}

ISR(TIMER5_OVF_vect) {
  global_counter++;
  TCNT5 = 65535-15625;
}

void loop() {
  if(global_counter == 10) {
    global_counter = 0;
    digitalWrite(LED, 1-digitalRead(LED));
  }
}

//a global counter azert van mert 10 masodpercenkent akarom a ledet kapcsolgatni.
// Mivel a 16 bites szamlaloba a 10 masodperc mar nem fer bele ezert bevezettem egy global_countert
// ami masodpercenkent novelodik es a loopba megnezem hogy mikor 10.
// csunyacska megoldas, mert a loop is visz el idot, de elenyeszo idot visz el.
