/**
 * AutoBoat - Firmware principal
 * 
 * Architecture modulaire sans multiplexeur
 * Modules: GPS, ServoControl, RadioReceiver, MotorControl, LoRa
 * 
 * Période 2 (Semaines 5-7)
 * Auteur: Mohamed EL JILY
 */

// ============================================================================
// INCLUDES
// ============================================================================
#include "config_pins.h"
#include "Gps.hpp"
#include "ServoControl.hpp"
#include "RadioReceiver.hpp"
#include "MotorControl.hpp"
#include <SPI.h>
#include <LoRa.h>
#include <vector>
#include <cmath>
#include <ESP32Servo.h>

// ============================================================================
// INSTANCES DES MODULES
// ============================================================================
GPS gpsBoat;
ServoControl servoControl;
RadioReceiver radioReceiver;
MotorControl motorControl;

// ============================================================================
// VARIABLES GLOBALES
// ============================================================================
String boatMode = "setup";
int sendInterval = 1000;  // ms
long sendTimer = -sendInterval;
bool loraInitialized = false;  // Flag pour savoir si LoRa est initialisé
unsigned long lastLoRaCheck = 0;  // Dernière vérification LoRa
const unsigned long LORA_CHECK_INTERVAL = 10000;  // Vérifier LoRa toutes les 10 secondes
int loraFailureCount = 0;  // Compteur d'échecs LoRa
const int MAX_LORA_FAILURES = 3;  // Nombre max d'échecs avant réinitialisation

// Waypoints
String waypoints = "";
double currentWptLat;
double currentWptLng;
int waypointId = -1;
int waypointCount = 0;

// Navigation
double windDirection;
float sailAngle = -10;
float rudderAngle = 20;

// ============================================================================
// INITIALISATION GPS
// ============================================================================
void setupGPS() {
  Serial.println("[GPS] Initialisation...");
  gpsBoat.init();

  int retryDelay = 500;
  int failCount = 30000 / retryDelay;  // Timeout 30 secondes

  Serial.println("  Attente signal GPS...");
  while (gpsBoat.getStatus() != 2 && failCount > 0) {
    gpsBoat.upDatePosition();
    failCount--;
    
    if (failCount % 10 == 0) {  // Afficher toutes les 5 secondes
      Serial.print("  Statut GPS: ");
      int status = gpsBoat.getStatus();
      switch(status) {
        case 0: Serial.println("Aucun signal"); break;
        case 1: Serial.println("Recherche satellites..."); break;
        case 2: Serial.println("Fix GPS obtenu"); break;
        default: Serial.print("Inconnu ("); Serial.print(status); Serial.println(")"); break;
      }
    }
    
    delay(retryDelay);
  }

  if (failCount == 0) {
    Serial.println("[GPS] ⚠ Timeout - Continuation sans GPS fix");
    Serial.println("  Le GPS continuera à essayer en arrière-plan");
  } else {
    Serial.println("[GPS] ✓ Prêt");
    Serial.print("  Position: ");
    Serial.print(gpsBoat.getLat(), 6);
    Serial.print(", ");
    Serial.println(gpsBoat.getLng(), 6);
  }
}

// ============================================================================
// DIAGNOSTIC LoRa - Détecte l'erreur exacte (seulement en cas d'échec)
// ============================================================================
void diagnoseLoRaError() {
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
  delay(100);
  
  // Lire le registre de version
  digitalWrite(LORA_CS, LOW);
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  SPI.transfer(0x42);
  uint8_t version = SPI.transfer(0x00);
  SPI.endTransaction();
  digitalWrite(LORA_CS, HIGH);
  
  Serial.print("[LoRa] Erreur: ");
  if (version == 0x00 || version == 0xFF) {
    Serial.println("Pas de communication SPI - Vérifier connexions et alimentation");
  } else if (version != 0x12) {
    Serial.print("Version incorrecte (0x");
    if (version < 0x10) Serial.print("0");
    Serial.print(version, HEX);
    Serial.println(") - Module peut être endommagé");
  } else {
    Serial.println("Initialisation échouée - Vérifier fréquence et paramètres");
  }
}

