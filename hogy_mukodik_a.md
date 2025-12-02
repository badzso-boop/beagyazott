1. Konstansok + Változók definialasa 

```c
#define GLED 3
#define YLED 4
#define RLED 5

#define TRIG_PIN 7
#define ECHO_PIN 7

int inches = 0;
int cm = 0;
```

2. Időzítéshez fontos változók
```c
// IDŐZÍTŐ LOGIKA

////////////////////////////////////
////    Tinkercades megoldás        ////
////                                ////
////    struct Timer_s{             ////
////        unsigned long prevTime; ////
////        int deltaTime;          ////
////    };                          ////
////                                ////
////////////////////////////////////////
////    Arduino IDE megoldás        ////
////                                ////
////    typedef struct {            ////
////        unsigned long prevTime; ////
////        int deltaTime;          ////
////    } Timer_s;                  ////
////                                ////
////////////////////////////////////////

Timer_s BlinkTimer = { .prevTime = 0, .deltaTime = 500 };
Timer_s MeasureTimer = { .prevTime = 0, .deltaTime = 100 }; // 100ms mérés
```

3. Szűréshez fontos változók
```c
#define BUFFER_SIZE 10
int buffer[BUFFER_SIZE];
int bufferCopy[BUFFER_SIZE];
```

4. Állapothoz fontos változók

```c
// 2. ÁLLAPOTOK
typedef enum {
  ST_NORMAL,
  ST_WARNING,
  ST_DANGER
} State_e;

State_e State = ST_NORMAL;
```

5. fuggvenyek definialasa a timerhez

```c
unsigned char CheckTimer(struct Timer_s *timer) {
  unsigned long actTime = millis();
  if(actTime - timer->prevTime > timer->deltaTime) {
    timer->prevTime = actTime;
    return 1; 
  }
  return 0; 
}
```

6. fuggvenyek a szurohoz

```c
// Új adat hozzáadása a tömbhöz (csúszóablak elv - "FIFO szerű")
// A legrégebbi elem kiesik, az újak hátrébb csúsznak
void addSimple(int data) {
  for(byte i = 0; i < BUFFER_SIZE-1; i++) {
    buffer[i] = buffer[i+1];
  }
  buffer[BUFFER_SIZE-1] = data;
}

// Buborékrendezés (Bubble Sort) a mediánhoz
// Fontos: Mindig a másolaton dolgozunk, hogy az időrendiség megmaradjon az eredetiben!
void rendez() {
  // 1. Másolat készítése
  for(int j = 0; j < BUFFER_SIZE; j++) {
    bufferCopy[j] = buffer[j];
  }

  // 2. Rendezés (Növekvő sorrend)
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

// Medián lekérése (Rendezés után! - Medián szűrőhöz)
int getMedian() {
  // A rendezett tömb középső eleme
  return bufferCopy[BUFFER_SIZE/2];
}

// Átlag számítása (Mozgóátlag szűrőhöz)
int makeAvg() {
  long sum = 0;
  for(byte i = 0; i < BUFFER_SIZE; i++) {
    sum += buffer[i]; // Vagy bufferCopy, mindegy összegnél
  }
  return sum / BUFFER_SIZE;
}
```

7. Távolságmérő szenzor függvény

`Ezt egyébként a tinkercad mintákból loptam mi nem így valósítottuk meg órán.`

```c
long readUltrasonicDistance(int triggerPin, int echoPin)
{
  pinMode(triggerPin, OUTPUT);  // Clear the trigger
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  // Sets the trigger pin to HIGH state for 10 microseconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  // Reads the echo pin, and returns the sound wave travel time in microseconds
  return pulseIn(echoPin, HIGH);
}
```

8. Setup beállítása

```c
void setup()
{
  pinMode(GLED, OUTPUT);
  pinMode(YLED, OUTPUT);
  pinMode(RLED, OUTPUT);
  
  Serial.begin(9600);
  
  digitalWrite(GLED, HIGH);
}
```

Most jon az izgi rész :)
```c
                                                             void loop()
  ||                                                                                          ||
  \/                                                                                          \/
if(CheckTimer(&MeasureTimer))                                                              switch (State)
//ha a beállított                                                                 // lekéri hogy éppen milyen stateban 
// Timer_s MeasureTimer = { .prevTime = 0,                                        // van és ennek megfelelően csinál valamit
// .deltaTime = 100 };                                                            // csak a sárgánál csinál valamit
// // 100ms mérés elérte a megfelelő időt                                                       ||
// akkor lép be ide                                                                             \/
if (filteredValue > 150) {                                                             if(CheckTimer(&BlinkTimer)) {
      if(State != ST_NORMAL) {                                                          digitalWrite(YLED, 1 - digitalRead(YLED));
        State = ST_NORMAL;                                                             }
        digitalWrite(GLED, HIGH);                                                      //ez ismét lekéri hogy a blinkkertimer elérte-e
        digitalWrite(YLED, LOW);                                                       //már a megfelelő időt
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
                                                        
                                                        


```