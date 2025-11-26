# 🚢 AutoBoat - Système de Navigation Autonome

Collection complète de projets Arduino/ESP32 pour un bateau autonome avec navigation GPS, communication LoRa et contrôle radiocommande d'urgence.

## 🎯 Objectif du Projet

Développement d'un système de navigation autonome pour voilier capable de :
- **Navigation GPS autonome** avec waypoints programmables
- **Communication longue portée** via LoRa (10+ km)
- **Contrôle d'urgence** par radiocommande 2.4GHz
- **Observation automatique du vent** pour optimisation de route
- **Télémétrie temps réel** avec position, cap, servos, vent

## 📁 Structure du Projet

### 🚢 **Firmware Principal**
- **`boat/`** - **Système complet AutoBoat** 
  - Navigation autonome GPS avec waypoints
  - Communication LoRa bidirectionnelle
  - Basculement automatique autonome ↔ radiocommande
  - Observation et analyse du vent
  - Contrôle servos voile/safran + moteur ESC
  - Architecture modulaire (GPS, ServoControl, RadioReceiver, MotorControl)

### 🧪 **Projets de Test et Développement**
- **`test_signal_50/`** - Tests signaux PWM (génération 50% + copie signal)
- **`GPS/`** - Tests module GPS et positionnement
- **`heading/`** - Tests calcul de cap et navigation
- **`LoRa/`** - Tests communication LoRa (émetteur/récepteur)
- **`servo/`** - Tests servomoteurs et calibration
- **`transceiver/`** - Tests transceiver radio
- **`reset/`** - Utilitaire de reset système

### 📚 **Bibliothèques Intégrées**
- **`libraries/ESP32Servo/`** - Contrôle servos ESP32 avec PWM matériel
- **`libraries/LoRa/`** - Communication LoRa 433MHz
- **`libraries/TinyGPSPlus/`** - Parsing NMEA GPS avancé
- **`libraries/AXP202X_Library/`** - Gestion alimentation (optionnel)

## ⚡ **Caractéristiques Techniques**

### **Matériel Requis**
| Composant | Modèle/Spécification | Usage |
|-----------|---------------------|-------|
| **Microcontrôleur** | ESP32 Dev Module | Contrôle principal |
| **GPS** | Module UART 9600 baud | Positionnement ±3m |
| **LoRa** | SX1276 433MHz | Communication 10+ km |
| **Radiocommande** | Pro-Tronik 2.4GHz | Contrôle d'urgence |
| **Servos** | 2x Standard PWM | Voile + Safran |
| **ESC Moteur** | PWM 50Hz | Propulsion auxiliaire |

### **Communication**
- **LoRa 433MHz** : Portée 10+ km, télémétrie + commandes
- **USB Série** : Debug et contrôle direct (9600 baud)
- **Radiocommande 2.4GHz** : Contrôle manuel prioritaire
- **GPS UART** : Position temps réel

### **Modes de Fonctionnement**
1. **🤖 Mode Autonome (SEL=0)**
   - Navigation GPS avec waypoints programmables
   - Observation automatique du vent
   - Optimisation de route selon conditions météo
   - Télémétrie continue position/cap/servos

2. **🎮 Mode Radiocommande (SEL=1)**
   - Contrôle manuel direct des servos
   - Basculement instantané (< 1 seconde)
   - Priorité absolue sur mode autonome
   - Télémétrie maintenue

## 🚀 **Démarrage Rapide**

### **Installation**
```bash
git clone https://github.com/Eljily-Mohamed/autonomous-sailboat.git
cd autonomous-sailboat
```

### **Configuration Arduino IDE**
1. Installer les bibliothèques ESP32 (Gestionnaire de cartes)
2. Sélectionner **ESP32 Dev Module**
3. Ouvrir `boat/boat.ino`
4. Vérifier configuration pins dans `boat/config_pins.h`
5. Compiler et téléverser

### **Test Initial**
```json
// Message série pour test
{"origin":"server","type":"command","message":"waypoints:43.123,5.987;43.125,5.989"}
```

## 📊 **Exemple de Mission Type**

**Durée :** 37 minutes | **Distance :** 650m | **Waypoints :** 3 | **Précision :** ±3m

1. **Initialisation** (30s) → Connexion LoRa + GPS + Servos
2. **Programmation** (2min) → Chargement waypoints via LoRa
3. **Observation vent** (3-5min) → Analyse direction vent automatique
4. **Navigation autonome** (15-30min) → Route optimisée GPS
5. **Intervention urgence** (30s) → Basculement radiocommande
6. **Fin mission** (2min) → Tous waypoints atteints

## 🔧 **Configuration Matérielle**

### **Pins ESP32 (Open-Drain pour compatibilité 5V)**
```cpp
// Servos PWM Output
#define SERVO_SAIL_PIN 2      // Voile/Aileron
#define SERVO_RUDDER_PIN 25   // Safran/Rudder  
#define MOTOR_ESC_PIN 4       // Moteur ESC

// Radiocommande PWM Input
#define RADIO_PWM1_IN 21      // Canal voile
#define RADIO_PWM2_IN 22      // Canal safran
#define RADIO_SEL_IN 23       // Sélection mode

// LoRa SPI
#define LORA_CS 18, LORA_RST 14, LORA_IRQ 26
#define LORA_MOSI 27, LORA_MISO 19, LORA_SCK 5
```

## 📈 **Télémétrie Temps Réel**

```json
{
  "origin": "boat",
  "type": "info",
  "message": {
    "mode": "navigate",
    "location": [43.1245678, 5.9887654],
    "servos": {"sail": -5, "rudder": 15},
    "control_mode": "autonomous",
    "heading": 67,
    "wind": 135,
    "waypoints": {"total": 3, "current": 2},
    "distance_to_waypoint": 156.7,
    "speed": 2.1
  }
}
```

## 🛡️ **Sécurité et Fiabilité**

- **Communication redondante** : LoRa + USB + Radiocommande
- **Basculement d'urgence** : Radiocommande prioritaire à tout moment
- **Gestion d'erreurs** : Continuation sans GPS/LoRa si nécessaire
- **Watchdog** : Redémarrage automatique en cas de blocage
- **Télémétrie continue** : Surveillance temps réel de tous les paramètres

## 📚 **Documentation Détaillée**

- **`boat/README.md`** : Documentation complète avec scénario de mission
- **`boat/config_pins.h`** : Configuration matérielle détaillée
- Chaque module dispose de sa propre documentation intégrée

## 🎓 **Contexte Académique**

**Projet :** Système de Navigation Autonome - Période 2 (Semaines 5-7)  
**Objectifs pédagogiques :**
- Architecture modulaire ESP32
- Communication IoT longue portée
- Navigation GPS et calculs trigonométriques
- Systèmes temps réel et gestion d'interruptions
- Intégration matérielle et protocoles de communication

## 👨‍💻 **Auteur**

**Mohamed EL JILY**  
Projet de navigation autonome - ESP32/Arduino  
Novembre 2024
