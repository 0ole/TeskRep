// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12   // sonar sensor TRIGGER
#define PIN_ECHO 13   // sonar sensor ECHO

// configurable parameters
#define SND_VEL 346.0       // sound velocity at 24 celsius (m/sec)
#define INTERVAL 25         // sampling interval (msec)
#define PULSE_DURATION 10   // ultra-sound Pulse Duration (usec)
#define _DIST_MIN 100.0     // minimum distance to be measured (mm)
#define _DIST_MAX 300.0     // maximum distance to be measured (mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // max echo wait time (usec)
#define SCALE (0.001 * 0.5 * SND_VEL)     // convert microsec -> mm

unsigned long last_sampling_time = 0;   // msec

// 함수 원형 (세미콜론 필수)
float USS_measure(int TRIG, int ECHO);

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);  // ensure TRIG low
  Serial.begin(57600);
}

void loop() {
  // non-blocking timing (overflow-safe)
  if (millis() - last_sampling_time < INTERVAL) return;
  last_sampling_time = millis();

  float distance = USS_measure(PIN_TRIG, PIN_ECHO); // mm
  int brightness = 255; // default OFF (active-low: 0 brightest, 255 off)

  // handle out-of-range or measurement failure
  if (distance == 0.0 || distance < _DIST_MIN || distance > _DIST_MAX) {
    brightness = 255; // OFF
  } else {
    // Triangular mapping:
    // 100mm -> 255 (off), 200mm -> 0 (brightest), 300mm -> 255 (off)
    if (distance <= 200.0) {
      // map [100 .. 200] -> [255 .. 0]
      float ratio = (distance - _DIST_MIN) / (200.0 - _DIST_MIN); // 0..1
      brightness = (int)round(255.0 * (1.0 - ratio));
    } else {
      // map [200 .. 300] -> [0 .. 255]
      float ratio = (distance - 200.0) / (_DIST_MAX - 200.0); // 0..1
      brightness = (int)round(255.0 * ratio);
    }
    // 안전 범위 제한
    if (brightness < 0) brightness = 0;
    if (brightness > 255) brightness = 255;
  }

  // PWM 출력 (pin 9은 UNO에서 PWM 가능)
  analogWrite(PIN_LED, brightness);

  // 출력 (디버그)
  Serial.print("Min:");        Serial.print(_DIST_MIN);
  Serial.print(", distance:"); Serial.print(distance);
  Serial.print(", brightness:"); Serial.print(brightness);
  Serial.print(", Max:");      Serial.print(_DIST_MAX);
  Serial.println("");
}

// 초음파 거리 측정 (리턴: mm)
float USS_measure(int TRIG, int ECHO) {
  // 안정화를 위해 TRIG을 잠깐 LOW로 유지
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  // 트리거 펄스
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);

  // duration (usec). TIMEOUT을 unsigned long로 캐스팅
  unsigned long dur = pulseIn(ECHO, HIGH, (unsigned long)TIMEOUT);

  if (dur == 0) {
    // timeout or no echo
    return 0.0;
  }

  // dur * SCALE -> mm
  return (float)dur * (float)SCALE;
}
