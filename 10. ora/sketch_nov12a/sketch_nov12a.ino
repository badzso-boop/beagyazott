#define buffetSize 20
int buffer[buffetSize];
int bufferCopy[buffetSize];

const int trigPin = 9;
const int echoPin = 10;

long duration;
int distance;
int tmp;
int result;

//tombhoz hozzaadjuk a szamot
void addSimple(int simple) {
  for(byte i = 0; i < buffetSize-1; i++) {
    buffer[i] = buffer[i+1];
  }
  buffer[buffetSize-1] = simple;
}


//atlagot szamol a tombbol
int makeAvg() {
  long sum = 0;
  for(byte i = 0; i < buffetSize; i++) {
    sum += buffer[i];
  }

  return sum / buffetSize;
}

//rendezi a tombot valamilyen sorrendbe
void rendez() {
  for(int j = 0; j < buffetSize; j++) {
    bufferCopy[j] = buffer[j];
  }

  for(int i = 0; i < buffetSize; i++) {
    for(int j = i; j < buffetSize; j++) {
      if(bufferCopy[i] > bufferCopy[j]) {
        tmp = bufferCopy[i];
        bufferCopy[i] = bufferCopy[j];
        bufferCopy[j] = tmp;
      }
    }
  }
}


//lekeri a rendezett tombnek a medianjat
int getMedian() {
  return bufferCopy[buffetSize/2];
}

//lekeri a tavolsagot es a switch caseval rak bele zavarast 0 es 400cm-t
int getDistance() {
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 25000);

  distance = duration*0.034/2;

  switch(random(0,6)) {
    case 1: distance = 400; break;
    case 0: distance = 0; break;
  }

  return distance;
}


void setup() {
  // put your setup code here, to run once:
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  //lekeri a tavolsagot, hozzaadja a tombhoz, rendezi, lekeri a mediant, majd megjeleniti ket grafikonban
  result = getDistance();
  addSimple(distance);
  rendez();
  int median = getMedian();

  Serial.print("Distance:");
  Serial.print(result);
  Serial.print(",");
  Serial.print("Median:");
  Serial.println(median);
}


