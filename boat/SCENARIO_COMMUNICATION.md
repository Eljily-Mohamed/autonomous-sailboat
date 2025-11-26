# 🚢 Scénario de Communication - AutoBoat

Ce document décrit un scénario complet de communication entre le bateau autonome et la station de contrôle.

## 🎯 **SCÉNARIO : Mission de Navigation Autonome**

### **📍 ÉTAPE 1 : Démarrage du Système**

**🔌 Au démarrage du bateau :**

```json
[BATEAU] → Série USB + LoRa : {"origin":"System","type":"info","message":"Starting AutoBoat firmware..."}
[BATEAU] → Série USB + LoRa : {"origin":"LoRa","type":"info","message":"Ready"}
[BATEAU] → Série USB + LoRa : {"origin":"Servos","type":"info","message":"Ready"}
[BATEAU] → Série USB + LoRa : {"origin":"RadioReceiver","type":"info","message":"Ready"}
[BATEAU] → Série USB + LoRa : {"origin":"Motor","type":"info","message":"Ready"}
[BATEAU] → Série USB + LoRa : {"origin":"System","type":"info","message":"All systems ready"}
```

**📊 État initial :**
- **Mode :** `setup-ready`
- **Servos :** Position initiale (Sail: -10°, Rudder: 20°)
- **Communication :** LoRa 433MHz + USB Série actifs

---

### **📡 ÉTAPE 2 : Envoi des Waypoints depuis la Station de Contrôle**

**📤 Station de contrôle → Bateau (via LoRa) :**
```json
{
  "origin": "server",
  "type": "command", 
  "message": "waypoints:43.123,5.987;43.125,5.989;43.127,5.991"
}
```

**🔄 Traitement par le bateau :**
- Parsing du message dans `parseMessage()`
- Extraction des coordonnées GPS
- Stockage des waypoints en mémoire

**📥 Réponse du bateau :**
```json
[BATEAU] → LoRa + USB : {"origin":"waypoints","type":"info","message":"3 waypoints loaded"}
```

**📊 Nouvel état :**
- **Mode :** `route-ready`
- **Waypoints :** 3 points chargés
- **Prêt pour :** Observation du vent

---

### **🌬️ ÉTAPE 3 : Observation du Vent**

**📤 Station → Bateau :**
```json
{
  "origin": "server",
  "type": "command",
  "message": "wind-observation"
}
```

**🤖 Exécution automatique par le bateau :**
1. **Sauvegarde position actuelle** comme point de référence
2. **Configuration servos** pour observation :
   - Sail : 10°
   - Rudder : -20°
3. **Démarrage mouvement** pour mesurer le vent

**📊 Télémétrie continue (toutes les 1 seconde) :**
```json
[BATEAU] → LoRa + USB : {
  "origin": "boat",
  "type": "info",
  "message": {
    "mode": "wind-observation",
    "location": [43.1234567890, 5.9876543210],
    "servos": {"sail": 10, "rudder": -20},
    "control_mode": "autonomous",
    "heading": 45,
    "wind": 0,
    "distance_traveled": 15.2
  }
}
```

**🌪️ Détection du vent (après 30m de distance) :**
```json
[BATEAU] → LoRa + USB : {"origin":"wind","type":"info","message":"wind acquired"}
```

**📊 Nouvel état :**
- **Mode :** `wind-ready`
- **Direction du vent :** 135° (calculée automatiquement)
- **Prêt pour :** Navigation

---

### **🧭 ÉTAPE 4 : Lancement de la Navigation**

**📤 Station → Bateau :**
```json
{
  "origin": "server",
  "type": "command",
  "message": "navigate"
}
```

**🚀 Démarrage de la navigation autonome :**
1. **Sélection du premier waypoint**
2. **Calcul de la route** en fonction du vent
3. **Ajustement automatique** des servos

**📊 Nouvel état :**
- **Mode :** `navigate`
- **Waypoint actuel :** 1/3
- **Navigation :** Active

---

### **⛵ ÉTAPE 5 : Navigation Active**

**📡 Télémétrie continue (toutes les 2 secondes) :**
```json
[BATEAU] → LoRa + USB : {
  "origin": "boat",
  "type": "info",
  "message": {
    "mode": "navigate",
    "location": [43.1245678901, 5.9886543210],
    "servos": {"sail": -5, "rudder": 15},
    "control_mode": "autonomous", 
    "heading": 67,
    "wind": 135,
    "waypoints": {"total": 3, "current": 1},
    "distance_to_waypoint": 245.7,
    "speed": 2.3
  }
}
```

**🧠 Logique de navigation automatique :**
- **Calcul continu** de la route vers le waypoint
- **Ajustement des servos** selon le vent et la direction
- **Évitement** des zones de vent contraire
- **Progression** vers les waypoints successifs

**🎯 Atteinte d'un waypoint :**
```json
[BATEAU] → LoRa + USB : {"origin":"navigation","type":"info","message":"Waypoint 1 reached"}
```

---

### **🚨 ÉTAPE 6 : Intervention d'Urgence (Radiocommande)**

