# AutoBoat - Documentation

Bienvenue dans la documentation du projet **AutoBoat**, un système de navigation autonome pour voilier basé sur ESP32.

## 🚤 À propos du projet

AutoBoat est un système embarqué permettant la navigation autonome d'un voilier à l'aide de :
- **GPS** pour la localisation et la navigation
- **LoRa** pour la communication longue portée
- **Radiocommande** pour le contrôle manuel
- **Servomoteurs** pour le contrôle de la voile et du gouvernail

Le système peut fonctionner en deux modes :
- **Mode Manuel** : Contrôle via radiocommande
- **Mode Autonome** : Navigation automatique vers des waypoints

## 📚 Navigation rapide

- [Introduction au projet](introduction.md) - Vue d'ensemble et objectifs
- [Fonctionnement](fonctionnement.md) - Architecture système et communication
- [Démarrage](demarrage.md) - Installation et configuration
- [Architecture du code](architecture.md) - Structure des fichiers et modules
- [API Reference](api.md) - Documentation des fonctions principales
- [Troubleshooting](troubleshooting.md) - Résolution de problèmes courants

## 🎯 Fonctionnalités principales

- ✅ Navigation autonome vers des waypoints GPS
- ✅ Détection et adaptation au vent
- ✅ Communication LoRa bidirectionnelle
- ✅ Basculement automatique entre mode manuel et autonome
- ✅ Télémetrie en temps réel
- ✅ Gestion des servomoteurs et moteur

## 🔧 Technologies utilisées

- **Microcontrôleur** : ESP32
- **Communication** : LoRa (SX1278)
- **GPS** : Module GPS série
- **Servomoteurs** : Contrôle PWM hardware
- **Langage** : C++ (Arduino)

## 📁 Structure du projet

Le projet est organisé dans le dossier `boat/` :

```
Arduino/
├── boat/              # Projet AutoBoat
│   ├── boat.ino       # Fichier principal
│   ├── config_pins.h  # Configuration des pins
│   ├── Gps.*          # Module GPS
│   ├── ServoControl.* # Module servomoteurs
│   ├── RadioReceiver.*# Module récepteur radio
│   └── MotorControl.* # Module contrôle moteur
└── docs/              # Documentation (ce dossier)
```

---

*Documentation générée avec MkDocs Material*

