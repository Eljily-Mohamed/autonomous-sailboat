#include "Gps.hpp"


#define UBLOX_GPS_OBJECT()  TinyGPSPlus gps
UBLOX_GPS_OBJECT();

uint64_t gpsSec = 0;

void GPS::init(){
  AXP20X_Class axp;
  //Power GPS
  axp.setPowerOutPut(AXP192_LDO3, AXP202_ON);
  axp.setLDO3Voltage(3300);   //GPS VDD      3v3

  Serial1.begin(GPS_BAUD_RATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
}

void GPS::upDatePosition(){
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
        __status = 2;
        __locLng = gps.location.lng();
        __locLat = gps.location.lat();
        __satellites = gps.satellites.value();

        gpsMap = millis();
    }
  }
}

void GPS::computeDirectPath(double locLatObj, double locLngObj){
  if(__status == 2){
    __distance = TinyGPSPlus::distanceBetween(__locLat, __locLng, locLatObj, locLngObj);
    __cap = TinyGPSPlus::courseTo(__locLat, __locLng, locLatObj, locLngObj);
  }
}

void GPS::displayGps(){
  char buff[5][256];
  if(getStatus() == 0){
      Serial.println("No GPS detected");
  }
  if(getStatus() == 1){
      Serial.println("Locating ...");
  }
  if(getStatus() == 2){
      snprintf(buff[0], sizeof(buff[1]), "LNG:%.4f", getLng());
      snprintf(buff[1], sizeof(buff[2]), "LAT:%.4f", getLat());
      snprintf(buff[2], sizeof(buff[3]), "satellites:%u", getSatellites());
      Serial.println(buff[0]);
      Serial.println(buff[1]);
      Serial.println(buff[2]);
  }
}

double GPS::getLng(){
  if (__status == 2){
    return __locLng;
  }
  else{
    return 0;
  }
}

double GPS::getLat(){
  if(__status == 2){
    return __locLat;
  }
  else{
    return 0;
  }
}

double GPS::getDist(){
  if(__status == 2){
    return __distance;
  }
  else{
    return 0;
  }
}

double GPS::getCap(){
  if(__status == 2){
    return __cap;
  }
  else{
    return 0;
  }
}

int GPS::getStatus(){
  return __status;
}

int GPS::getSatellites(){
  return __satellites;
}
