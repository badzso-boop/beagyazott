#define GLED 3
#define YLED 4
#define RLED 5

typedef struct {
  unsigned long prevTime;
  int deltaTime;
} Timer_s;

Timer_s BYTimer {
  .prevTime = 0,
  .deltaTime = 500
};

Timer_s RStateTimer {
  .prevTime = 0,
  .deltaTime = 3000
};

Timer_s BYStateTimer {
  .prevTime = 0,
  .deltaTime = 4000
};


typedef enum {
  RSTATE, //red state
  RYSTATE, // red yellow state
  YSTATE, // yellow state
  GSTATE, // green state
  BYSTATE  //blink yellow state
} State_e;

State_e State = BYSTATE;

unsigned char CheckTimer(Timer_s *timer);

void setup() {
  pinMode(GLED, OUTPUT);
  pinMode(YLED, OUTPUT);
  pinMode(RLED, OUTPUT);

  digitalWrite(GLED, LOW);
  digitalWrite(YLED, LOW);
  digitalWrite(RLED, LOW);
}

void loop() {
  switch (State) {
    case RSTATE:
      if(CheckTimer(&RStateTimer)) {
        State = BYSTATE;
        BYStateTimer.prevTime = millis();
        digitalWrite(RLED, LOW);
      } 
      break;

    case RYSTATE:
      break;

    case YSTATE:
      break;

    case GSTATE:
      break;

    case BYSTATE:
      if (CheckTimer(&BYTimer)) {
        digitalWrite(YLED, 1-digitalRead(YLED));
      }
      if (CheckTimer(&BYStateTimer)) {
        State = RSTATE;
        // BYSTATE exit
        digitalWrite(YLED, LOW); // sárga kikapcsol
        // RSTATE entry
        digitalWrite(RLED, HIGH);
        // következpo allapot timer prev ertekenek beallitasa
        RStateTimer.prevTime = millis();
      }
      break; 
  }
}

unsigned char CheckTimer(Timer_s *timer) {
  unsigned long actTime = millis();
  if(actTime - timer->prevTime > timer->deltaTime) {
    timer->prevTime = actTime;
    return 1; //eltelt
  }
  return 0; //nem telt el
}
