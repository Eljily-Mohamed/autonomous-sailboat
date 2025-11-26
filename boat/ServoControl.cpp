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
    
    // Améliorer la sensibilité pour les petites variations
    int sailDiff = abs(sailPWMus - lastSailPWM);
    int rudderDiff = abs(rudderPWMus - lastRudderPWM);
    
    // Réduire le seuil pour plus de sensibilité (< 5µs au lieu de 10µs)
    if (sailDiff > 0 && sailDiff < 5) {
        // Amplifier les très petites variations
        sailPWMus = lastSailPWM + (sailPWMus > lastSailPWM ? 8 : -8);
    }
    
    if (rudderDiff > 0 && rudderDiff < 5) {
        // Amplifier les très petites variations  
        rudderPWMus = lastRudderPWM + (rudderPWMus > lastRudderPWM ? 8 : -8);
    }
    
    // MAPPING PRÉCIS POUR TÉLÉCOMMANDE 5.68%-8.67% DUTY CYCLE
    // Télécommande: 5.68% = 1136µs, 8.67% = 1734µs (598µs de variation)
    // Mapper vers une plage servo optimisée pour contrôle précis
    
    // Mapper 1136-1734µs (entrée mesurée) vers 1450-1550µs (sortie très limitée)
    // Plage ultra-réduite pour éviter les tours multiples - seulement 100µs de variation
    sailPWMus = map(sailPWMus, 1136, 1734, 1450, 1550);
    rudderPWMus = map(rudderPWMus, 1136, 1734, 1450, 1550);
    
    // Zone morte au centre pour stabilité (réduite pour la plage limitée)
    if (abs(sailPWMus - 1500) < 10) sailPWMus = 1500;
    if (abs(rudderPWMus - 1500) < 10) rudderPWMus = 1500;
    
    // Contraindre dans la plage ultra-réduite pour éviter tours multiples
    sailPWMus = constrain(sailPWMus, 1450, 1550);
    rudderPWMus = constrain(rudderPWMus, 1450, 1550);
    
    // COPIE LIMITÉE : Envoyer microsecondes dans la plage réduite
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

