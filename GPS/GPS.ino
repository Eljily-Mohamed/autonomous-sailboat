
#include "Gps.hpp"

GPS gpsBoat;


void setupGPS() {
  gpsBoat.init();

  while (gpsBoat.getStatus() != 2) {
    gpsBoat.upDatePosition();
    Serial.println("Locating in progress");
    delay(500);
  }
}

void setup() {
  Serial.begin(115200);
  setupGPS();
}

void loop() {
  Serial.print("Satellites: ");
  Serial.println(gpsBoat.getSatellites());
  Serial.print("Latitude  : ");
  Serial.println(gpsBoat.getLat(), 10);
  Serial.print("Longitude : ");
  Serial.println(gpsBoat.getLng(), 10);
  Serial.print("cap actuel : ");
  Serial.println(gpsBoat.getCap());
  gpsBoat.upDatePosition();
  delay(1000);
}