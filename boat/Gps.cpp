#include "Gps.hpp"


#define UBLOX_GPS_OBJECT() TinyGPSPlus gps
UBLOX_GPS_OBJECT();


#define POSITION_HISTORY_LEN 5

double __latHistory[POSITION_HISTORY_LEN]; //Historique des positions 5
double __lngHistory[POSITION_HISTORY_LEN];
int __positionShiftIndex = 0;
int positionCount = 0; // Compteur pour suivre le nombre de positions ajoutées
double historique_heading = 0;


uint64_t gpsSec = 0;

void GPS::init() {
  AXP20X_Class axp;
  //Power GPS
  axp.setPowerOutPut(AXP192_LDO3, AXP202_ON);
  axp.setLDO3Voltage(3300);  //GPS VDD      3v3
  
  Serial1.begin(GPS_BAUD_RATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
}
void GPS::upDatePosition() {
  static uint64_t gpsMap = 0;

  while (Serial1.available())
    gps.encode(Serial1.read());

  if (millis() > 5000 && gps.charsProcessed() < 10) {
    __status = 0;
    return;
  }
  
  if (!gps.location.isValid()) {
    if (millis() - gpsMap > 1000) {
      __status = 1;
      gpsMap = millis();
    }
  } else {
    if (millis() - gpsMap > 1000) {
      __prevLng = __locLng;
      __prevLat = __locLat;

      __status = 2;
      __locLng = gps.location.lng();
      __locLat = gps.location.lat();
      __satellites = gps.satellites.value();

      // Calcul de la position lissée
      double smoothLat, smoothLng;
      getSmoothPosition(smoothLat, smoothLng);

      // Ajout de la position dans l'historique
      addPositionEntry(__locLat, __locLng);

      // Utilisation des positions lissées pour calculer la direction
      computeDirectPath(smoothLat, smoothLng);

      // Ajout de la direction dans l'historique
      addHeadingEntry(__heading);

      // Debugage NBRE_satellites
      int NBR_satellite = getSatellites();
      Serial.println("Nombre de GPS " + String(NBR_satellite));

      gpsMap = millis();
    }
  }
}

void GPS::computeDirectPath(double locLatObj, double locLngObj) {
  if (__status == 2) {
    // Utilisation des positions lissées pour calculer la direction
    __distance = TinyGPSPlus::distanceBetween(locLatObj, locLngObj, __locLat, __locLng);
    __heading = TinyGPSPlus::courseTo(locLatObj, locLngObj, __locLat, __locLng);
  }
}

void GPS::displayGps() {
  char buff[5][256];
  if (getStatus() == 0) {
    Serial.println("No GPS detected");
  }
  if (getStatus() == 1) {
    Serial.println("Locating ...");
  }
  if (getStatus() == 2) {
    snprintf(buff[0], sizeof(buff[1]), "LNG:%.4f", getLng());
    snprintf(buff[1], sizeof(buff[2]), "LAT:%.4f", getLat());
    snprintf(buff[2], sizeof(buff[3]), "satellites:%u", getSatellites());
    Serial.println(buff[0]);
    Serial.println(buff[1]);
    Serial.println(buff[2]);
  }
}


double GPS::getLng() {
  if (__status == 2) {
  if (POSITION_HISTORY_LEN == 5 && positionCount == 0)
  {
    __locLng = gps.location.lng();
    return __locLng;
  }
  if (positionCount != 0)
  {
    double smoothLat, smoothLng;
    getSmoothPosition(smoothLat, smoothLng); // Calcul des positions moyennes et filtrées
    return smoothLng; // Retourne la latitude lissée
  }
  } else {
    return 0;
  }
}


double GPS::getLat() {
  if (__status == 2) {
    if (POSITION_HISTORY_LEN == 5 && positionCount == 0)
    {
      __locLat = gps.location.lat();
      return __locLat;
    }
  if (positionCount != 0)
  {
    double smoothLat, smoothLng;
    getSmoothPosition(smoothLat, smoothLng); // Calcul des positions moyennes et filtrées
    return smoothLat; // Retourne la latitude lissée
  }
  } else {
    return 0;
  }
}


double GPS::getDist() {
  if (__status == 2) {
    return __distance;
  } else {
    return 0;
  }
}

double GPS::getHeading() {
  if (__status == 2) {
    return __heading;
  } else {
    return 0;
  }
}

/*
double GPS::getSmoothHeading() {
  if (__status == 2) {
    double smoothLat, smoothLng;
    getSmoothPosition(smoothLat, smoothLng); // Récupère la position lissée actuelle
    

    // Récupère l'ancienne position lissée dans l'historique
    int lastIndex = (__positionShiftIndex - 2 + POSITION_HISTORY_LEN) % POSITION_HISTORY_LEN; // Avant-dernière position enregistrée
    double prevLat = __latHistory[lastIndex];
    double prevLng = __lngHistory[lastIndex];

    // Calcul de la distance entre la position lissée récente et l'ancienne position lissée
    double distance = TinyGPSPlus::distanceBetween(prevLat, prevLng, smoothLat, smoothLng);

    if (distance >= 2) {
      // Si la distance est suffisante, calculer l'orientation
      double heading = TinyGPSPlus::courseTo(prevLat, prevLng, smoothLat, smoothLng);
      historique_heading = heading;

      return heading;

    } else {
      // Si la distance est insuffisante, retourner une orientation par défaut ou celle de l'historique
      //Serial.println("Distance insuffisante (< 2m). Orientation basée sur l'ancienne position.");
      return historique_heading;
    }
  } else {
    return 0;
  }
}

*/

// Ancienne méthode de calcul du heading avec position brut 

double GPS::getSmoothHeading() {
  if (__status == 2) {
    float x = 0;
    float y = 0;
    int totalWeight = 0;

    for (int i = 0; i < HEADING_HISTORY_LEN; i++) {
      // Pondération décroissante (le plus récent a un poids plus élevé)
      int weight = HEADING_HISTORY_LEN - i;
      int index = (__headingShiftIndex - 1 - i + HEADING_HISTORY_LEN) % HEADING_HISTORY_LEN;

      // Convertir les caps en radians et calculer les composantes pondérées
      float radians = __headingHistory[index] * PI / 180.0;
      x += cos(radians) * weight;
      y += sin(radians) * weight;
      totalWeight += weight;
    }

    // Calculer le cap moyen pondéré en radians
    float averageRadians = atan2(y / totalWeight, x / totalWeight);

    // Convertir le cap moyen en degrés
    float averageHeading = averageRadians * 180.0 / PI;

    // Ramener à l'intervalle [0°, 360°]
    averageHeading = fmod(averageHeading + 360.0, 360.0);

    return averageHeading;

  } else {
    return 0;
  }
}

int GPS::getStatus() {
  return __status;
}

int GPS::getSatellites() {
  return __satellites;
}

void GPS::addHeadingEntry(double heading) {
  __headingHistory[__headingShiftIndex] = heading;
  __headingShiftIndex = (__headingShiftIndex + 1) % HEADING_HISTORY_LEN;
}


void GPS::addPositionEntry(double lat, double lng) {
 
  // Si c'est la première position brute, l'ajouter directement
  if (positionCount == 0) {
    lat = gps.location.lat();
    lng = gps.location.lng();
    __latHistory[__positionShiftIndex] = lat;
    __lngHistory[__positionShiftIndex] = lng;
    __positionShiftIndex = (__positionShiftIndex + 1) % POSITION_HISTORY_LEN;
    positionCount++;
    return;
  }

  // Calcul de la distance entre la nouvelle position et la dernière position enregistrée
  if (__positionShiftIndex > 0 || __latHistory[0] != 0 || __lngHistory[0] != 0) {
    double distance = TinyGPSPlus::distanceBetween(
      __latHistory[(__positionShiftIndex - 1 + POSITION_HISTORY_LEN) % POSITION_HISTORY_LEN],
      __lngHistory[(__positionShiftIndex - 1 + POSITION_HISTORY_LEN) % POSITION_HISTORY_LEN],
      lat,
      lng
    );

    // Conditions de filtrage / Si la position dépasse des sauts abberants on passe directement l'ancienne valeur
    if (distance > 25) {  
      Serial.println("Position aberrante ignorée (distance > 25m).");
      lat = __latHistory[(__positionShiftIndex - 1 + POSITION_HISTORY_LEN) % POSITION_HISTORY_LEN];
      lng = __lngHistory[(__positionShiftIndex - 1 + POSITION_HISTORY_LEN) % POSITION_HISTORY_LEN];
    } else if (distance < 3) {
      Serial.println("Position ignorée (distance < 3m).");
      lat = __latHistory[(__positionShiftIndex - 1 + POSITION_HISTORY_LEN) % POSITION_HISTORY_LEN];
      lng = __lngHistory[(__positionShiftIndex - 1 + POSITION_HISTORY_LEN) % POSITION_HISTORY_LEN];
    }
  }
    // Ajouter la position filtrée/lissée à l'historique
  __latHistory[__positionShiftIndex] = lat;
  __lngHistory[__positionShiftIndex] = lng;
  __positionShiftIndex = (__positionShiftIndex + 1) % POSITION_HISTORY_LEN;
  positionCount++;


  // Si 30 positions ont été ajoutées, réinitialiser la liste et prochaine boucle 1er valeur brut
  if (positionCount >= 30) {
    Serial.println("Réinitialisation de l'historique après 30 positions.");
    positionCount = 0; // Réinitialisation du compteur
    __positionShiftIndex = 0; // Réinitialisation de l'index de l'historique
    memset(__latHistory, 0, sizeof(__latHistory)); // Vider l'historique des latitudes
    memset(__lngHistory, 0, sizeof(__lngHistory)); // Vider l'historique des longitudes
    lat = gps.location.lat();
    lng = gps.location.lng();
    __latHistory[__positionShiftIndex] = lat;
    __lngHistory[__positionShiftIndex] = lng;
    __positionShiftIndex = (__positionShiftIndex + 1) % POSITION_HISTORY_LEN;
    positionCount++;
  }
}


// Calcul de la position moyennée pondérée
void GPS::getSmoothPosition(double &smoothLat, double &smoothLng) {
  double weightedSumLat = 0;
  double weightedSumLng = 0;
  int totalWeight = 0;

  // Calculer le nombre de positions réellement valides dans l'historique
  // On prends la valeur plus petite entre le nombre d'éléments max dans la liste et le count
  int validEntries = std::min(positionCount, POSITION_HISTORY_LEN);

  // Boucler uniquement sur les entrées valides
  for (int i = 0; i < validEntries; i++) {
    int weight = validEntries - i; // Pondération décroissante (plus récent a un poids plus élevé)
    int index = (__positionShiftIndex - 1 - i + POSITION_HISTORY_LEN) % POSITION_HISTORY_LEN;

    weightedSumLat += __latHistory[index] * weight; // Attribuer une pondération dans le moyennage
    weightedSumLng += __lngHistory[index] * weight;
    totalWeight += weight;
  }

  // Éviter une division par zéro
  if (totalWeight > 0) {
    smoothLat = weightedSumLat / totalWeight;
    smoothLng = weightedSumLng / totalWeight;
  } else {
    smoothLat = 0;
    smoothLng = 0;
    Serial.println("Pas assez de données valides pour calculer une position lissée.");
  }
}
