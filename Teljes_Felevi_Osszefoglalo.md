# TELJES FÉLÉVI GYAKORLATI ÖSSZEFOGLALÓ
**Beágyazott Rendszerek LA**

Ez a dokumentum tartalmazza az eddigi összes gyakorlati óra legfontosabb kódjait és megoldásait. Ha a ZH-n olyan feladat jön elő, ami a "kis puskában" nincs benne (pl. regiszter szintű vezérlés vagy UART megszakítás), akkor innen tudsz puskázni.

---

## 1. ALACSONY SZINTŰ PORTKEZELÉS (Bitműveletek)
* **Forrás:** `3. ora/la3.ino`
* **Mikor kell?** Ha tiltják a `digitalWrite`-ot, vagy közvetlenül a regisztereket kell írni.

Az Arduino Mega 2560-on a 13-as láb a **PB7** bithez tartozik.

```c
void setup() {
  // Kimenet beállítása (DDR = Data Direction Register)
  // PB7 legyen kimenet (1-es bit)
  DDRB |= (1 << PB7); 
}

void loop() {
  // 1. LED Bekapcsolása (PORT regiszter írása)
  // A PB7 bitet 1-be állítjuk (VAGY művelet)
  PORTB |= (1 << PB7); 
  
  delay(300); // Itt még használtunk delay-t

  // 2. LED Kikapcsolása
  // A PB7 bitet 0-ba állítjuk (ÉS művelet a negálttal)
  PORTB &= ~(1 << PB7); 
  
  delay(300);

  // 3. LED Toggle (Állapotváltás) - XOR művelet
  // PORTB ^= (1 << PB7);
}
```

---

## 2. UART KOMMUNIKÁCIÓ (Regiszter szinten + Megszakítással)
* **Forrás:** `7. ora/sketch_oct22a`
* **Elmélet:** `8. ora/UART jegyzet.pdf`, `7. ora/02_UART_kommunikacio.pdf`
* **Mikor kell?** Ha "hardver közeli" soros kommunikációt kérnek, vagy körbuffert kell implementálni.

Ha csak simán ki kell írni valamit, használd a `Serial.begin(9600)` és `Serial.println()` parancsokat! Az alábbi kód a "hogyan működik a motorháztető alatt" verzió.

### Inicializálás és Adatküldés
```c
#define CPU 16000000UL 
#define UART_BAUDRATE 9600
#define UBRR_VALUE ((CPU/(UART_BAUDRATE*16UL))-1)

void UART_Init(int ubrr) {
  // Baud rate beállítása
  UBRR0L = ubrr;
  UBRR0H = (ubrr >> 8);

  // Aszinkron mód, 8 adatbit, nincs paritás, 1 stop bit
  UCSR0C &= ~((1 << UMSEL01) | (1 << UMSEL00)); // Aszinkron
  UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);      // 8 bit
  UCSR0B |= (1 << RXEN0)  | (1 << TXEN0);       // Adó+Vevő bekapcs
}

void UART_Transmit(char data) {
  // Várakozás, amíg a buffer üres nem lesz
  while (!(UCSR0A & (1 << UDRE0)));
  UDR0 = data; // Adat küldése
}
```

### Vétel Megszakítással (Interrupt) és Körbufferrel
Ez a legprofibb módja az adatfogadásnak, mert nem blokkolja a főprogramot.

```c
#define BUFFER_SIZE 50
volatile char ring_buffer[BUFFER_SIZE];
volatile uint8_t write_index = 0;
volatile uint8_t read_index = 0;

// Inicializálásnál engedélyezni kell a megszakítást!
// UCSR0B |= (1 << RXCIE0); 
// sei(); // Globális megszakítás engedélyezése

// ISR: Ez fut le automatikusan, ha jön adat
ISR(USART0_RX_vect) {
  char received = UDR0; 
  uint8_t next_index = (write_index + 1) % BUFFER_SIZE;

  if(next_index != read_index) { // Ha nincs tele a buffer
    ring_buffer[write_index] = received;
    write_index = next_index;
  }
}

// Olvasás a bufferből a főprogramban
char ring_buffer_read() {
  if(read_index == write_index) return 0; // Nincs új adat
  
  char c = ring_buffer[read_index];
  read_index = (read_index + 1) % BUFFER_SIZE;
  return c;
}
```

