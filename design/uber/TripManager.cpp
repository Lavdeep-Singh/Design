#ifndef TRIP_MANAGER
#define TRIP_MANAGER

#include <iostream>
#include "RiderManager.cpp"
#include "DriverManager.cpp"
#include "TripMetadata.cpp"
#include "Trip.cpp"
#include "mutex"
#include "StrategyManager.cpp"
using namespace std;

class TripMgr {
	static TripMgr* tripMgrInstance;
	static mutex mtx;
	TripMgr(){
		riderMgr = RiderMgr::getRiderMgr();
		driverMgr = DriverMgr::getDriverMgr();
	}
	RiderMgr* riderMgr;
	DriverMgr* driverMgr;
	unordered_map<int, TripMetaData*> tripsMetaDataInfo; 
	unordered_map<int, Trip*> tripsInfo;
public:
	static TripMgr* getTripMgr(){
        if (tripMgrInstance == nullptr) {
            mtx.lock();
            if (tripMgrInstance == nullptr) {
                tripMgrInstance = new TripMgr();
            }
            mtx.unlock();
        }
        return tripMgrInstance;
    }

	void CreateTrip(Rider* pRider, Location* pSrcLoc, Location* pDstLoc) {
	
        TripMetaData* metaData = new TripMetaData(pSrcLoc, pDstLoc, pRider->getRating());
        StrategyMgr* strategyMgr = StrategyMgr::getStrategyMgr();
        PricingStrategy* pricingStrategy = strategyMgr->determinePricingStrategy(metaData);
        DriverMatchingStrategy* driverMatchingStrategy = strategyMgr->determineMatchingStrategy(metaData);
        
        Driver* driver = driverMatchingStrategy->matchDriver(metaData);
        double tripPrice = pricingStrategy->calculatePrice(metaData);

        Trip* trip = new Trip(pRider, driver, pSrcLoc, pDstLoc, tripPrice, pricingStrategy, driverMatchingStrategy);
        int tripId = trip->getTripId();
        tripsInfo[tripId] = trip;
        tripsMetaDataInfo[tripId] = metaData;
    }

    unordered_map<int, Trip*> getTripsMap() {
        return tripsInfo;
    }
};

TripMgr* TripMgr::tripMgrInstance;
mutex TripMgr :: mtx;

#endif