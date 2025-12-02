# 📚 Beágyazott Rendszerek LA - Félévi Összefoglaló

Ez a repozitórium a **Beágyazott Rendszerek LA** tárgy gyakorlati anyagait, kódjait és jegyzeteit tartalmazza a teljes félévre vonatkozóan. A dokumentum célja, hogy segítse a felkészülést és a gyors navigációt a fájlok között.

> **Megjegyzés:** A fájlok linkjei a GitHub felületén és helyi környezetben is működnek.

---

## 📂 Tartalomjegyzék (Gyorstár)

### 📝 Összefoglaló Anyagok (Cheat Sheets)
A gyökérkönyvtárban található legfontosabb puskák:
- **[Teljes Félévi Összefoglaló (MD)](Teljes_Felevi_Osszefoglalo.md)** - A legfontosabb kódok (Timer, UART, Állapotgép, Szűrők) egy helyen.
- **[Órai Anyag Összefoglaló (MD)](Orai_Anyag_Osszefoglalo.md)** - Rövidebb, témakörökre bontott puska.
- **[Hogyan működik a... (MD)](hogy_mukodik_a.md)** - Részletes magyarázat a Timer struktúráról, állapotgépről és szűrésről.

---

## 📅 Órai Anyagok Részletesen

### [1. Óra - Bevezetés](1.%20ora/)
*Alapfogalmak és a tárgy bemutatása.*
- 📄 [01_Bevezetes_V3.pdf](1.%20ora/01_Bevezetes_V3.pdf)

### [2. Óra - C Nyelv Alapjai](2.%20ora/)
*Operátorok és alapvető szintaxis.*
- 📄 [Előadás jegyzet - Operátorok - kóddal.pdf](2.%20ora/Előadás%20jegyzet%20-%20Operátorok%20-%20kóddal.pdf)

### [3. Óra - Alacsony Szintű IO (GPIO)](3.%20ora/)
*Regiszter szintű bitműveletek (DDRB, PORTB).*
- 📄 [Előadás jegyzet - GPIO - kóddal.pdf](3.%20ora/Előadás%20jegyzet%20-%20GPIO%20-%20kóddal.pdf)
- 📄 [GPIO jegyzet.pdf](3.%20ora/GPIO%20jegyzet.pdf)
- 💻 **Kód:** [la3.ino](3.%20ora/la3/la3.ino)
    - `DDRB`, `PORTB` használata `digitalWrite` helyett.
    - Bitmaszkolás: `|= (1<<PB7)` (beállítás), `&= ~(1<<PB7)` (törlés), `^=` (toggle).

### [5. Óra - UART (Polling/Blocking)](5.%20ora/)
*Soros kommunikáció regiszter szinten, várakozással.*
- 📄 [02_UART_kommunikacio.pdf](5.%20ora/02_UART_kommunikacio.pdf)
- 💻 **Kód:** [sketch_oct8a.ino](5.%20ora/sketch_oct8a/sketch_oct8a.ino)
    - `UBRR0L`, `UBRR0H` beállítása (Baud rate).
    - `UCSR0B`, `UCSR0C` konfigurálása (Aszinkron, 8N1).
    - Küldés/Fogadás `while` ciklusos várakozással (`UDRE0`, `RXC0` flag-ek).

### [6. Óra - Hardveres Timer Interrupt](6.%20ora/)
*Időzítés hardveres megszakítással (Timer5).*
- 📄 [Delay, Millis, Timer modul működésének áttekintése.pdf](6.%20ora/Delay,%20Millis,%20Timer%20modul%20működésének%20áttekintése.pdf)
- 💻 **Kód:** [sketch_oct8a.ino](6.%20ora/sketch_oct8a/sketch_oct8a.ino)
    - `TCCR5A`, `TCCR5B` beállítása (Prescaler).
    - `TIMSK5` megszakítás engedélyezése (`TOIE5`).
    - `ISR(TIMER5_OVF_vect)` használata.

### [7. Óra - Szoftveres Időzítés (Millis)](7.%20ora/)
*Delay nélküli időzítés `millis()` függvény használatával.*
- 💻 **Kód:** [sketch_oct22a.ino](7.%20ora/sketch_oct22a/sketch_oct22a.ino)
    - `prevTime`, `actTime`, `deltaTime` változók használata.
    - Több LED független villogtatása blokkolás nélkül.

### [8. Óra - UART Interrupt & Körbuffer](8.%20ora/)
*Fejlett soros kommunikáció megszakítással és puffereléssel.*
- 📄 [UART jegyzet.pdf](8.%20ora/UART%20jegyzet.pdf)
- 📄 [UARTring buffer.pdf](8.%20ora/UARTring%20buffer.pdf)
- 💻 **Kód:** [sketch_oct29a.ino](8.%20ora/sketch_oct29a/sketch_oct29a.ino)
    - `ISR(USART0_RX_vect)` vételi megszakítás.
    - `ring_buffer`, `write_index`, `read_index` implementáció (FIFO).
    - Nem blokkoló adatfogadás.

### [9. Óra - Állapotgép (State Machine) & Timer Struct](9.%20ora/)
*Struktúrált kódolás és vezérlés.*
- 📄 [Finite_State_Machine.pdf](9.%20ora/Finite_State_Machine.pdf)
- 💻 **Kód:** [sketch_nov5a.ino](9.%20ora/sketch_nov5a/sketch_nov5a.ino)
    - `typedef struct Timer_s` bevezetése az időzítők kezelésére.
    - `typedef enum State_e` állapotok definiálása.
    - `switch(State)` alapú vezérlés a `loop()`-ban.

### [10. Óra - Szenzorok és Szűrés](10.%20ora/)
*Ultrahangos távolságmérés, Átlagoló és Medián szűrő.*
- 📄 [tavolsagmero.pdf](10.%20ora/tavolsagmero.pdf)
- 💻 **Kód:** [sketch_nov12a.ino](10.%20ora/sketch_nov12a/sketch_nov12a.ino)
    - `HC-SR04` kezelése (`pulseIn`).
    - Csúszóablak (FIFO) puffer megvalósítása (`addSimple`).
    - Buborékrendezés (`rendez`) a medián kereséshez.
    - Átlag és Medián számítás.

### [12. Óra - Analóg Szenzorok (Termisztor)](12.%20ora/)
*NTC termisztor és Steinhart-Hart egyenlet.*
- 📄 [Termisztor, fotóellenállás.pdf](12.%20ora/Termisztor,%20fotóellenállás.pdf)
- 💻 **Kód:** [sketch_nov26a.ino](12.%20ora/sketch_nov26a/sketch_nov26a.ino)
    - `analogRead()` használata.
    - Feszültségosztó számítása.
    - Logaritmikus hőmérséklet számítás (Kelvin -> Celsius).

---

## 🧪 Kiegészítések és Vizsgaanyagok

### Kiegészítés
- 📄 [PWM, ledvezérlés.pdf](kiegeszites/PWM,%20ledvezérlés.pdf)

### Minta ZH Megoldások
Gyakorló feladatok kidolgozott megoldásai:
- 💻 **"A" csoport:** [MintaZH_A_Megoldas.ino](MintaZH_A_Megoldas/MintaZH_A_Megoldas.ino)
- 💻 **"B" csoport:** [MintaZH_B_Megoldas.ino](MintaZH_B_Megoldas/MintaZH_B_Megoldas.ino)

---
*Készült a félévi jegyzetek és kódok alapján.*
