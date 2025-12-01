/*
 * MINTA ZH FELADAT - "A" CSOPORT
 * Téma: Állapotgép vezérlés Medián szűrővel + HC-SR04 Szenzor
 * 
 * ================================================================
 * TINKERCAD BEKÖTÉS (Hardware Setup):
 * ================================================================
 * 1. SZENZOR: HC-SR04 Ultrahangos Távolságmérő
 *    - VCC  --> 5V
 *    - GND  --> GND
 *    - TRIG --> D9  (Digital Pin 9)
 *    - ECHO --> D10 (Digital Pin 10)
 * 
 * 2. LEDEK (Kimenetek - 220 Ohm előtét ellenállással):
 *    - Zöld LED (GLED) anód  --> D3
 *    - Sárga LED (YLED) anód --> D4
 *    - Piros LED (RLED) anód --> D5
 *    - Minden LED katódja    --> GND
 * ================================================================
 * 
 * FELADAT LEÍRÁSA:
 * Készítsen egy Arduino programot, amely a HC-SR04 szenzorral távolságot mér
 * és az érték alapján állapotokat vált. Mivel a szenzor zajos lehet, 
 * a mért adatokat szűrni kell MEDIÁN szűrővel.
 * 
 * KÖVETELMÉNYEK:
 * 1. Bemenet: 
 *    - Használja a 'pulseIn()' függvényt a távolság mérésére (lásd órai anyag).
 * 
 * 2. Szűrés (KÖTELEZŐ):
 *    - Tárolja az utolsó 10 mérést egy tömbben.
 *    - Rendezze sorba az adatokat (buborékrendezés).
 *    - A döntéshozatalhoz a MEDIÁN (középső) értéket használja.
 * 
 * 3. Állapotgép (KÖTELEZŐ):
 *    - NORMAL (Zöld LED): Távolság > 150 cm.
 *    - WARNING (Sárga LED): Távolság 50 és 150 cm között. 
 *      (Sárga LED villog: 500ms).
 *    - DANGER (Piros LED): Távolság < 50 cm. (Piros LED világít).
 * 
 * 4. Időzítés:
 *    - Tilos a delay() használata a loop()-on belül!
 */







































 

// --- MEGOLDÁS ---

// Pin definíciók
#define GLED 3
#define YLED 4
#define RLED 5
#define TRIG_PIN 9
#define ECHO_PIN 10

// Szűrő paraméterek
#define BUFFER_SIZE 10
int buffer[BUFFER_SIZE];
int bufferCopy[BUFFER_SIZE];

// 1. IDŐZÍTŐ LOGIKA
typedef struct {
  unsigned long prevTime;
  int deltaTime;
} Timer_s;

Timer_s BlinkTimer = { .prevTime = 0, .deltaTime = 500 };
Timer_s MeasureTimer = { .prevTime = 0, .deltaTime = 100 }; // 100ms mérés

unsigned char CheckTimer(Timer_s *timer) {
  unsigned long actTime = millis();
  if(actTime - timer->prevTime > timer->deltaTime) {
    timer->prevTime = actTime;
    return 1; 
  }
  return 0; 
}

// 2. ÁLLAPOTOK
typedef enum {
  ST_NORMAL,
  ST_WARNING,
  ST_DANGER
} State_e;

State_e State = ST_NORMAL;

// 3. SZŰRŐ FÜGGVÉNYEK

void addData(int data) {
  for(byte i = 0; i < BUFFER_SIZE-1; i++) {
    buffer[i] = buffer[i+1];
  }
  buffer[BUFFER_SIZE-1] = data;
}

void rendez() {
  for(int j = 0; j < BUFFER_SIZE; j++) {
    bufferCopy[j] = buffer[j];
  }
  int tmp;
  for(int i = 0; i < BUFFER_SIZE; i++) {
    for(int j = i; j < BUFFER_SIZE; j++) {
      if(bufferCopy[i] > bufferCopy[j]) {
        tmp = bufferCopy[i];
        bufferCopy[i] = bufferCopy[j];
        bufferCopy[j] = tmp;
      }
    }
  }
}

int getMedian() {
  return bufferCopy[BUFFER_SIZE/2];
}

// 4. MÉRÉS (HC-SR04 logika)
int measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  // Távolság = idő * sebesség (0.034 cm/us) / 2 (oda-vissza út)
  int distance = duration * 0.034 / 2;
  return distance;
}

void setup() {
  pinMode(GLED, OUTPUT);
  pinMode(YLED, OUTPUT);
  pinMode(RLED, OUTPUT);
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  Serial.begin(9600);
  digitalWrite(GLED, HIGH);
}

void loop() {
  
  if(CheckTimer(&MeasureTimer)) {
    // Mérés és Szűrés
    int rawValue = measureDistance();
    
    addData(rawValue);
    rendez();
    int filteredValue = getMedian();
    
    Serial.print("Raw: "); Serial.print(rawValue);
    Serial.print(" cm | Filtered: "); Serial.println(filteredValue);
    
    // Állapotváltás (cm alapján)
    if (filteredValue > 150) {
      if(State != ST_NORMAL) {
        State = ST_NORMAL;
        digitalWrite(GLED, HIGH);
        digitalWrite(YLED, LOW);
        digitalWrite(RLED, LOW);
      }
    } 
    else if (filteredValue >= 50 && filteredValue <= 150) {
      if(State != ST_WARNING) {
        State = ST_WARNING;
        digitalWrite(GLED, LOW);
        digitalWrite(RLED, LOW);
      }
    }
    else { // < 50 cm
       if(State != ST_DANGER) {
        State = ST_DANGER;
        digitalWrite(GLED, LOW);
        digitalWrite(YLED, LOW);
        digitalWrite(RLED, HIGH);
       }
    }
  }

  // Állapot viselkedés
  switch (State) {
    case ST_NORMAL: break;
    case ST_WARNING:
      if(CheckTimer(&BlinkTimer)) {
        digitalWrite(YLED, 1 - digitalRead(YLED));
      }
      break;
    case ST_DANGER: break;
  }
}