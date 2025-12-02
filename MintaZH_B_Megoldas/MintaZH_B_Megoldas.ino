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

// C++ code
//
#define GLED 3
#define YLED 4
#define RLED 5

#define SENSOR_PIN A1
int celsius = 0;

#define BUF_SIZE 5
int buffer[BUF_SIZE];

// 1. IDŐZÍTŐ LOGIKA
struct Timer_s{
  unsigned long prevTime;
  int deltaTime;
};

Timer_s BlinkTimer = { .prevTime = 0, .deltaTime = 200 };
Timer_s MeasureTimer = { .prevTime = 0, .deltaTime = 100 };

unsigned long hotStateStartTime = 0;
bool isOverheated = false; 

unsigned char CheckTimer(struct Timer_s *timer) {
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
  for(byte i = 0; i < BUF_SIZE-1; i++) {
    buffer[i] = buffer[i+1];
  }
  buffer[BUF_SIZE-1] = data;
}

int getAverage() {
  long sum = 0;
  for(byte i=0; i<BUF_SIZE; i++) {
    sum += buffer[i];
  }
  return (int)(sum / BUF_SIZE);
}


void setup()
{
  Serial.begin(9600); 
  pinMode(GLED, OUTPUT);
  pinMode(YLED, OUTPUT);
  pinMode(RLED, OUTPUT);
}

void loop()
{
  if(CheckTimer(&MeasureTimer)) {
    //netrol lopott hogy igy kell csinalni a tinkercades homeronel
    celsius = map(((analogRead(SENSOR_PIN) - 20) * 3.04), 0, 1023, -40, 125);
    Serial.print(celsius);
    Serial.println(" C, ");
    
    addData(celsius);
    int avarage = getAverage();
    
    Serial.print("Temp: "); Serial.println(avarage);
    
    
    // Állapotváltás logika (°C alapján)
    switch (State) {
      case ST_COLD:
        if (avarage >= 20) {
          State = ST_OPTIMAL;
          digitalWrite(YLED, LOW);
          digitalWrite(GLED, HIGH);
        }
        break;

      case ST_OPTIMAL:
        if (avarage < 20) {
          State = ST_COLD;
          digitalWrite(GLED, LOW);
          digitalWrite(YLED, HIGH);
        } 
        else if (avarage > 30) {
          State = ST_HOT;
          digitalWrite(GLED, LOW);
          hotStateStartTime = millis(); 
          isOverheated = false;
        }
        break;

      case ST_HOT:
        if (avarage <= 30) {
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