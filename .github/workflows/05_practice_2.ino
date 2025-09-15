#define LED_PIN 7  // GPIO 7번 핀에 LED 연결

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {

  digitalWrite(LED_PIN, LOW);
  delay(1000);


  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_PIN, HIGH);   
    delay(100);                   
    digitalWrite(LED_PIN, LOW);  
    delay(100);                   
  }

  
  digitalWrite(LED_PIN, HIGH);


  while (1) {
  }
}

