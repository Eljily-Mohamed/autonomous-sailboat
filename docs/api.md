# API Reference

## Module GPS

### `void init()`
Initialise le module GPS et configure Serial1.

### `void upDatePosition()`
Met à jour la position GPS en lisant les données depuis Serial1. Doit être appelé régulièrement.

### `void computeDirectPath(double LocLatObj, double LocLngObj)`
Calcule la distance et le cap vers un waypoint.

**Paramètres :**
- `LocLatObj` : Latitude du waypoint
- `LocLngObj` : Longitude du waypoint

### `double getLat()` / `double getLng()`
Retourne la latitude/longitude actuelle.

### `double getHeading()`
Retourne le cap vers le waypoint actuel (en degrés).

### `double getSmoothHeading()`
Retourne le cap lissé (moyenne des dernières valeurs).

### `double getDist()`
Retourne la distance vers le waypoint (en mètres).

### `int getStatus()`
Retourne le statut GPS :
- `0` : Aucun signal
- `1` : Recherche satellites
- `2` : Fix GPS obtenu

### `int getSatellites()`
Retourne le nombre de satellites visibles.

---

## Module ServoControl

### `void init()`
Initialise les servomoteurs et configure le PWM hardware.

### `void setSailAngle(float angle)`
Définit l'angle du servo Sail (voile).

**Paramètres :**
- `angle` : Angle en degrés (-10° à +10°)

### `void setRudderAngle(float angle)`
Définit l'angle du servo Rudder (gouvernail).

**Paramètres :**
- `angle` : Angle en degrés (-45° à +45°)

### `void setAnglesDirect(int sailAngle, int rudderAngle)`
Définit directement les angles en mode PWM (pour copie signal radiocommande).

**Paramètres :**
- `sailAngle` : Angle Sail en microsecondes PWM
- `rudderAngle` : Angle Rudder en microsecondes PWM

### `float getSailAngle()` / `float getRudderAngle()`
Retourne l'angle actuel du servo.

---

## Module RadioReceiver

### `void init()`
Initialise le récepteur radio et configure les pins d'entrée.

### `void update()`
Lit les signaux PWM et met à jour le mode. **Doit être appelé régulièrement.**

### `bool isRadioControlMode()`
Retourne `true` si le mode radiocommande est actif (SEL=1), `false` sinon (mode autonome).

### `bool hasModeChanged()`
Retourne `true` si le mode a changé depuis le dernier appel.

### `void resetModeChanged()`
Réinitialise le flag de changement de mode.

### `int getPWM1()` / `int getPWM2()`
Retourne la valeur PWM brute du canal 1 ou 2 (en microsecondes).

### `int readPWM(int pin)`
Lit un signal PWM depuis un pin spécifique.

**Paramètres :**
- `pin` : Pin GPIO à lire

**Retourne :** Largeur d'impulsion en microsecondes

---

## Module MotorControl

### `void init()`
Initialise le contrôleur moteur et configure le PWM.

### `void arm()`
Arme l'ESC avec la séquence d'armement standard.

### `void disarm()`
Désarme l'ESC (arrêt du moteur).

### `void setSpeed(int speed, bool usePercent = false)`
Définit la vitesse du moteur.

**Paramètres :**
- `speed` : Vitesse (en µs si `usePercent=false`, en % si `usePercent=true`)
- `usePercent` : Si `true`, speed est en pourcentage (0-100)

### `int getSpeed()`
Retourne la vitesse actuelle en microsecondes.

### `bool isArmed()`
Retourne `true` si le moteur est armé.

---

## Fonctions principales (boat.ino)

### `void setupLoRa()`
Initialise le module LoRa avec retry et diagnostic d'erreurs.

**Retourne :** `true` si succès, `false` sinon

### `void setupGPS()`
Initialise le GPS et attend un fix GPS (timeout 30 secondes).

### `void handleServoControl()`
Gère le contrôle des servomoteurs selon le mode (manuel ou autonome).

### `void handleNavigation()`
Gère la navigation autonome : calcul de route, adaptation au vent.

### `void parseMessage(String data)`
Parse un message JSON et exécute les commandes.

**Paramètres :**
- `data` : Message JSON à parser

### `void sendInfo()`
Envoie la télémetrie périodique via LoRa (si LoRa initialisé).

### `void jsonMessage(String origin, String type, String message)`
Envoie un message JSON via LoRa.

**Paramètres :**
- `origin` : Origine du message ("boat", "server", etc.)
- `type` : Type de message ("info", "error", "command")
- `message` : Contenu du message

### `void checkAndReinitLoRa()`
Vérifie périodiquement l'état de LoRa et réinitialise si nécessaire.

---

## Fonctions mathématiques

### `double relativeAngle(double reference, double target)`
Calcule l'angle relatif entre deux angles.

**Retourne :** Angle relatif (-180° à +180°)

### `double oppositeAngle(double angle)`
Calcule l'angle opposé (angle + 180°).

### `bool isBetween(double angle, double start, double end)`
Vérifie si un angle est entre deux autres angles.

### `bool sameSign(double a, double b)`
Vérifie si deux valeurs ont le même signe.

---

## Variables globales

### `String boatMode`
Mode actuel du bateau :
- `"setup"` : Initialisation
- `"setup-ready"` : Prêt
- `"standby"` : En attente
- `"route-ready"` : Waypoints reçus
- `"wind-observation"` : Observation vent
- `"wind-ready"` : Vent mesuré
- `"navigate"` : Navigation active

### `int sendInterval`
Intervalle d'envoi de télémetrie (en millisecondes, défaut : 1000ms).

### `bool loraInitialized`
Flag indiquant si LoRa est initialisé et fonctionnel.

---

*Pour comprendre l'utilisation, voir [Architecture](architecture.md)*

