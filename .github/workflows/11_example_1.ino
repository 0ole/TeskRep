#include <Servo.h>

// 핀 설정
#define PIN_LED   9
#define PIN_TRIG  12
#define PIN_ECHO  13
#define PIN_SERVO 10

// 초음파 센서 설정
#define SND_VEL 346.0
#define INTERVAL 25
#define PULSE_DURATION 10
#define _DIST_MIN 180.0
#define _DIST_MAX 360.0

#define TIMEOUT ((INTERVAL / 2) * 1000.0)
#define SCALE (0.001 * 0.5 * SND_VEL)

#define _EMA_ALPHA 0.3

#define _TARGET_LOW  250.0
#define _TARGET_HIGH 290.0

#define _DUTY_MIN 500
#define _DUTY_NEU 1500
#define _DUTY_MAX 2500

float dist_ema, dist_prev = _DIST_MAX;
unsigned long last_sampling_time;

Servo myservo;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);

  myservo.attach(PIN_SERVO);
  myservo.writeMicroseconds(_DUTY_NEU);

  dist_prev = _DIST_MIN;

  Serial.begin(57600);
}

void loop() {
  float dist_raw, dist_filtered;

  if (millis() < last_sampling_time + INTERVAL)
    return;

  dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);

  if ((dist_raw == 0.0) || (dist_raw > _DIST_MAX)) {
    dist_filtered = dist_prev;
  } else if (dist_raw < _DIST_MIN) {
    dist_filtered = dist_prev;
  } else {
    dist_filtered = dist_raw;
    dist_prev = dist_raw;
  }

  dist_ema = _EMA_ALPHA * dist_filtered + (1 - _EMA_ALPHA) * dist_ema;

  if ((dist_ema == 0.0) || (dist_ema > _DIST_MAX)) {
    dist_ema = dist_prev;

    
  } else if (dist_ema < _DIST_MIN) {
    dist_ema = dist_prev;

  } else {
    dist_filtered = dist_ema;
    dist_prev = dist_ema;

  }

if ((dist_raw >= _DIST_MIN) && (dist_raw <= _DIST_MAX)) {
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);  
} else {
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH);  
}

int angle = map(dist_ema,_DIST_MIN,_DIST_MAX,_DUTY_MIN,_DUTY_MAX);
myservo.writeMicroseconds(angle);

 Serial.print("Min:");    Serial.print(_DIST_MIN);
 Serial.print(",dist:");  Serial.print(dist_raw);
 Serial.print(",ema:");   Serial.print(dist_ema);
 Serial.print(",Servo:"); Serial.print(myservo.read());
 Serial.print(",Max:");   Serial.print(_DIST_MAX);

 Serial.println("");

  last_sampling_time += INTERVAL;
}

// 초음파 거리 측정 함수
float USS_measure(int TRIG, int ECHO) {
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE;
}
