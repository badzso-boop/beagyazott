const double R = 10000; // 10k ohm reference
const double adc_resolution = 1023; // 10 bit adc

//thermistor equation parameters according to table
const double A = 3.354016e-3;
const double B = 2.569355e-4;
const double C = 2.626311e-6;
const double D = 0.675278e-7;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  double Rt, temperature, adc_value;
  adc_value = analogRead(A1);
  Rt = ((adc_value * R) / (adc_resolution - adc_value));

  // Steinhart-Hart thermistor equation
  // temperature in kelvin
  double logRt = log(Rt/R);
  temperature = (1 / (A + (B * logRt) 
                + (C * pow(logRt, 2)) 
                + ( D * pow(logRt, 3))));

  //temperature in celsius
  temperature = temperature - 273.15;
  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.println(" degree celsius");
  delay(1000);
}
