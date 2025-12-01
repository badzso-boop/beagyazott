/*
 * MINTA ZH FELADAT - "B" CSOPORT
 * Téma: Állapotgép vezérlés Mozgóátlag szűrővel + TMP36 Szenzor
 * 
 * ================================================================
 * TINKERCAD BEKÖTÉS (Hardware Setup):
 * ================================================================
 * 1. SZENZOR: "Temperature Sensor [TMP36]" (Tinkercad-ben)
 *    - Bal láb (Power)   --> 5V
 *    - Jobb láb (GND)    --> GND
 *    - Középső láb (Vout)--> A1 (Analog Pin 1)
 *    
 *    MEGJEGYZÉS: A szimuláció egyszerűsítése érdekében a TMP36 szenzort 
 *    használjuk az NTC termisztor helyett, mert ennek lineáris a kimenete
 *    és könnyebb kezelni vizsgakörnyezetben.
 * 
 * 2. LEDEK (Kimenetek - 220 Ohm előtét ellenállással):
 *    - Zöld LED (GLED) anód  --> D3
 *    - Sárga LED (YLED) anód --> D4
 *    - Piros LED (RLED) anód --> D5
 *    - Minden LED katódja    --> GND
 * ================================================================
 * 
 * FELADAT LEÍRÁSA:
 * Készítsen egy Arduino programot, amely a TMP36 szenzorral hőmérsékletet figyel.
 * A jel zajos, ezért átlagoló szűrőt kell alkalmazni.
 * 
 * KÖVETELMÉNYEK:
 * 1. Bemenet: 
 *    - Használja az 'analogRead(A1)' függvényt.
 *    - Konverzió: °C = (Feszültség[V] - 0.5) * 100
 * 
 * 2. Szűrés (KÖTELEZŐ):
 *    - Tárolja az utolsó 5 mérést egy tömbben.
 *    - Számítsa ki az elemek számtani ÁTLAGÁT.
 * 
 * 3. Állapotgép (KÖTELEZŐ):
 *    - COLD (Sárga LED): Hőmérséklet < 20 °C.
 *    - OPTIMAL (Zöld LED): Hőmérséklet 20 °C és 30 °C között.
 *    - HOT (Piros LED): Hőmérséklet > 30 °C. 
 *      (Piros LED villog: 200ms).
 *      HA HOT állapotban van > 5 másodpercig, bekapcsol a Sárga LED is (Vészjelzés).
 * 
 * 4. Időzítés:
 *    - Használja a 'millis()' függvényt.
 */

// --- MEGOLDÁS ---

#define GLED 3
#define YLED 4
#define RLED 5
#define SENSOR_PIN A1

#define BUFFER_SIZE 5
int buffer[BUFFER_SIZE]; // Itt tároljuk a hőmérsékletet (egész számként)

// 1. IDŐZÍTŐ LOGIKA
typedef struct {
  unsigned long prevTime;
  int deltaTime;
} Timer_s;

Timer_s BlinkTimer = { .prevTime = 0, .deltaTime = 200 };
Timer_s MeasureTimer = { .prevTime = 0, .deltaTime = 100 };

unsigned long hotStateStartTime = 0;
bool isOverheated = false; 

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
  ST_COLD,
  ST_OPTIMAL,
  ST_HOT
} State_e;

State_e State = ST_COLD;

// 3. SZŰRÉS
void addData(int data) {
  for(byte i = 0; i < BUFFER_SIZE-1; i++) {
    buffer[i] = buffer[i+1];
  }
  buffer[BUFFER_SIZE-1] = data;
}

int getAverage() {
  long sum = 0;
  for(byte i = 0; i < BUFFER_SIZE; i++) {
    sum += buffer[i];
  }
  return (int)(sum / BUFFER_SIZE);
}

void setup() {
  pinMode(GLED, OUTPUT);
  pinMode(YLED, OUTPUT);
  pinMode(RLED, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  
  if(CheckTimer(&MeasureTimer)) {
    int rawAdc = analogRead(SENSOR_PIN);
    
    // KONVERZIÓ TMP36-hoz:
    // 1. ADC -> Feszültség (0-5V)
    float voltage = rawAdc * (5.0 / 1023.0);
    // 2. Feszültség -> Celsius fok
    // 10mV / fok, 500mV offset (0 foknál 0.5V)
    int tempC = (voltage - 0.5) * 100; 

    addData(tempC);
    int avgTemp = getAverage();

    Serial.print("Temp: "); Serial.println(avgTemp);

    // Állapotváltás logika (°C alapján)
    switch (State) {
      case ST_COLD:
        if (avgTemp >= 20) {
          State = ST_OPTIMAL;
          digitalWrite(YLED, LOW);
          digitalWrite(GLED, HIGH);
        }
        break;

      case ST_OPTIMAL:
        if (avgTemp < 20) {
          State = ST_COLD;
          digitalWrite(GLED, LOW);
          digitalWrite(YLED, HIGH);
        } 
        else if (avgTemp > 30) {
          State = ST_HOT;
          digitalWrite(GLED, LOW);
          hotStateStartTime = millis(); 
          isOverheated = false;
        }
        break;

      case ST_HOT:
        if (avgTemp <= 30) {
          State = ST_OPTIMAL;
          digitalWrite(RLED, LOW);
          digitalWrite(YLED, LOW);
          digitalWrite(GLED, HIGH);
        }
        break;
    }
  }

  // Állapot viselkedés
  switch (State) {
    case ST_COLD: break;
    case ST_OPTIMAL: break;
    case ST_HOT:
      if(CheckTimer(&BlinkTimer)) {
        digitalWrite(RLED, 1 - digitalRead(RLED));
      }
      if (!isOverheated && (millis() - hotStateStartTime > 5000)) {
        isOverheated = true;
        digitalWrite(YLED, HIGH); 
      }
      break;
  }
}