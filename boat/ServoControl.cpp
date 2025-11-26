#include "ServoControl.hpp"

void ServoControl::init() {
    // Configuration des pins en drain ouvert (open-drain)
    pinMode(SERVO_SAIL_PIN, OUTPUT_OPEN_DRAIN);
    pinMode(SERVO_RUDDER_PIN, OUTPUT_OPEN_DRAIN);
    
    // Configuration PWM
    sailServo.setPeriodHertz(SERVO_PWM_FREQ);
    rudderServo.setPeriodHertz(SERVO_PWM_FREQ);
    
    // Attacher les servos avec limites de pulse
    sailServo.attach(SERVO_SAIL_PIN, SERVO_PULSE_MIN, SERVO_PULSE_MAX);
    rudderServo.attach(SERVO_RUDDER_PIN, SERVO_PULSE_MIN, SERVO_PULSE_MAX);
    
    // Position initiale
    setSailAngle(sailAngle);
    delay(500);
    setRudderAngle(rudderAngle);
    delay(500);
}

void ServoControl::setSailAngle(float angle) {
    angle = constrainSailAngle(angle);
    sailAngle = angle;
    
    // Convertir angle (-10° à +10°) en valeur servo (0-180°)
    angle += 90;  // Décalage pour centrer à 90°
    sailServo.write(constrain((int)angle, 0, 180));
}

void ServoControl::setRudderAngle(float angle) {
    angle = constrainRudderAngle(angle);
    rudderAngle = angle;
    
    // Convertir angle (-45° à +45°) en valeur servo (0-180°)
    angle += 90;  // Décalage pour centrer à 90°
    rudderServo.write(constrain((int)angle, 0, 180));
}

void ServoControl::setAnglesDirect(int sailAngle, int rudderAngle) {
    // Pour mode radiocommande - valeurs directes 0-180°
    sailServo.write(constrain(sailAngle, 0, 180));
    rudderServo.write(constrain(rudderAngle, 0, 180));
    
    // Mettre à jour les variables internes pour cohérence
    this->sailAngle = sailAngle - 90;
    this->rudderAngle = rudderAngle - 90;
}

float ServoControl::constrainSailAngle(float angle) {
    return constrain(angle, SERVO_SAIL_ANGLE_MIN, SERVO_SAIL_ANGLE_MAX);
}

float ServoControl::constrainRudderAngle(float angle) {
    return constrain(angle, SERVO_RUDDER_ANGLE_MIN, SERVO_RUDDER_ANGLE_MAX);
}

