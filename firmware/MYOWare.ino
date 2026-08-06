const int emgPin = A0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  int valor = analogRead(emgPin);

  Serial.println(valor);

  delay(50);
}