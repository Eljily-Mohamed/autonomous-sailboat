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
    
    // AMÉLIORER LA PRÉCISION : Filtrer les valeurs aberrantes
    // Si la valeur est très éloignée de la plage attendue, utiliser la dernière valeur valide
    static unsigned long lastValidSail = 1435;  // Centre de votre télécommande
    static unsigned long lastValidRudder = 1435;
    
    // Vérifier si la valeur est dans une plage raisonnable (1100-1800µs pour votre télécommande)
    if (avgPulse >= 1100 && avgPulse <= 1800) {
        if (pin == RADIO_PWM1_IN) lastValidSail = avgPulse;
        if (pin == RADIO_PWM2_IN) lastValidRudder = avgPulse;
        return avgPulse;
    } else {
        // Valeur aberrante, retourner la dernière valeur valide
        return (pin == RADIO_PWM1_IN) ? lastValidSail : lastValidRudder;
    }
}

void RadioReceiver::checkMode() {
    // Lire le signal PWM sur le pin SEL
    unsigned long selPulseWidth = pulseIn(RADIO_SEL_IN, HIGH, 25000);
    
    if (selPulseWidth == 0) {
        // Pas de signal, garder le mode actuel
        return;
    }
    
    // Calculer le duty cycle : 6% = 1200µs sur période 20ms
    // Si duty > 6% (1200µs) → Mode manuel
    // Si duty < 6% (1200µs) → Mode autonome
    bool newMode = (selPulseWidth > 1200);
    
    // Si le mode change, mettre à jour
    if (newMode != radioControlMode) {
        radioControlMode = newMode;
        modeChanged = true;  // Marquer le changement
    }
}

int RadioReceiver::pwmToAngle(int pwmValue) const {
    // Convertit une valeur PWM (1000-2000 µs) en angle servo (0-180°)
    return map(pwmValue, RADIO_PWM_MIN, RADIO_PWM_MAX, 0, 180);
}

