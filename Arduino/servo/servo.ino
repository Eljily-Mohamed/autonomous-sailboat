/*
 IMPORTANT NOTE :
 HS-322-HD only goes from 0° to 180°

 PWM Frequency and pluse length are from SG90, seems to be working
*/

#include <ESP32Servo.h>

#define SERVO_PIN 2

Servo hs322hd;


void setup() {
  Serial.begin(115200);

  // Setup servo
  hs322hd.setPeriodHertz(50);             // PWM control frequency
  hs322hd.attach(SERVO_PIN, 500, 2400);   // Min and max pulse (in µs)
}


int angle = 0;
void loop() {
  angle++;
  angle = angle % 180;
  Serial.println(angle);
  hs322hd.write(angle);
  delay(50);
}
