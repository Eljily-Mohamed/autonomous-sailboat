
#include <LiquidCrystal_I2C.h>

#include "Gps.hpp"

#define _GPS_EARTH_MEAN_RADIUS 6371009

GPS gpsBoat;

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define MAX_HISTORY 5
double locationHistory[MAX_HISTORY][2];
int locationShiftIndex = 0;


double prevLat;
double prevLng;

double DISTANCE_THRESHOLD = 1;

int loopCount = 0;

void setupLCD() {
  lcd.init();
  lcd.backlight();
}

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
  setupLCD();

  lcd.setCursor(0, 0);
  lcd.print("Locating...");

  setupGPS();

  lcd.clear();
  lcd.print("Done !");

  prevLat = gpsBoat.getLat();
  prevLng = gpsBoat.getLng();

  delay(1000);
}

void loop() {
  loopCount++;

  gpsBoat.upDatePosition();

  double heading = gpsBoat.getHeading();//courseTo(prevLat, prevLng, gpsBoat.getLat(), gpsBoat.getLng());
  double distance = gpsBoat.getDist(); //distanceBetween(prevLat, prevLng, gpsBoat.getLat(), gpsBoat.getLng());


  lcd.clear();
  lcd.print("Sat:");
  lcd.print(gpsBoat.getSatellites());
  lcd.print(" Head:");
  lcd.print(heading);

  lcd.setCursor(0, 1);
  lcd.print("Dist:");
  lcd.print(distance);

  //if (loopCount > 5) {
    lcd.print(" ");
    lcd.print(gpsBoat.getSmoothHeading());

    Serial.println(gpsBoat.getSmoothHeading());
  //}
  Serial.print("Satellites: ");
  Serial.println(gpsBoat.getSatellites());
  Serial.print("Latitude  : ");
  Serial.println(gpsBoat.getLat(), 10);
  Serial.print("Longitude : ");
  Serial.println(gpsBoat.getLng(), 10);

  Serial.print("Prev Latitude  : ");
  Serial.println(prevLat, 10);
  Serial.print("Prev Longitude : ");
  Serial.println(prevLng, 10);

  Serial.print("Heading : ");
  Serial.println(heading);


  Serial.print("Distance : ");
  Serial.println(distance);

  prevLat = gpsBoat.getLat();
  prevLng = gpsBoat.getLng();

  //addLocationEntry(gpsBoat.getLat(), gpsBoat.getLng());

  delay(1000);
}

double courseTo(double lat1, double long1, double lat2, double long2) {
  // returns course in degrees (North=0, West=270) from position 1 to position 2,
  // both specified as signed decimal-degrees latitude and longitude.
  // Because Earth is no exact sphere, calculated course may be off by a tiny fraction.
  // Courtesy of Maarten Lamers
  double dlon = radians(long2 - long1);
  lat1 = radians(lat1);
  lat2 = radians(lat2);
  double a1 = sin(dlon) * cos(lat2);
  double a2 = sin(lat1) * cos(lat2) * cos(dlon);
  a2 = cos(lat1) * sin(lat2) - a2;
  a2 = atan2(a1, a2);
  if (a2 < 0.0) {
    a2 += TWO_PI;
  }
  return degrees(a2);
}


double distanceBetween(double lat1, double long1, double lat2, double long2) {
  // returns distance in meters between two positions, both specified
  // as signed decimal-degrees latitude and longitude. Uses great-circle
  // distance computation for hypothetical sphere of radius 6371009 meters.
  // Because Earth is no exact sphere, rounding errors may be up to 0.5%.
  // Courtesy of Maarten Lamers
  double delta = radians(long1 - long2);
  double sdlong = sin(delta);
  double cdlong = cos(delta);
  lat1 = radians(lat1);
  lat2 = radians(lat2);
  double slat1 = sin(lat1);
  double clat1 = cos(lat1);
  double slat2 = sin(lat2);
  double clat2 = cos(lat2);
  delta = (clat1 * slat2) - (slat1 * clat2 * cdlong);
  delta = sq(delta);
  delta += sq(clat2 * sdlong);
  delta = sqrt(delta);
  double denom = (slat1 * slat2) + (clat1 * clat2 * cdlong);
  delta = atan2(delta, denom);
  return delta * _GPS_EARTH_MEAN_RADIUS;
}


void addLocationEntry(double lat, double lng) {
  locationHistory[locationShiftIndex][0] = lat;
  locationHistory[locationShiftIndex][1] = lng;

  locationShiftIndex = (locationShiftIndex + 1) % MAX_HISTORY;
}