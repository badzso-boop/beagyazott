#define CPU 16000000UL // 16Mhz
#define UART_BAUDRATE 9600 //9600bps
#define UBRR_VALUE ((CPU/(UART_BAUDRATE*16UL))-1)
#define BUFFER_SIZE 50 // körbuffer mérete

#include <avr/io.h>
#include <avr/interrupt.h>

//körbuffer struktúra

volatile char ring_buffer[BUFFER_SIZE];
volatile uint8_t write_index = 0;
volatile uint8_t read_index = 0;


void UART_Init(int ubrr);
void UART_Transmit(char data);
char UART_Recive();

char data;

void setup() {
  // // put your setup code here, to run once:
  // UART_Init(UBRR_VALUE);
  USART_Init(UBRR_VALUE);
  data = UART_Recive();
  USART_SendString(data);
}

void loop() {
  // put your main code here, to run repeatedly:
  // data = UART_Recive();
  // UART_Transmit('[');
  // UART_Transmit(data);
  // UART_Transmit(']');

  char c = ring_buffer_read(); // körbufferből kapott karakter beolvasasa
  if (c) {
    USART_Transmit(c);// kiirjauk a kapott adatot az UART-ra
  }
}

// Interrupt service rutin
// ISR(USART0_RX_vect) {
//   char data;
//   data = UART_Recive();
//   UART_Transmit('[');
//   UART_Transmit(data);
//   UART_Transmit(']');
// }

// Körbfferbe karakter beolvasása megszakításon belül
ISR(USART0_RX_vect) {
  char received = UDR0; // beérkező karakter

  // következő index meghatározása
  // a buffer max indexe = BUFFER_SIZE-1;
  // ha a write_inded+1 = BUFFER_SIZE, akkor kezdődik előröl
  // az indexelés
  uint8_t next_index = (write_index + 1) % BUFFER_SIZE;

  // túlcsordulás elkerülése
  // ha a next_index = a read_index-el akkor tele a buffer
  if(next_index != read_index) {
    // ha nincs tele, akkor az adat tárolása a körbufferben
    ring_buffer[write_index] = received;
    write_index = next_index;
  }
}

// körbufferből egy karakter kiolvasása
char ring_buffer_read() {
  //van-e uj adat?
  if(read_index == write_index) {
    return 0; // ha nincs új adat visszatér a 0
  }

  // uj adat kiolvasasa
  char c = ring_buffer[read_index];
  // read_index frissitese
  read_index = (read_index + 1) % BUFFER_SIZE;
  return c;
}

void USART_Init(int ubrr) {
  cli(); // Globális megszakítások tiltása

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

  // megszaktás engedélyezése
  UCSR0B |= (1 << RXCIE0);
  sei(); // Globális megszíktások engedélyezése
}

void USART_Transmit(char data) {
  // Várakozas amig a TX buffer ures nem lesz
  while (!(UCSR0A & (1 << UDRE0)));

  // Az adat behelyezese a bufferbe, adat elkuldese
  UDR0 = data;
}

//String küldése az UART-on
void USART_SendString(const char *str) {
  // string = karakter tömb
  // a string végét mindig egy 0x00 értékú karakter zárja le
  // ha a while feltétele = 0 akkor kilép a ciklusból
  while(*str) {
    USART_Transmit(*str++);
  }
}

char UART_Recive() {
  // Varakozas amig adat nem erkezik
  while (!( UCSR0A & (1 << RXC0)));

  // adat kiolvasasa az UDR regiszterbol
  return UDR0;
}
