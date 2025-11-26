# AutoBoat - Firmware ESP32

Firmware principal pour le système de navigation autonome d'un bateau.

## Architecture

Architecture modulaire sans multiplexeur avec les modules suivants:
- **GPS**: Gestion de la position et navigation
- **ServoControl**: Contrôle des servos (Sail/Aileron et Rudder/Safran)
- **RadioReceiver**: Réception et traitement des signaux PWM de la radiocommande
- **MotorControl**: Contrôle du moteur
- **LoRa**: Communication longue portée

## Matériel

- **Microcontrôleur**: ESP32
- **GPS**: Module GPS
- **Servos**: 2 servos (Sail et Rudder)
- **Radio**: Récepteur PWM pour mode radiocommande
- **Moteur**: Contrôleur de moteur
- **LoRa**: Module de communication LoRa

## Modes de fonctionnement

### Mode Autonome (SEL=0)
Le bateau navigue de manière autonome en suivant les waypoints GPS et en ajustant les servos selon la direction du vent.

### Mode Radiocommande (SEL=1)
Le bateau copie directement les signaux PWM de la radiocommande vers les servos pour contrôle manuel.

## Structure du code

```
boat/
├── boat.ino              # Firmware principal
├── config_pins.h         # Configuration des pins GPIO
├── Gps.hpp/cpp           # Module GPS
├── ServoControl.hpp/cpp  # Module contrôle servos
├── RadioReceiver.hpp/cpp # Module récepteur radio
└── MotorControl.hpp/cpp  # Module contrôle moteur
```

## Compilation

1. Ouvrir `boat.ino` dans l'IDE Arduino
2. Installer les bibliothèques requises:
   - ESP32Servo
   - LoRa
3. Sélectionner la carte: ESP32 Dev Module
4. Compiler et téléverser

## Auteur

Mohamed EL JILY