---

## 3. IDŐZÍTÉS (Millis és Timer Struct)
* **Forrás:** `8. ora` (alapok), `9. ora` (Struct)
* **Elmélet:** `8. ora/Delay, Millis...`

Ez a "Szent Grál" a ZH-n. Delay helyett **mindig** ezt használd!

```c
typedef struct {
  unsigned long prevTime;
  int deltaTime;
} Timer_s;

Timer_s MyTimer = { .prevTime = 0, .deltaTime = 1000 }; // 1 sec

unsigned char CheckTimer(Timer_s *timer) {
  unsigned long actTime = millis();
  if(actTime - timer->prevTime > timer->deltaTime) {
    timer->prevTime = actTime;
    return 1; // Eltelt
  }
  return 0;
}

// Használat loop-ban:
// if (CheckTimer(&MyTimer)) { ... }
```

---

## 4. ÁLLAPOTGÉP (State Machine)
* **Forrás:** `9. ora/sketch_nov5a`
* **Elmélet:** `9. ora/Finite_State_Machine.pdf`

Vezérlésekhez kötelező elem.

```c
typedef enum {
  ST_IDLE,
  ST_WORK,
  ST_ERROR
} State_e;

State_e State = ST_IDLE;

void loop() {
  // Állapotok közötti váltás és cselekvés
  switch (State) {
    case ST_IDLE:
      // Ha gombnyomás -> State = ST_WORK;
      break;
    case ST_WORK:
      // Ha kész -> State = ST_IDLE;
      break;
  }
}
```

---

## 5. SZŰRÉS ÉS ALGORITMUSOK
* **Forrás:** `10. ora` (Medián), `12. ora` (Átlag)
* **Elmélet:** `10. ora/tavolsagmero.pdf`

### A) Átlagoló Szűrő (Mozgóátlag)
Egyszerűbb, gyorsabb, de az extrém kiugró értékek elronthatják.

```c
#define BUF_SIZE 5
int buffer[BUF_SIZE];

// FIFO hozzáadás
void addData(int data) {
  for(int i=0; i<BUF_SIZE-1; i++) {
    buffer[i] = buffer[i+1];
  }
  buffer[BUF_SIZE-1] = data;
}

int getAverage() {
  long sum = 0;
  for(int i=0; i<BUF_SIZE; i++) {
    sum += buffer[i];
  }
  return sum / BUF_SIZE;
}
```

### B) Medián Szűrő (Rendezéssel)
Zajos jeleknél (pl. ultrahang) sokkal jobb, mert eldobja a hibás kiugrásokat.

```c
// Buborékrendezés (Bubble Sort)
void rendez() {
  // Először átmásoljuk a buffert egy bufferCopy-ba!
  for(int i=0; i<BUF_SIZE; i++) bufferCopy[i] = buffer[i];
  
  // Rendezés a másolaton
  for(int i=0; i<BUF_SIZE; i++) {
    for(int j=i; j<BUF_SIZE; j++) {
      if(bufferCopy[i] > bufferCopy[j]) {
        int tmp = bufferCopy[i];
        bufferCopy[i] = bufferCopy[j];
        bufferCopy[j] = tmp;
      }
    }
  }
}
// Medián = bufferCopy[BUF_SIZE/2];
```

---

## 6. SZENZOROK KEZELÉSE

### A) HC-SR04 Ultrahangos Távolságmérő
* **Forrás:** `10. ora`

```c
digitalWrite(trigPin, LOW); delayMicroseconds(2);
digitalWrite(trigPin, HIGH); delayMicroseconds(10);
digitalWrite(trigPin, LOW);

long duration = pulseIn(echoPin, HIGH);
int distance = duration * 0.034 / 2; // cm-ben
```

### B) Analóg (Potméter / Hőmérő)
* **Forrás:** `12. ora`

```c
int celsuis = 0;
celsius = map(((analogRead(A0) - 20) * 3.04), 0, 1023, -40, 125);
Serial.print(celsius);
Serial.println(" C, ");
```
