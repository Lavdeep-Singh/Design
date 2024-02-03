#ifndef STRATEGY_MANAGER
#define STRATEGY_MANAGER

#include <iostream>
#include "TripMetadata.cpp"
#include "PricingStrategy.cpp"
#include "DriverMatchingStrategy.cpp"
#include "Common.cpp"
#include "mutex"

using namespace std;

class StrategyMgr {
	StrategyMgr() {}
	static StrategyMgr* strategyMgrInstance;
	static mutex mtx;

public:
	static StrategyMgr* getStrategyMgr(){
        if (strategyMgrInstance == nullptr) {
            mtx.lock();
            if (strategyMgrInstance == nullptr) {
                strategyMgrInstance = new StrategyMgr();
            }
            mtx.unlock();
        }
        return strategyMgrInstance;
    }
	PricingStrategy* determinePricingStrategy(TripMetaData* metaData) {
        cout << "Based on location and other factors, setting pricing strategy" << endl;
        return new DefaultPricingStrategy();
    }

    DriverMatchingStrategy* determineMatchingStrategy(TripMetaData* metaData) {
        cout << "Based on location and other factors, setting matching strategy" << endl;
        return new LeastTimeBasedMatchingStrategy();
    }
};

StrategyMgr* StrategyMgr::strategyMgrInstance;
mutex StrategyMgr :: mtx;

#endif