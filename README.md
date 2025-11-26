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

## 🧪 **Scénario de Test Système Complet**

### **Phase 1 : Tests Unitaires (10 minutes)**

#### **1.1 Test Communication LoRa**
```bash
# Terminal série Arduino IDE (9600 baud)
# Vérifier messages de démarrage
[00:00] {"origin":"System","type":"info","message":"Starting AutoBoat firmware..."}
[00:05] {"origin":"LoRa","type":"info","message":"Ready"}

# Test envoi/réception
# Envoyer via série :
{"origin":"server","type":"command","message":"restart"}
# Attendre : Redémarrage ESP32 confirmé
```

#### **1.2 Test Servos et Calibration**
```json
# Test position initiale
[00:30] {"origin":"Servos","type":"info","message":"Ready"}
# Vérifier : Sail -10°, Rudder 20° (positions visibles)

# Test mouvement servos via série :
{"origin":"server","type":"command","message":"test-servo-sail:0"}
{"origin":"server","type":"command","message":"test-servo-rudder:0"}
# Vérifier : Servos bougent vers position neutre
```

#### **1.3 Test Radiocommande**
```bash
# Activer émetteur radiocommande
# Canal SEL = 0 (mode autonome)
[01:00] {"origin":"RadioReceiver","type":"info","message":"Ready"}

# Test basculement mode :
# Canal SEL = 1 → Mode radiocommande
[01:30] {"origin":"Mode","type":"info","message":"Radio control active"}

# Test contrôle servos :
# PWM1 = 1500µs → Sail neutre
# PWM2 = 1500µs → Rudder neutre
# Vérifier mouvement servos en temps réel
```

#### **1.4 Test GPS (si disponible)**
```json
# Décommenter setupGPS() dans boat.ino
[02:00] {"origin":"GPS","type":"info","message":"Initializing"}
[02:30] {"origin":"GPS","type":"info","message":"Ready"}
# ou
[02:30] {"origin":"GPS","type":"error","message":"Setup failed - Continuing without GPS"}
```

### **Phase 2 : Tests d'Intégration (15 minutes)**

#### **2.1 Test Communication Bidirectionnelle**
```json
# Test 1 : Chargement waypoints
{"origin":"server","type":"command","message":"waypoints:43.123,5.987;43.125,5.989;43.127,5.991"}

# Réponse attendue :
{"origin":"waypoints","type":"info","message":"3 waypoints loaded"}

# Vérification mode :
# Mode passe de "setup-ready" → "route-ready"
```

#### **2.2 Test Observation Vent (Simulation)**
```json
# Test 2 : Commande observation vent
{"origin":"server","type":"command","message":"wind-observation"}

# Réponses attendues :
{"origin":"boat","type":"info","message":{"mode":"wind-observation","servos":{"sail":10,"rudder":-20}}}

# Simulation détection vent (après 30 secondes) :
{"origin":"wind","type":"info","message":"wind acquired"}
# Mode passe à "wind-ready"
```

#### **2.3 Test Navigation Simulée**
```json
# Test 3 : Lancement navigation
{"origin":"server","type":"command","message":"navigate"}

# Réponse attendue :
{"origin":"boat","type":"info","message":{"mode":"navigate","waypoints":{"total":3,"current":1}}}

# Télémétrie continue (toutes les 2 secondes) :
{
  "origin":"boat","type":"info",
  "message":{
    "mode":"navigate",
    "servos":{"sail":-5,"rudder":15},
    "control_mode":"autonomous"
  }
}
```

### **Phase 3 : Tests de Robustesse (10 minutes)**

#### **3.1 Test Basculement d'Urgence**
```bash
# Pendant navigation autonome :
# 1. Activer radiocommande (SEL=1)
[15:00] {"origin":"Mode","type":"info","message":"Radio control active"}

# 2. Bouger sticks radiocommande
# Vérifier : Servos suivent immédiatement les commandes PWM

# 3. Retour autonome (SEL=0)
[15:30] {"origin":"Mode","type":"info","message":"Autonomous mode active"}
# Vérifier : Reprise navigation automatique
```

#### **3.2 Test Gestion d'Erreurs**
```json
# Test commande invalide :
{"origin":"server","type":"command","message":"commande-inexistante"}
# Vérifier : Pas de plantage, message ignoré

# Test navigation sans vent :
{"origin":"server","type":"command","message":"navigate"}
# Réponse attendue :
{"origin":"navigation","type":"error","message":"Not wind-ready"}
```

#### **3.3 Test Redémarrage**
```json
# Test redémarrage à distance :
{"origin":"server","type":"command","message":"restart"}
# Vérifier : ESP32 redémarre, tous systèmes se réinitialisent
```

### **Phase 4 : Test Longue Durée (30 minutes)**

#### **4.1 Mission Complète Simulée**
```bash
[00:00] Démarrage système
[02:00] Chargement 5 waypoints
[05:00] Observation vent (simulation 3 minutes)
[08:00] Navigation autonome continue
[25:00] Test intervention radiocommande (2 minutes)
[27:00] Retour navigation autonome
[30:00] Fin mission - tous waypoints "atteints"
```

#### **4.2 Surveillance Continue**
```json
# Vérifications toutes les 5 minutes :
- Télémétrie régulière (pas de gaps > 3 secondes)
- Réponse servos cohérente
- Communication LoRa stable
- Pas de redémarrages intempestifs
- Mémoire ESP32 stable (pas de fuites)
```

### **📋 Checklist de Validation**

#### **✅ Communication**
- [ ] LoRa : Envoi/réception messages JSON
- [ ] USB Série : Debug messages visibles
- [ ] Radiocommande : Détection SEL + PWM1/PWM2

#### **✅ Contrôle Servos**
- [ ] Position initiale correcte
- [ ] Réponse aux commandes autonomes
- [ ] Réponse aux commandes radiocommande
- [ ] Basculement instantané entre modes

#### **✅ Navigation**
- [ ] Chargement waypoints
- [ ] Observation vent (simulation)
- [ ] Calculs de navigation
- [ ] Télémétrie temps réel

#### **✅ Robustesse**
- [ ] Gestion erreurs de communication
- [ ] Redémarrage à distance
- [ ] Fonctionnement sans GPS
- [ ] Stabilité longue durée (30+ minutes)

### **🚨 Critères d'Échec**
- Redémarrage intempestif (> 1 par heure)
- Perte communication LoRa (> 10 secondes)
- Servos ne répondent pas (> 2 secondes)
- Basculement mode défaillant (> 3 secondes)
- Fuite mémoire (RAM < 200KB disponible)

### **📊 Métriques de Performance**
- **Temps de démarrage** : < 30 secondes
- **Latence télémétrie** : < 2 secondes
- **Précision servos** : ±2°
- **Temps basculement mode** : < 1 seconde
- **Stabilité système** : 99.9% uptime

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
