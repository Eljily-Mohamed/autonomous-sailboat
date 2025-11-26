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

## Communication

Le système utilise plusieurs canaux de communication :

### Canaux disponibles
- **LoRa 433MHz** : Communication principale longue portée (10+ km)
- **USB Série** : Debug et contrôle direct via PC
- **Radiocommande 2.4GHz** : Contrôle d'urgence (1-2 km)
- **GPS UART** : Réception position satellite

### Format des messages
Tous les messages utilisent le format JSON :
```json
{
  "origin": "boat|server",
  "type": "info|command|error",
  "message": "contenu du message"
}
```

### Scénario type de mission

#### 1. Démarrage
```json
{"origin":"System","type":"info","message":"All systems ready"}
```

#### 2. Chargement waypoints
```json
// Commande envoyée au bateau
{"origin":"server","type":"command","message":"waypoints:43.123,5.987;43.125,5.989"}

// Réponse du bateau
{"origin":"waypoints","type":"info","message":"2 waypoints loaded"}
```

#### 3. Observation du vent
```json
// Commande
{"origin":"server","type":"command","message":"wind-observation"}

// Télémétrie continue
{
  "origin":"boat","type":"info",
  "message":{
    "mode":"wind-observation",
    "location":[43.123,5.987],
    "servos":{"sail":10,"rudder":-20},
    "control_mode":"autonomous",
    "heading":45,
    "wind":0
  }
}
```

#### 4. Navigation autonome
```json
// Lancement navigation
{"origin":"server","type":"command","message":"navigate"}

// Télémétrie navigation
{
  "origin":"boat","type":"info",
  "message":{
    "mode":"navigate",
    "location":[43.124,5.988],
    "servos":{"sail":-5,"rudder":15},
    "control_mode":"autonomous",
    "heading":67,
    "wind":135,
    "waypoints":{"total":2,"current":1}
  }
}
```

#### 5. Intervention d'urgence
```json
// Activation automatique radiocommande (SEL=1)
{"origin":"Mode","type":"info","message":"Radio control active"}

// Télémétrie en mode radio
{
  "origin":"boat","type":"info",
  "message":{
    "control_mode":"radio",
    "servos":{"sail":25,"rudder":-30},
    "radio_signals":{"pwm1":1750,"pwm2":1250}
  }
}
```

## Configuration matérielle

### Pins LoRa (SPI)
- MISO: GPIO 19, MOSI: GPIO 27, SCK: GPIO 5
- CS: GPIO 18, RST: GPIO 14, IRQ: GPIO 26

### Pins Radiocommande (PWM Input)
- PWM1 IN: GPIO 21 (Sail), PWM2 IN: GPIO 22 (Rudder)
- SEL IN: GPIO 23 (Sélection mode)

### Pins Servos (PWM Output - Open Drain)
- SERVO_SAIL: GPIO 2, SERVO_RUDDER: GPIO 25
- MOTOR_ESC: GPIO 4

## Compilation

1. Ouvrir `boat.ino` dans l'IDE Arduino
2. Installer les bibliothèques requises:
   - ESP32Servo
   - LoRa
3. Sélectionner la carte: ESP32 Dev Module
4. Compiler et téléverser

## Documentation

- `config_pins.h` : Configuration complète des pins GPIO

## Auteur

Mohamed EL JILY

