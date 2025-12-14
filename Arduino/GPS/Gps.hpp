#ifndef GPS_HPP
#define GPS_HPP

#include <TinyGPS++.h>
#include <axp20x.h>

#define GPS_BAUD_RATE 9600
#define GPS_RX_PIN 34
#define GPS_TX_PIN 12

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

    //getters
    double getLng();
    double getLat();
    double getDist();
    double getCap();
    int getStatus();
    int getSatellites();

    
  private:
    double __locLng;
    double __locLat;
    double __distance;
    double __cap;
    int __satellites;
    int __status; //0: noGPS / 1: locating / 2: location valid
  
};


#endif //GPS_HPP
