#ifndef MOTOR_CONTROL_HPP
#define MOTOR_CONTROL_HPP

#include <ESP32Servo.h>
#include "config_pins.h"

/**
 * Module de contrôle du moteur/ESC
 * Gère l'armement et le contrôle de vitesse du moteur
 */

class MotorControl {
public:
    /**
     * Initialise le contrôleur moteur
     */
    void init();
    
    /**
     * Arme l'ESC (séquence d'armement)
     */
    void arm();
    
    /**
     * Désarme l'ESC
     */
    void disarm();
    
    /**
     * Définit la vitesse du moteur
     * @param speed Vitesse en microsecondes (1000-2000) ou en pourcentage (0-100)
     * @param usePercent Si true, speed est en pourcentage, sinon en microsecondes
     */
    void setSpeed(int speed, bool usePercent = false);
    
    /**
     * Récupère la vitesse actuelle
     * @return Vitesse en microsecondes
     */
    int getSpeed() const { return currentSpeed; }
    
    /**
     * Vérifie si le moteur est armé
     * @return true si armé, false sinon
     */
    bool isArmed() const { return isArmedFlag; }

private:
    Servo escServo;
    int currentSpeed = ESC_PULSE_MID;  // Vitesse actuelle (µs)
    bool isArmedFlag = false;          // État d'armement
    
    /**
     * Convertit un pourcentage en valeur PWM
     */
    int percentToPWM(int percent) const;
};

#endif // MOTOR_CONTROL_HPP