// ============================================================================
// INITIALISATION LoRa
// ============================================================================
bool setupLoRa() {
  Serial.println("[LoRa] Initialisation...");
  Serial.print("  Fréquence: ");
  Serial.print(LORA_BAND / 1E6);
  Serial.println(" MHz");
  
  // Reset matériel du module LoRa
  Serial.println("  Reset matériel...");
  pinMode(LORA_RST, OUTPUT);
  digitalWrite(LORA_RST, LOW);
  delay(50);
  digitalWrite(LORA_RST, HIGH);
  delay(100);
  
  // Initialiser SPI
  Serial.println("  Configuration SPI...");
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
  delay(200);
  
  // Configurer les pins LoRa
  Serial.print("  Pins: CS=");
  Serial.print(LORA_CS);
  Serial.print(", RST=");
  Serial.print(LORA_RST);
  Serial.print(", IRQ=");
  Serial.println(LORA_IRQ);
  LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);
  delay(200);
  
  // Essayer d'initialiser LoRa avec retry
  Serial.println("  Démarrage LoRa...");
  int initResult = 0;
  int retryCount = 5;
  
  for (int i = 0; i < retryCount; i++) {
    initResult = LoRa.begin(LORA_BAND);
    if (initResult == 1) {
      Serial.println("  ✓ LoRa.begin() réussi");
      break;
    } else {
      Serial.print("  ✗ Tentative ");
      Serial.print(i + 1);
      Serial.print("/");
      Serial.print(retryCount);
      Serial.print(" échouée (code: ");
      Serial.print(initResult);
      Serial.println(")");
    }
    
    // Reset matériel avant la prochaine tentative
    if (i < retryCount - 1) {
      digitalWrite(LORA_RST, LOW);
      delay(50);
      digitalWrite(LORA_RST, HIGH);
      delay(300);
    }
  }
  
  if (initResult != 1) {
    Serial.println("[LoRa] ERREUR: Initialisation échouée");
    diagnoseLoRaError();
    loraInitialized = false;
    return false;
  }
  
  // Configurer les paramètres LoRa
  Serial.println("  Configuration paramètres...");
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  Serial.println("    - Spreading Factor: 7");
  Serial.println("    - Bandwidth: 125 kHz");
  Serial.println("    - Coding Rate: 4/5");
  
  Serial.println("[LoRa] ✓ Initialisé avec succès");
  loraInitialized = true;
  return true;
}


// ============================================================================
// SETUP
// ============================================================================
void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  delay(1000);  // Stabilisation alimentation
  
  // Initialiser LoRa
  if (!setupLoRa()) {
    Serial.println("[System] LoRa non disponible - Continuation sans LoRa");
  }
  
  // Initialiser GPS
  setupGPS();
  
  // Initialiser les autres modules
  Serial.println("[System] Initialisation des modules...");
  servoControl.init();
  Serial.println("  ✓ Servos");
  radioReceiver.init();
  Serial.println("  ✓ Radio");
  motorControl.init();
  Serial.println("  ✓ Moteur");
  
  Serial.println("[System] ✓ Système prêt");
  boatMode = "setup-ready";
}

// ============================================================================
// GESTION DES SERVOS (Mode radiocommande ou autonome)
// ============================================================================
void handleServoControl() {
  // Mettre à jour le récepteur radio
  radioReceiver.update();

  // Vérifier les changements de mode (sans messages)
  if (radioReceiver.hasModeChanged()) {
    radioReceiver.resetModeChanged();
  }

  // Si mode radiocommande (SEL=1), COPIE SIGNAL DIRECTE
  if (radioReceiver.isRadioControlMode()) {
    // COPIE SIGNAL DIRECTE : Passer les valeurs PWM en microsecondes
    int sailPWMus = radioReceiver.getPWM1();
    int rudderPWMus = radioReceiver.getPWM2();
    
    servoControl.setAnglesDirect(sailPWMus, rudderPWMus);
  }
  // Si mode autonome (SEL=0), les servos sont contrôlés par la logique de navigation
  // (les fonctions setSailAngle/setRudderAngle seront appelées dans la logique de navigation)
}

