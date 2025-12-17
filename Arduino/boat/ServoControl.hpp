#ifndef SERVO_CONTROL_HPP
#define SERVO_CONTROL_HPP

#include <ESP32Servo.h>
#include "config_pins.h"

/**
 * Module de contrôle des servos-moteurs
 * Gère les servos Sail (Aileron) et Rudder (Safran)
 */

class ServoControl {
public:
    /**
     * Initialise les servos
     */
    void init();
    
    /**
     * Définit l'angle du servo Sail (Aileron)
     * @param angle Angle en degrés (-10° à +10°)
     */
    void setSailAngle(float angle);
    
    /**
     * Définit l'angle du servo Rudder (Safran)
     * @param angle Angle en degrés (-45° à +45°)
     */
    void setRudderAngle(float angle);
    
    /**
     * Définit directement les angles PWM (pour mode radiocommande)
     * @param sailAngle Angle Sail (0-180°)
     * @param rudderAngle Angle Rudder (0-180°)
     */
    void setAnglesDirect(int sailAngle, int rudderAngle);
    
    /**
     * Récupère l'angle actuel du Sail
     * @return Angle en degrés
     */
    float getSailAngle() const { return sailAngle; }
    
    /**
     * Récupère l'angle actuel du Rudder
     * @return Angle en degrés
     */
    float getRudderAngle() const { return rudderAngle; }

private:
    Servo sailServo;
    Servo rudderServo;
    
    float sailAngle = -10;   // Angle actuel Sail
    float rudderAngle = 20;   // Angle actuel Rudder
    
    /**
     * Contraint l'angle Sail dans les limites de sécurité
     */
    float constrainSailAngle(float angle);
    
    /**
     * Contraint l'angle Rudder dans les limites de sécurité
     */
    float constrainRudderAngle(float angle);
};

#endif // SERVO_CONTROL_HPP

