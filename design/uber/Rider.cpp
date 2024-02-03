#ifndef RIDER_H
#define RIDER_H

#include <iostream>
#include "Common.cpp"

using namespace std;

class Rider {
	string name;
	RATING rating;
public:
	Rider(string pName, RATING pRating) : name(pName), rating(pRating) {}
	string getRiderName() {
		return name;
	}
	RATING getRating() {
		return rating;
	}
};

#endif // COMMON_H