// ============================================================================
// VÉRIFICATION ET RÉINITIALISATION LoRa
// ============================================================================
void checkAndReinitLoRa() {
  if (!loraInitialized) {
    return;  // Si LoRa n'est pas initialisé, ne rien faire
  }
  
  // Vérifier périodiquement si LoRa répond encore
  if (millis() - lastLoRaCheck > LORA_CHECK_INTERVAL) {
    lastLoRaCheck = millis();
    
    // Test simple : essayer d'envoyer un paquet de test
    LoRa.beginPacket();
    LoRa.print("TEST");
    int result = LoRa.endPacket();
    
    if (!result) {
      loraFailureCount++;
      
      if (loraFailureCount >= MAX_LORA_FAILURES) {
        Serial.println("[LoRa] ⚠ Échecs détectés - Réinitialisation...");
        loraInitialized = false;
        loraFailureCount = 0;
        
        delay(100);
        if (setupLoRa()) {
          Serial.println("[LoRa] ✓ Réinitialisation réussie");
        } else {
          Serial.println("[LoRa] ✗ Réinitialisation échouée");
        }
      }
    } else {
      if (loraFailureCount > 0) {
        loraFailureCount = 0;
      }
    }
  }
}

// ============================================================================
// BOUCLE PRINCIPALE
// ============================================================================
void loop() {
  // Gérer le contrôle des servos selon le mode
  handleServoControl();

  // Vérifier et réinitialiser LoRa si nécessaire
  checkAndReinitLoRa();

  // Réception LoRa (seulement si initialisé)
  if (loraInitialized && LoRa.parsePacket()) {
    String data = "";
    while (LoRa.available()) {
      data += (char)LoRa.read();
    }
    
    // Afficher le message reçu depuis le serveur
    if (data.length() > 0) {
      Serial.print("[LoRa] ← Reçu: ");
      Serial.println(data);
      
      // Afficher le RSSI et SNR
      Serial.print("  RSSI: ");
      Serial.print(LoRa.packetRssi());
      Serial.print(" dBm, SNR: ");
      Serial.print(LoRa.packetSnr());
      Serial.println(" dB");
      
      parseMessage(data);
    }
  }

  // Debug: réception série
  if (Serial.available() > 0) {
    String data = Serial.readString();
    data.trim();  // Supprimer les espaces
    
    // Parser comme message JSON
    parseMessage(data);
  }

  // Envoi périodique d'informations
  if (millis() >= sendTimer + sendInterval) {
    // Gestion des différents modes
    if (boatMode == "standby") {
      // Mode standby
    } else if (boatMode == "setup-ready") {
      // Système prêt
    } else if (boatMode == "route-ready") {
      // Route chargée, prêt pour navigation
    } else if (boatMode == "wind-observation") {
      handleWindObservation();
    } else if (boatMode == "wind-ready") {
      sendInterval = 2000;  // Réduire la fréquence
    } else if (boatMode == "navigate") {
      handleNavigation();
    }

    // Mise à jour GPS et envoi d'informations
    gpsBoat.upDatePosition();
    
    // Debug GPS périodique
    static unsigned long lastGpsDebug = 0;
    if (millis() - lastGpsDebug > 5000) {  // Toutes les 5 secondes
      lastGpsDebug = millis();
      int gpsStatus = gpsBoat.getStatus();
      if (gpsStatus == 2) {
        Serial.print("[GPS] Position: ");
        Serial.print(gpsBoat.getLat(), 6);
        Serial.print(", ");
        Serial.print(gpsBoat.getLng(), 6);
        Serial.print(" | Cap: ");
        Serial.print(gpsBoat.getSmoothHeading(), 1);
        Serial.print("° | Satellites: ");
        Serial.println(gpsBoat.getSatellites());
      } else {
        Serial.print("[GPS] Statut: ");
        switch(gpsStatus) {
          case 0: Serial.println("Aucun signal"); break;
          case 1: Serial.println("Recherche..."); break;
          default: Serial.println("Inconnu"); break;
        }
      }
    }
    
    // Envoyer des informations périodiquement si LoRa est initialisé
    if (loraInitialized && (boatMode == "setup-ready" || boatMode == "route-ready" || boatMode == "wind-ready" || boatMode == "navigate")) {
      sendInfo();
    }
    
    sendTimer = millis();
  }
}

