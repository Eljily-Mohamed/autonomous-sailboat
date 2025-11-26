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
    // LECTURE PRÉCISE : Moyenne de 3 mesures pour stabilité
    unsigned long totalPulse = 0;
    int validReadings = 0;
    
    for (int i = 0; i < 3; i++) {
        unsigned long pulseWidth = pulseIn(pin, HIGH, 25000);
        if (pulseWidth > 0) {
            totalPulse += pulseWidth;
            validReadings++;
        }
    }
    
    if (validReadings == 0) {
        return 1500; // Valeur par défaut
    }
    
    unsigned long avgPulse = totalPulse / validReadings;
    
    // COPIE EXACTE : Pas de contrainte pour garder le signal original
    return avgPulse;
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

