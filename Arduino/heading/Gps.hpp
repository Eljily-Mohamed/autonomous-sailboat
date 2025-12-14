#ifndef GPS_HPP
#define GPS_HPP

#include <TinyGPS++.h>
#include <axp20x.h>

#define GPS_BAUD_RATE 9600
#define GPS_RX_PIN 34
#define GPS_TX_PIN 12

#define HEADING_HISTORY_LEN 5

class GPS{
  public:
    //initializer
    void init();

    //ask the position to the gps via serial link
    void upDatePosition();

    //compute the distance and cap to the next waypoint
    void computeDirectPath(double LocLatObj, double LocLngObj);

    //To display the position and more via serial link
    void displayGps();

    void addHeadingEntry(double heading);

    //getters
    double getLng();
    double getLat();
    double getDist();
    double getHeading();
    int getStatus();
    int getSatellites();
    double getSmoothHeading();

    
  private:
    double __locLng;
    double __locLat;
    double __distance;
    double __heading;
    int __satellites;
    int __status; //0: noGPS / 1: locating / 2: location valid
    double __headingHistory[HEADING_HISTORY_LEN];
    int __headingShiftIndex = 0;

    double __prevLng;
    double __prevLat;
  
};


#endif //GPS_HPP
