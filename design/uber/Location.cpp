#ifndef LOCATION_H
#define LOCATION_H

#include <iostream>

using namespace std;

class Location {
public:
	double latitude;
	double longitude;
	Location(double pLatitude, double pLongitude) : latitude(pLatitude), longitude(pLongitude) {}
};

#endif