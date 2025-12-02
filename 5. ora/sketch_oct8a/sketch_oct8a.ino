#define CPU 16000000UL // 16Mhz
#define UART_BAUDRATE 9600 //9600bps
#define UBRR_VALUE ((CPU/(UART_BAUDRATE*16UL))-1)

void UART_Init(int ubrr);
void UART_Transmit(char data);
char UART_Recive();


char data;

void setup() {
  // put your setup code here, to run once:
  UART_Init(UBRR_VALUE);
}

void loop() {
  // put your main code here, to run repeatedly:
  data = UART_Recive();
  UART_Transmit('[');
  UART_Transmit(data);
  UART_Transmit(']');
}

void UART_Init(int ubrr) {
  //9600 bps sebesseg beallitasa
  // UBRR also 8 bitjének betoltese az UBRRL regiszterbe
  UBRR0L = ubrr;
  // UBRR felso 8 bitjének betoltese az UBRRL regiszterbe
  // elshifteljuk 8 bitet jobbra az erteket mert 16 bit ez az ubrr es mi nekunk ketto 8 bites regiszterbe kell raknom
  UBRR0H = (ubrr >> 8);

  // Aszinkron mod -> pptbe 20.dia
  UCSR0C &= ~( (1 << UMSEL01 ) | (1 << UMSEL00) );

  // 8 adat bit
  UCSR0B &= ~(1 << UCSZ02);
  UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);

  // nincs paritás
  UCSR0C &= ~((1 << UPM01) | (1 << UPM00));

  // 1 stop bit
  UCSR0C &= ~(1 << USBS0);

  // Adó és vevő bekapcsolása
  UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
}

void UART_Transmit(char data) {
  // Varakozas amig a TX buffer ures nem lesz
  while (!(UCSR0A & (1 << UDRE0)));

  // Az adat behelyezese a bufferbe, adat elkuldese
  UDR0 = data;
}

char UART_Recive() {
  // Varakozas amig adat nem erkezik
  while (!( UCSR0A & (1 << RXC0)));

  // adat kiolvasasa az UDR regiszterbol
  return UDR0;
}