// ============================================================================
// GESTION OBSERVATION VENT
// ============================================================================
void handleWindObservation() {
  gpsBoat.computeDirectPath(currentWptLat, currentWptLng);

  if (gpsBoat.getDist() >= WIND_DISTANCE) {
    jsonMessage("wind", "info", "wind acquired");
    windDirection = gpsBoat.getSmoothHeading() + 90;
    windDirection = fmod(windDirection + 360.0, 360.0);
    boatMode = "wind-ready";
  }
}

// ============================================================================
// GESTION NAVIGATION
// ============================================================================
void handleNavigation() {
  gpsBoat.computeDirectPath(currentWptLat, currentWptLng);

  double boatHeading = gpsBoat.getSmoothHeading();
  double wptHeading = gpsBoat.getHeading();

  double oppositeWind = oppositeAngle(windDirection);
  double relativeWind = relativeAngle(boatHeading, windDirection);
  double relativeWpt = relativeAngle(boatHeading, wptHeading);

  // Calcul des angles de navigation selon le vent et le waypoint
  if (isBetween(oppositeWind, boatHeading, wptHeading) || isBetween(windDirection, boatHeading, wptHeading)) {
    // VDB - Changement de bord
    if (relativeWind < 0) {
      rudderAngle = 20;
      sailAngle = -10;
      jsonMessage("boat", "info", "VDB relativeWind < 0");
      sendInterval = 300;
    } else {
      rudderAngle = -20;
      sailAngle = 10;
      jsonMessage("boat", "info", "VDB relativeWind > 0");
      sendInterval = 300;
    }
  } else {
    if (sameSign(relativeWind, relativeWpt)) {
      // Lofer
      if (relativeWind < 0) {
        sailAngle = -10;
        rudderAngle += 5;
        sendInterval = 2000;
        jsonMessage("boat", "info", "Lofer relativeWind < 0");
      } else {
        rudderAngle -= 5;
        sailAngle = 10;
        sendInterval = 2000;
        jsonMessage("boat", "info", "Lofer relativeWind > 0");
      }
    } else {
      // Abattre
      if (relativeWind < 0) {
        sailAngle = -10;
        rudderAngle -= 5;
        sendInterval = 2000;
        jsonMessage("boat", "info", "Abattre relativeWind < 0");
      } else {
        rudderAngle += 5;
        sailAngle = 10;
        sendInterval = 2000;
        jsonMessage("boat", "info", "Abattre relativeWind > 0");
      }
    }
  }

  // Appliquer les angles seulement si en mode autonome
  if (!radioReceiver.isRadioControlMode()) {
    servoControl.setRudderAngle(rudderAngle);
    servoControl.setSailAngle(sailAngle);
  }

  // Vérifier l'atteinte du waypoint
  if (gpsBoat.getDist() <= WAYPOINT_DISTANCE) {
    extractNextWaypoint();
  }
}

// ============================================================================
// PARSING DES MESSAGES
// ============================================================================
void parseMessage(String data) {
  if (data.indexOf("origin\":\"server") != -1) {
    if (data.indexOf("type\":\"command") != -1) {
      String message = data.substring(data.indexOf("message\":") + 9);

      // Réception waypoints
      if (message.indexOf("waypoints") != -1) {
        handleWaypoints(message);
      }

      // Commande observation vent
      if (message.indexOf("wind-observation") != -1) {
        if (boatMode == "route-ready") {
          currentWptLat = gpsBoat.getLat();
          currentWptLng = gpsBoat.getLng();
          boatMode = "wind-observation";
          sailAngle = 10;
          rudderAngle = -20;
          if (!radioReceiver.isRadioControlMode()) {
            servoControl.setSailAngle(sailAngle);
            servoControl.setRudderAngle(rudderAngle);
          }
        }
      }

      // Commande direction vent
      if (message.indexOf("wind-command") != -1) {
        if (boatMode == "route-ready") {
          windDirection = message.substring(message.indexOf("value\":") + 8, message.lastIndexOf("\"")).toInt();
          boatMode = "wind-ready";
        }
      }

      // Commande navigation
      if (message.indexOf("navigate") != -1) {
        if (boatMode == "wind-ready") {
          waypointId = -1;
          extractNextWaypoint();
          boatMode = "navigate";
        } else {
          jsonMessage("navigation", "error", "Not wind-ready");
        }
      }

      // Commande redémarrage
      if (message.indexOf("restart") != -1) {
        ESP.restart();
      }
    }
  }
}

