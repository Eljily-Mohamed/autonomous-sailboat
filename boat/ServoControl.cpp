#include "ServoControl.hpp"

void ServoControl::init() {
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

void ServoControl::setAnglesDirect(int sailPWMus, int rudderPWMus) {
    // COPIE SIGNAL EXACTE : Utiliser writeMicroseconds pour précision maximale
    
    // Ajouter offset pour améliorer la sensibilité aux petites variations
    static int lastSailPWM = 1500;
    static int lastRudderPWM = 1500;
    
    // Appliquer un offset si la variation est très petite (< 10µs)
    int sailDiff = abs(sailPWMus - lastSailPWM);
    int rudderDiff = abs(rudderPWMus - lastRudderPWM);
    
    if (sailDiff > 0 && sailDiff < 10) {
        // Amplifier les petites variations
        sailPWMus = lastSailPWM + (sailPWMus > lastSailPWM ? 15 : -15);
    }
    
    if (rudderDiff > 0 && rudderDiff < 10) {
        // Amplifier les petites variations
        rudderPWMus = lastRudderPWM + (rudderPWMus > lastRudderPWM ? 15 : -15);
    }
    
    // Contraindre dans les limites servo
    sailPWMus = constrain(sailPWMus, SERVO_PULSE_MIN, SERVO_PULSE_MAX);
    rudderPWMus = constrain(rudderPWMus, SERVO_PULSE_MIN, SERVO_PULSE_MAX);
    
    // COPIE DIRECTE : Envoyer microsecondes exactes
    sailServo.writeMicroseconds(sailPWMus);
    rudderServo.writeMicroseconds(rudderPWMus);
    
    // Sauvegarder pour la prochaine fois
    lastSailPWM = sailPWMus;
    lastRudderPWM = rudderPWMus;
    
    // Mettre à jour les variables internes
    this->sailAngle = map(sailPWMus, SERVO_PULSE_MIN, SERVO_PULSE_MAX, -90, 90);
    this->rudderAngle = map(rudderPWMus, SERVO_PULSE_MIN, SERVO_PULSE_MAX, -90, 90);
}

float ServoControl::constrainSailAngle(float angle) {
    return constrain(angle, SERVO_SAIL_ANGLE_MIN, SERVO_SAIL_ANGLE_MAX);
}

float ServoControl::constrainRudderAngle(float angle) {
    return constrain(angle, SERVO_RUDDER_ANGLE_MIN, SERVO_RUDDER_ANGLE_MAX);
}

