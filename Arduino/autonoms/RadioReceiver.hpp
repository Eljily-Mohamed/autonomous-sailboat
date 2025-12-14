#ifndef RADIO_RECEIVER_HPP
#define RADIO_RECEIVER_HPP

#include "config_pins.h"
#include <Arduino.h>

/**
 * Module de gestion du récepteur radio Pro-Tronik 2.4GHz
 * Gère la lecture des signaux PWM et le signal SEL
 */

class RadioReceiver {
public:
    /**
     * Initialise le récepteur radio
     */
    void init();
    
    /**
     * Lit et met à jour les valeurs PWM depuis le récepteur
     * Doit être appelé régulièrement dans la boucle principale
     */
    void update();
    
    /**
     * Lit un signal PWM depuis un pin spécifique
     * @param pin Pin GPIO à lire
     * @return Largeur d'impulsion en microsecondes (1000-2000)
     */
    int readPWM(int pin);
    
    /**
     * Vérifie si le mode radiocommande est actif
     * @return true si SEL=1 (mode radiocommande), false si SEL=0 (mode autonome)
     */
    bool isRadioControlMode() const { return radioControlMode; }
    
    /**
     * Vérifie si le mode a changé depuis le dernier appel
     * @return true si le mode a changé
     */
    bool hasModeChanged() const { return modeChanged; }
    
    /**
     * Réinitialise le flag de changement de mode
     */
    void resetModeChanged() { modeChanged = false; }
    
    /**
     * Récupère la valeur PWM1 (Aileron)
     * @return Valeur PWM en microsecondes
     */
    int getPWM1() const { return pwm1Value; }
    
    /**
     * Récupère la valeur PWM2 (Safran)
     * @return Valeur PWM en microsecondes
     */
    int getPWM2() const { return pwm2Value; }
    
    /**
     * Convertit une valeur PWM (µs) en angle servo (0-180°)
     * @param pwmValue Valeur PWM en microsecondes
     * @return Angle en degrés (0-180)
     */
    int pwmToAngle(int pwmValue) const;

private:
    bool radioControlMode = false;  // Mode actuel (true=radio, false=autonome)
    bool modeChanged = false;        // Flag indiquant si le mode a changé
    unsigned long lastPWMRead = 0;   // Dernière lecture PWM
    int pwm1Value = 1500;            // Valeur PWM1 (Aileron)
    int pwm2Value = 1500;            // Valeur PWM2 (Safran)
    
    /**
     * Vérifie le signal SEL et met à jour le mode
     */
    void checkMode();
};

#endif // RADIO_RECEIVER_HPP