// ============================================================================
// GESTION WAYPOINTS
// ============================================================================
void handleWaypoints(String message) {
  String waypointsTmp = message.substring(message.lastIndexOf("points\":") + 9, message.lastIndexOf("\""));
  int waypointsNumber = message.substring(message.indexOf("number\":") + 8, message.indexOf(",")).toInt();

  // Compter les coordonnées
  int i, count;
  for (i = 0, count = 0; waypointsTmp[i]; i++) {
    count += (waypointsTmp[i] == '.');
  }

  if (count / waypointsNumber == 2) {
    waypoints = waypointsTmp;
    waypointCount = waypointsNumber;
    jsonMessage("waypoints", "info", "Ready");
    boatMode = "route-ready";
  } else {
    jsonMessage("waypoints", "error", "Reception failed");
  }
}

// ============================================================================
// EXTRACTION WAYPOINT
// ============================================================================
void extractNextWaypoint() {
  if (waypointId + 1 < waypointCount) {
    waypointId++;
  } else {
    waypointId--;
  }

  int lastCoordIndex = 0;
  for (int i = 0; i < (waypointId + 1); i++) {
    int endLngIndex = waypoints.indexOf(",", lastCoordIndex);
    currentWptLng = waypoints.substring(lastCoordIndex, endLngIndex).toDouble();

    int endLatIndex = waypoints.indexOf(",", endLngIndex + 1);
    currentWptLat = waypoints.substring(endLngIndex + 1, endLatIndex).toDouble();

    lastCoordIndex = endLatIndex + 1;
  }
}

// ============================================================================
// ENVOI D'INFORMATIONS
// ============================================================================
void sendInfo() {
  String message = "{";
  message += "\"mode\":\"" + boatMode + "\",";
  message += "\"location\":[" + String(gpsBoat.getLat(), 10) + "," + String(gpsBoat.getLng(), 10) + "],";
  message += "\"servos\":{\"sail\":" + String(int(servoControl.getSailAngle())) + ",\"rudder\":" + String(int(servoControl.getRudderAngle())) + "},";
  message += "\"control_mode\":\"" + String(radioReceiver.isRadioControlMode() ? "radio" : "autonomous") + "\",";
  message += "\"heading\":" + String(int(gpsBoat.getSmoothHeading())) + ",";
  message += "\"wind\":" + String(int(windDirection));

  if (waypointCount > 0) {
    message += ",\"waypoints\":{\"total\":" + String(waypointCount) + ",\"current\":" + waypointId + "}";
  }

  message += "}";

  String jsonFormattedMessage = "{\"origin\":\"boat\",\"type\":\"info\",\"message\":" + message + "}";
  
  // Envoyer via LoRa seulement si initialisé
  if (!loraInitialized) {
    return;
  }
  
  // Debug: afficher ce qui est envoyé
  Serial.print("[LoRa] → Envoi: ");
  Serial.println(jsonFormattedMessage);
  
  LoRa.beginPacket();
  LoRa.print(jsonFormattedMessage);
  int result = LoRa.endPacket();
  
  if (result) {
    Serial.println("  ✓ Message envoyé");
  } else {
    Serial.println("  ✗ Erreur envoi");
  }
}

// ============================================================================
// MESSAGES JSON
// ============================================================================
void jsonMessage(String origin, String type, String message) {
  // MODE MANUEL : Pas de messages pour performance maximale
}

// ============================================================================
// FONCTIONS MATHÉMATIQUES NAVIGATION
// ============================================================================
double relativeAngle(double reference, double target) {
  double angle = target - reference;
  if (angle > 180) angle -= 360;
  else if (angle < -180) angle += 360;
  return angle;
}

bool sameSign(double angle1, double angle2) {
  return (angle1 >= 0 && angle2 >= 0) || (angle1 < 0 && angle2 < 0);
}

double oppositeAngle(double angle) {
  double opposite = angle + 180;
  if (opposite >= 360) opposite -= 360;
  return opposite;
}

bool isBetween(double angle, double start, double end) {
  double relativeAngleStart = relativeAngle(angle, start);
  double relativeAngleEnd = relativeAngle(angle, end);
  return !sameSign(relativeAngleStart, relativeAngleEnd);
}
