void setup() {
  // PB7-es lába beállítása kimenetnek
  DDRB |= (1 << PB7);
  //pinMode(13, OUTPUT);
}


void loop() {
  // bit billegtetése XOR művelettel (toggle)
  //PORTB ^= (1 << PB7); // (1 << PB7) => 0b10000000


  // put your main code here, to run repeatedly:
  // PORTB7 bit értékeinek beállítása 0-re
  PORTB &= ~(1 << PB7); // ~(1 << PB7) => 0b01111111
  //digitalWrite(13, HIGH);
  delay(300);

  // PORTB7 bit értékeinek beállítása 1-re
  PORTB |= (1 << PB7); // (1 << PB7) => 0b10000000
  //digitalWrite(13, LOW);
  delay(300);
}
