#include "MotorControl.hpp"

void MotorControl::init() {
    // Configuration PWM pour ESC
    escServo.setPeriodHertz(ESC_PWM_FREQ);
    escServo.attach(MOTOR_ESC_PIN, ESC_PULSE_MIN, ESC_PULSE_MAX);
    
    // Position initiale (arrêt)
    escServo.writeMicroseconds(ESC_PULSE_MID);
    currentSpeed = ESC_PULSE_MID;
    isArmedFlag = false;
}

void MotorControl::arm() {
    if (isArmedFlag) {
        return;  // Déjà armé
    }
    
    // Séquence d'armement ESC standard:
    // 1. Envoyer signal minimum (1000µs)
    escServo.writeMicroseconds(ESC_PULSE_MIN);
    delay(1000);
    
    // 2. Envoyer signal milieu (1500µs) pour armer
    escServo.writeMicroseconds(ESC_PULSE_MID);
    delay(500);
    
    isArmedFlag = true;
    currentSpeed = ESC_PULSE_MID;
}

void MotorControl::disarm() {
    if (!isArmedFlag) {
        return;  // Déjà désarmé
    }
    
    // Ramener à la position idle
    escServo.writeMicroseconds(ESC_PULSE_MID);
    delay(100);
    
    isArmedFlag = false;
    currentSpeed = ESC_PULSE_MID;
}

void MotorControl::setSpeed(int speed, bool usePercent) {
    if (!isArmedFlag) {
        // Ne pas permettre de changement de vitesse si non armé
        return;
    }
    
    int pwmValue;
    
    if (usePercent) {
        // Convertir pourcentage en PWM
        pwmValue = percentToPWM(speed);
    } else {
        // Utiliser directement la valeur en microsecondes
        pwmValue = constrain(speed, ESC_PULSE_MIN, ESC_PULSE_MAX);
    }
    
    escServo.writeMicroseconds(pwmValue);
    currentSpeed = pwmValue;
}

int MotorControl::percentToPWM(int percent) const {
    // Convertit un pourcentage (0-100) en valeur PWM (1000-2000 µs)
    percent = constrain(percent, 0, 100);
    return map(percent, 0, 100, ESC_PULSE_MIN, ESC_PULSE_MAX);
}

