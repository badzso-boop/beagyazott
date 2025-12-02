#define LED1 2
#define LED2 3

unsigned long led1PrevTime = 0;
int led1DeltaT = 1000; //ms
unsigned long led2PrevTime = 0;
int led2DeltaT = 1000; //ms

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);

  led1PrevTime = millis();
  led2PrevTime = millis();

  Serial.begin(9600);
}

int start = 1;
void loop() {
  unsigned long actTime = millis();
  if(actTime - led1PrevTime > led1DeltaT) {
    digitalWrite(LED1, 1-digitalRead(LED1));
    led1PrevTime = led1PrevTime + led1DeltaT;
    Serial.println("LED1");
  }

  if(actTime - led2PrevTime > led2DeltaT) {
    digitalWrite(LED2, 1-digitalRead(LED2));
    led2PrevTime = actTime;
    Serial.println("LED2");
  }

  if(start == 1) {
    start++;
    delay(5000);
  }
}