**📻 L'opérateur active la radiocommande :**
- **Canal SEL = 1** sur la radiocommande Pro-Tronik 2.4GHz
- **Détection automatique** par le bateau

**🔄 Notification immédiate :**
```json
[BATEAU] → LoRa + USB : {"origin":"Mode","type":"info","message":"Radio control active"}
```

**🎮 Contrôle direct des servos :**
- **PWM1 IN (GPIO 21)** → Contrôle Sail
- **PWM2 IN (GPIO 22)** → Contrôle Rudder
- **Copie directe** PWM IN → PWM OUT
- **Bypass** de la logique de navigation

**📊 Télémétrie en mode radiocommande :**
```json
[BATEAU] → LoRa + USB : {
  "origin": "boat",
  "type": "info",
  "message": {
    "mode": "navigate",
    "location": [43.1267891234, 5.9896543210],
    "servos": {"sail": 25, "rudder": -30},
    "control_mode": "radio", 
    "heading": 82,
    "wind": 135,
    "radio_signals": {"pwm1": 1750, "pwm2": 1250}
  }
}
```

---

### **🔄 ÉTAPE 7 : Retour en Mode Autonome**

**📻 L'opérateur désactive la radiocommande :**
- **Canal SEL = 0** sur la radiocommande

**📡 Notification automatique :**
```json
[BATEAU] → LoRa + USB : {"origin":"Mode","type":"info","message":"Autonomous mode active"}
```

**🤖 Reprise de la navigation autonome :**
- **Recalcul** de la position actuelle
- **Reprise** de la route vers le waypoint suivant
- **Ajustement** des servos selon la logique de navigation

---

### **🏁 ÉTAPE 8 : Fin de Mission**

**🎯 Dernier waypoint atteint :**
```json
[BATEAU] → LoRa + USB : {"origin":"navigation","type":"info","message":"All waypoints completed"}
```

**📤 Commande d'arrêt (optionnelle) :**
```json
{
  "origin": "server",
  "type": "command",
  "message": "restart"
}
```

**🔄 Redémarrage du système :**
```json
[BATEAU] → LoRa + USB : {"origin":"System","type":"info","message":"Restarting system..."}
```

---

## 📊 **RÉSUMÉ DES CANAUX DE COMMUNICATION**

| **Canal** | **Fréquence/Protocole** | **Usage** | **Direction** | **Portée** |
|-----------|-------------------------|-----------|---------------|------------|
| **LoRa** | 433MHz | Communication principale | ↕️ Bidirectionnel | 10+ km |
| **USB Série** | 9600 baud | Debug + Contrôle direct | ↕️ Bidirectionnel | Câble USB |
| **Radiocommande** | 2.4GHz Pro-Tronik | Contrôle d'urgence | ➡️ Vers bateau | 1-2 km |
| **GPS** | UART 9600 baud | Positionnement | ➡️ Vers bateau | Global |

## 🔧 **Configuration Matérielle**

### **Pins LoRa (SPI) :**
- **MISO :** GPIO 19
- **MOSI :** GPIO 27  
- **SCK :** GPIO 5
- **CS :** GPIO 18
- **RST :** GPIO 14
- **IRQ :** GPIO 26

### **Pins Radiocommande (PWM Input) :**
- **PWM1 IN :** GPIO 21 (Aileron/Sail)
- **PWM2 IN :** GPIO 22 (Safran/Rudder)
- **SEL IN :** GPIO 23 (Sélection mode)

### **Pins Servos (PWM Output - Open Drain) :**
- **SERVO_SAIL :** GPIO 2
- **SERVO_RUDDER :** GPIO 25
- **MOTOR_ESC :** GPIO 4

## 🎯 **Messages JSON Standards**

### **Format de base :**
```json
{
  "origin": "boat|server|GPS|LoRa|Mode|System",
  "type": "info|command|error|warning",
  "message": "contenu du message ou objet JSON"
}
```

### **Commandes supportées :**
- `waypoints:lat1,lng1;lat2,lng2;...`
- `wind-observation`
- `wind-command:{"value":135}`
- `navigate`
- `restart`

### **Types de télémétrie :**
- **Position GPS** (latitude, longitude)
- **État des servos** (angles sail/rudder)
- **Mode de contrôle** (autonomous/radio)
- **Direction du bateau** (heading)
- **Direction du vent** (wind)
- **Progression waypoints** (current/total)
- **Signaux radio** (valeurs PWM)

## 🚀 **Avantages du Système**

1. **Communication redondante** (LoRa + USB + Radio)
2. **Télémétrie temps réel** avec position précise
3. **Contrôle d'urgence** via radiocommande
4. **Messages structurés JSON** pour interopérabilité
5. **Gestion d'erreurs** et notifications d'état
6. **Navigation autonome** avec évitement du vent contraire
7. **Basculement automatique** entre modes de contrôle

---

**Auteur :** Mohamed EL JILY  
**Projet :** AutoBoat - Système de Navigation Autonome  
**Date :** Novembre 2024
