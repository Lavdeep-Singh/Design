#ifndef DRIVER_MATCHING_STRATEGY
#define DRIVER_MATCHING_STRATEGY

#include <iostream>
#include "Driver.cpp"
#include "TripMetadata.cpp"
#include "DriverManager.cpp"
using namespace std;

// driver matching strategy interface
class DriverMatchingStrategy {
    public:
	virtual Driver* matchDriver(TripMetaData* pTripMetaData) = 0;
};

//implementing driver matching strategy with least time based driver matching strategy
class LeastTimeBasedMatchingStrategy : public DriverMatchingStrategy{
    public:
    Driver* matchDriver(TripMetaData* pTripMetaData){
        DriverMgr* driverMgr = DriverMgr::getDriverMgr();

        if(driverMgr->getDriversMap().size()==0){
            cout<<"No divers available!!!"<<endl;
        }
        cout << "Using quadtree to see nearest cabs, using driver manager to get details of drivers and send notifications" << endl;
		Driver* driver = ((driverMgr->getDriversMap()).begin())->second; //here we can call quadtree algo to get nearest cabs
		cout << "Setting " << driver->getDriverName() << " as driver" << endl;
		pTripMetaData->setDriverRating(driver->getRating());
		return driver;
    }
};

#endif