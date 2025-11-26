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

## Scénario complet de fonctionnement

### 🚀 **Mission type : Navigation autonome avec waypoints**

#### **Phase 1 : Initialisation (30 secondes)**
```
[00:00] Démarrage ESP32 + initialisation modules
[00:05] LoRa 433MHz : Connexion établie
[00:10] GPS : Recherche satellites (peut prendre 1-2 minutes)
[00:15] Servos : Position initiale (Sail: -10°, Rudder: 20°)
[00:20] Radiocommande : Détection récepteur Pro-Tronik 2.4GHz
[00:25] Moteur ESC : Armement et position idle
[00:30] Système prêt → Mode "setup-ready"
```

#### **Phase 2 : Programmation mission (2 minutes)**
```
[00:30] Station de contrôle envoie waypoints via LoRa :
        Waypoint 1: 43.123456, 5.987654 (Port de départ)
        Waypoint 2: 43.125678, 5.989876 (Bouée intermédiaire) 
        Waypoint 3: 43.127890, 5.991098 (Point d'arrivée)

[00:35] Bateau confirme : "3 waypoints loaded" → Mode "route-ready"

[01:00] Vérification matériel :
        - GPS : Position fixe acquise
        - LoRa : Signal fort (-45 dBm)
        - Servos : Réponse correcte
        - Radiocommande : Standby (SEL=0, mode autonome)

[02:00] Mission validée, prêt pour observation vent
```

#### **Phase 3 : Observation du vent (3-5 minutes)**
```
[02:00] Commande "wind-observation" reçue
[02:01] Configuration automatique :
        - Sail : 10° (position d'observation)
        - Rudder : -20° (virage pour analyse)
        - Moteur : Idle (pas de propulsion)

[02:01-05:00] Navigation d'observation :
        - Distance parcourue : 30+ mètres requis
        - Mesure heading continu via GPS
        - Calcul direction vent relative au bateau
        
[05:00] Vent détecté : 135° (Sud-Est)
        Bateau confirme : "wind acquired" → Mode "wind-ready"
```

#### **Phase 4 : Navigation autonome (15-30 minutes)**
```
[05:00] Commande "navigate" reçue
[05:01] Calcul route optimale :
        - Waypoint 1 → 2 : Cap 067°, distance 245m
        - Vent à 135° → Navigation possible (pas de vent contraire)
        - Ajustement servos : Sail -5°, Rudder 15°

[05:01-20:00] Navigation vers Waypoint 2 :
        Télémétrie continue (toutes les 2 secondes) :
        {
          "mode": "navigate",
          "location": [43.124567, 5.988765],
          "servos": {"sail": -3, "rudder": 12},
          "heading": 69,
          "wind": 135,
          "waypoints": {"total": 3, "current": 2},
          "distance_to_waypoint": 156.7,
          "speed": 2.1
        }

[20:00] Waypoint 2 atteint (distance < 10m)
        Bateau confirme : "Waypoint 2 reached"
        Calcul automatique route vers Waypoint 3

[20:01-35:00] Navigation vers Waypoint 3 :
        - Cap ajusté : 078°, distance 198m
        - Vent toujours favorable
        - Vitesse moyenne : 2.3 nœuds
```

#### **Phase 5 : Intervention d'urgence (30 secondes)**
```
[25:00] URGENCE : Obstacle détecté visuellement par opérateur
[25:01] Activation radiocommande : SEL=1 sur émetteur
[25:02] Détection automatique par bateau :
        "Radio control active" → Basculement immédiat

[25:02-25:30] Contrôle manuel :
        - PWM1 (1750µs) → Sail 25° (évitement)
        - PWM2 (1250µs) → Rudder -30° (virage serré)
        - Navigation autonome suspendue
        - Télémétrie continue avec signaux radio

[25:30] Obstacle évité, retour autonome : SEL=0
[25:31] "Autonomous mode active" → Reprise navigation
        Recalcul position et route vers Waypoint 3
```

#### **Phase 6 : Fin de mission (2 minutes)**
```
[35:00] Waypoint 3 atteint
        Bateau confirme : "All waypoints completed"
        Position finale : 43.127890, 5.991098

[35:01] Arrêt automatique :
        - Servos : Position neutre (Sail 0°, Rudder 0°)
        - Moteur : Idle permanent
        - Mode : "mission-completed"

[37:00] Commande optionnelle "restart" pour nouvelle mission
        ou récupération manuelle du bateau
```

### 📊 **Statistiques de la mission**
- **Durée totale** : 37 minutes
- **Distance parcourue** : ~650 mètres
- **Waypoints atteints** : 3/3 (100%)
- **Interventions manuelles** : 1 (30 secondes)
- **Messages LoRa échangés** : ~1100 (télémétrie + commandes)
- **Précision GPS** : ±3 mètres
- **Vitesse moyenne** : 2.2 nœuds

### 🔧 **Points critiques surveillés**
1. **Signal LoRa** : Maintien connexion jusqu'à 10+ km
2. **Précision GPS** : Validation position toutes les 2 secondes  
3. **Réponse servos** : Vérification angles réels vs consignes
4. **Basculement modes** : Transition autonome ↔ radiocommande < 1 seconde
5. **Gestion vent** : Évitement zones de vent contraire automatique
6. **Sécurité** : Intervention manuelle prioritaire à tout moment

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

