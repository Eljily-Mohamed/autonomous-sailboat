#ifndef CONFIG_PINS_H
#define CONFIG_PINS_H

/**
 * Configuration Pin-Mapping - AutoBoat TTGO-T-Beam T22_v1.1
 * 
 * Ce fichier centralise toutes les définitions de pins du système.
 * Architecture sans multiplexeur - connexions directes.
 * 
 * Auteur: Mohamed EL JILY
 */

// ============================================================================
// CONNEXIONS GPS (UART Direct - Serial1)
// ============================================================================
#define GPS_RX_PIN 34        // GPIO 34 - RX (Serial1) - Input only
#define GPS_TX_PIN 12        // GPIO 12 - TX (Serial1) - Output
#define GPS_BAUD_RATE 9600   // Baud rate pour communication GPS

// ============================================================================
// CONNEXIONS SERVO-MOTEURS (PWM Output)
// ============================================================================
// Les signaux PWM passent par des amplificateurs 3.3V → 5V avant d'atteindre les servos
#define SERVO_SAIL_PIN 2     // GPIO 2 - PWM1 OUT - Aileron (Sail)
#define SERVO_RUDDER_PIN 25  // GPIO 25 - PWM2 OUT - Safran (Rudder)

// Configuration PWM Servos
#define SERVO_PWM_FREQ 50           // Fréquence PWM en Hz (standard servos)
#define SERVO_PULSE_MIN 500         // Pulse width minimum (µs)
#define SERVO_PULSE_MAX 2400        // Pulse width maximum (µs)
#define SERVO_SAIL_ANGLE_MIN -10    // Angle minimum Sail (degrés)
#define SERVO_SAIL_ANGLE_MAX 10     // Angle maximum Sail (degrés)
#define SERVO_RUDDER_ANGLE_MIN -45  // Angle minimum Rudder (degrés)
#define SERVO_RUDDER_ANGLE_MAX 45  // Angle maximum Rudder (degrés)

// ============================================================================
// CONNEXIONS RÉCEPTEUR RADIO (PWM Input)
// ============================================================================
// Récepteur Pro-Tronik 2.4GHz
#define RADIO_PWM1_IN 21     // GPIO 21 - PWM1 IN (CH4) - Aileron depuis radiocommande
#define RADIO_PWM2_IN 22     // GPIO 22 - PWM2 IN (CH2) - Safran depuis radiocommande
#define RADIO_SEL_IN 23      // GPIO 23 - SEL IN (CH6) - Sélection mode (0=autonome, 1=radiocommande)

// Configuration PWM Input
#define RADIO_PWM_MIN 1000   // Valeur PWM minimum (µs)
#define RADIO_PWM_MAX 2000  // Valeur PWM maximum (µs)
#define RADIO_PWM_READ_INTERVAL 50  // Intervalle de lecture PWM (ms)

// ============================================================================
// CONNEXIONS MOTEUR/ESC (PWM Output)
// ============================================================================
// À définir selon la configuration matérielle
#define MOTOR_ESC_PIN 4      // GPIO 4 - PWM Motor/ESC (à vérifier selon câblage)

// Configuration ESC
#define ESC_PWM_FREQ 50           // Fréquence PWM en Hz (standard ESC)
#define ESC_PULSE_MIN 1000       // Pulse width minimum (µs) - Arrêt
#define ESC_PULSE_MID 1500        // Pulse width milieu (µs) - Idle
#define ESC_PULSE_MAX 2000       // Pulse width maximum (µs) - Maximum

// ============================================================================
// CONNEXIONS LoRa (SPI)
// ============================================================================
#define LORA_MISO 19         // GPIO 19 - MISO (Master In Slave Out)
#define LORA_MOSI 27         // GPIO 27 - MOSI (Master Out Slave In)
#define LORA_SCK 5           // GPIO 5 - SCK (Serial Clock)
#define LORA_CS 18           // GPIO 18 - CS (Chip Select)
#define LORA_RST 14          // GPIO 14 - RST (Reset)
#define LORA_IRQ 26          // GPIO 26 - IRQ (Interrupt Request)

#define LORA_BAND 433E6      // LoRa Band 433MHz (Europe)

// ============================================================================
// CONNEXIONS I²C (Capteurs - optionnel)
// ============================================================================
// Note: Les pins I²C standard (21, 22) sont utilisés pour le récepteur radio
// Si des capteurs I²C sont nécessaires, utiliser d'autres pins GPIO
#define I2C_SDA_PIN 21       // GPIO 21 - Utilisé pour PWM1 IN (non disponible pour I²C)
#define I2C_SCL_PIN 22       // GPIO 22 - Utilisé pour PWM2 IN (non disponible pour I²C)

// ============================================================================
// PARAMÈTRES NAVIGATION
// ============================================================================
#define WAYPOINT_DISTANCE 10  // Distance minimale pour atteindre un waypoint (mètres)
#define WIND_DISTANCE 30      // Distance minimale pour mesure vent (mètres)

// ============================================================================
// COMMUNICATION SÉRIE
// ============================================================================
#define SERIAL_BAUD_RATE 9600  // Baud rate pour debug série

#endif // CONFIG_PINS_H

