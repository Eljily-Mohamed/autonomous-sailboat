#include "RadioReceiver.hpp"

void RadioReceiver::init() {
    // Configuration des pins en entrée
    pinMode(RADIO_PWM1_IN, INPUT);
    pinMode(RADIO_PWM2_IN, INPUT);
    pinMode(RADIO_SEL_IN, INPUT);
    
    // Initialisation du mode
    radioControlMode = digitalRead(RADIO_SEL_IN);
}

void RadioReceiver::update() {
    // Vérifier le mode SEL
    checkMode();
    
    // Lire les signaux PWM à intervalle régulier
    if (millis() - lastPWMRead >= RADIO_PWM_READ_INTERVAL) {
        pwm1Value = readPWM(RADIO_PWM1_IN);
        pwm2Value = readPWM(RADIO_PWM2_IN);
        lastPWMRead = millis();
    }
}

int RadioReceiver::readPWM(int pin) {
    // Lit la largeur d'impulsion en microsecondes
    // Timeout de 25ms (supérieur à la période PWM de 20ms)
    unsigned long pulseWidth = pulseIn(pin, HIGH, 25000);
    
    if (pulseWidth == 0) {
        // Pas de signal, retourner valeur par défaut (milieu)
        return 1500;
    }
    
    // Limiter la plage entre 1000 et 2000 µs
    return constrain(pulseWidth, RADIO_PWM_MIN, RADIO_PWM_MAX);
}

void RadioReceiver::checkMode() {
    bool selSignal = digitalRead(RADIO_SEL_IN);
    
    // Si le mode change, mettre à jour
    if (selSignal != radioControlMode) {
        radioControlMode = selSignal;
        modeChanged = true;  // Marquer le changement
    }
}

int RadioReceiver::pwmToAngle(int pwmValue) const {
    // Convertit une valeur PWM (1000-2000 µs) en angle servo (0-180°)
    return map(pwmValue, RADIO_PWM_MIN, RADIO_PWM_MAX, 0, 180);
}

