# BEÁGYAZOTT RENDSZEREK - GYAKORLATI ÖSSZEFOGLALÓ

Ez a dokumentum tartalmazza azokat a kódrészleteket (építőkockákat), amelyeket a ZH-n használni lehet. A kódok mellett feltüntettem, melyik PDF-ben található a hozzá tartozó elméleti magyarázat.

---

## 1. IDŐZÍTÉS (Szoftveres Timer)
* **Kód forrás:** `9. ora/sketch_nov5a`
* **Elméleti háttér:** `8. ora/Delay, Millis, Timer modul működésének áttekintése.pdf`

A `delay()` függvény használata a `loop()`-on belül (a mérés/vezérlés blokkolása miatt) **TILOS**! Helyette ezt a struktúrát használjuk:

```c
// Időzítő struktúra definíciója
typedef struct {
  unsigned long prevTime; // Utolsó lefutás ideje
  int deltaTime;          // Időzítés hossza (ms)
} Timer_s;

// Időzítő példányosítása (Példa: 1 másodperces timer)
Timer_s MyTimer = {
  .prevTime = 0,
  .deltaTime = 1000
};

// Időzítő ellenőrző függvény
// Visszatér 1-gyel, ha letelt az idő, és frissíti a prevTime-ot
unsigned char CheckTimer(Timer_s *timer) {
  unsigned long actTime = millis();
  if(actTime - timer->prevTime > timer->deltaTime) {
    timer->prevTime = actTime;
    return 1; // Eltelt az idő
  }
  return 0; // Még nem telt el
}
```

---

## 2. ÁLLAPOTGÉP (State Machine)
* **Kód forrás:** `9. ora/sketch_nov5a`
* **Elméleti háttér:** `9. ora/Finite_State_Machine.pdf`

Az állapotok kezelésére `enum` és `switch-case` szerkezetet használunk.

```c
// Állapotok felsorolása
typedef enum {
  STATE_IDLE,
  STATE_MEASURE,
  STATE_ALARM
} State_e;

// Kezdőállapot beállítása
State_e State = STATE_IDLE;


//Használat a loop()-ban:
switch (State) {
  case STATE_IDLE:
    // Cselekvés...
    // Átmenet feltétele...
    break;
}
```

---

## 3. SZŰRÉS: Adatgyűjtés és Rendezés
* **Kód forrás:** `10. ora/sketch_nov12a`
* **Elméleti háttér:** `10. ora/tavolsagmero.pdf` (Zajszűrés és medián elv)

A szűréshez (legyen az átlag vagy medián) először el kell tárolnunk az adatokat egy tömbben.

```c
#define BUFFER_SIZE 20
int buffer[BUFFER_SIZE];      // Nyers adatok
int bufferCopy[BUFFER_SIZE];  // Másolat a rendezéshez

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
```

---

## 4. SZŰRÉS: Átlag és Medián számítás
* **Kód forrás:** `10. ora/sketch_nov12a`
* **Elméleti háttér:** `10. ora/tavolsagmero.pdf`

A matematikai műveletek a pufferen (vagy a rendezett másolaton).

```c
// Átlag számítása (Mozgóátlag szűrőhöz)
int makeAvg() {
  long sum = 0;
  for(byte i = 0; i < BUFFER_SIZE; i++) {
    sum += buffer[i]; // Vagy bufferCopy, mindegy összegnél
  }
  return sum / BUFFER_SIZE;
}

// Medián lekérése (Rendezés után! - Medián szűrőhöz)
int getMedian() {
  // A rendezett tömb középső eleme
  return bufferCopy[BUFFER_SIZE/2];
}
```
