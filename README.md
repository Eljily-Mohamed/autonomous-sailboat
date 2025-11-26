# Arduino Projects Collection

Collection complète de projets Arduino/ESP32 pour le système de navigation autonome d'un bateau.

## Structure du projet

### 🚢 Projets principaux
- **`boat/`** - Firmware principal du bateau autonome
- **`test_signal_50/`** - Projet de test pour signaux PWM

### 🧭 Modules de test
- **`GPS/`** - Tests du module GPS
- **`heading/`** - Tests de cap et navigation
- **`LoRa/`** - Tests de communication LoRa (émetteur/récepteur)
- **`servo/`** - Tests des servomoteurs
- **`transceiver/`** - Tests de transceiver
- **`reset/`** - Utilitaire de reset

### 📚 Bibliothèques
- **`libraries/AXP202X_Library/`** - Gestion de l'alimentation
- **`libraries/ESP32Servo/`** - Contrôle des servos ESP32
- **`libraries/LoRa/`** - Communication LoRa
- **`libraries/TinyGPSPlus/`** - Parsing GPS

## Projet principal : AutoBoat

Le projet principal se trouve dans le dossier `boat/` et contient :

### Architecture modulaire
- **GPS** : Navigation et positionnement
- **ServoControl** : Contrôle des servos (voile et safran)
- **RadioReceiver** : Réception radiocommande
- **MotorControl** : Contrôle moteur
- **LoRa** : Communication longue portée

### Modes de fonctionnement
1. **Mode Autonome** (SEL=0) : Navigation automatique avec waypoints GPS
2. **Mode Radiocommande** (SEL=1) : Contrôle manuel via radio

## Matériel
- **Microcontrôleur** : ESP32
- **GPS** : Module de positionnement
- **Servos** : 2 servos (Sail et Rudder)
- **Radio** : Récepteur PWM
- **LoRa** : Module de communication
- **Moteur** : Contrôleur de moteur

## Installation

1. Cloner le dépôt
2. Ouvrir le projet souhaité dans l'IDE Arduino
3. Installer les bibliothèques depuis le dossier `libraries/`
4. Sélectionner la carte ESP32 Dev Module
5. Compiler et téléverser

## Auteur

Mohamed EL JILY - Projet de navigation autonome
