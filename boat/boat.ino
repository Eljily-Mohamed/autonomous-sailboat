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
  // jsonMessage("GPS", "info", "Initializing");
  // gpsBoat.init();

  // int retryDelay = 500;
  // int failCount = 30000 / retryDelay;  // Timeout 30 secondes (réduit pour tests)

  // while (gpsBoat.getStatus() != 2 && failCount > 0) {
  //   gpsBoat.upDatePosition();
  //   failCount--;
  //   delay(retryDelay);
  // }

  // if (failCount == 0) {
  //   jsonMessage("GPS", "error", "Setup failed - Continuing without GPS");
  //   jsonMessage("GPS", "warning", "GPS will retry in background");
  //   // NE PAS BLOQUER - Continuer sans GPS pour permettre les tests
  // } else {
  //   jsonMessage("GPS", "info", "Ready");
  // }
}

// ============================================================================
// INITIALISATION LoRa
// ============================================================================
void setupLoRa() {
  // LoRa silencieux pour mode manuel
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
  LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);
  LoRa.begin(LORA_BAND);  // Pas de vérification, continue quoi qu'il arrive
}

// ============================================================================
// SETUP
// ============================================================================
void setup() {
  // Serial.begin(SERIAL_BAUD_RATE);  // Désactivé pour mode manuel pur
  delay(500);

  // MODE MANUEL FOCUS : Pas de messages, juste l'initialisation
  setupLoRa();
  // setupGPS();  // Désactivé pour se concentrer sur le mode manuel
  
  servoControl.init();
  radioReceiver.init();
  motorControl.init();

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
    
    // Debug pour vérifier la copie (toutes les 500ms)
    // static unsigned long lastDebug = 0;
    // if (millis() - lastDebug > 500) {
    //   Serial.print("SIGNAL COPY: Input Sail=");
    //   Serial.print(sailPWMus);
    //   Serial.print("µs, Rudder=");
    //   Serial.print(rudderPWMus);
    //   Serial.println("µs");
    //   lastDebug = millis();
    // }
  }
  // Si mode autonome (SEL=0), les servos sont contrôlés par la logique de navigation
  // (les fonctions setSailAngle/setRudderAngle seront appelées dans la logique de navigation)
}

// ============================================================================
// BOUCLE PRINCIPALE
// ============================================================================
void loop() {
  // Gérer le contrôle des servos selon le mode
  handleServoControl();

  // Réception LoRa
  if (LoRa.parsePacket()) {
    while (LoRa.available()) {
      String data = LoRa.readString();
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

    // Mise à jour GPS et envoi d'informations (désactivé pour mode manuel)
    // gpsBoat.upDatePosition();
    // sendInfo();
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
  // Serial.println(jsonFormattedMessage);  // Désactivé pour mode manuel

  LoRa.beginPacket();
  LoRa.print(jsonFormattedMessage);
  LoRa.endPacket();
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
