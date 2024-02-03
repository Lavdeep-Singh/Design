#ifndef RIDER_MANAGER
#define RIDER_MANAGER

#include <iostream>
#include <unordered_map>
#include "Rider.cpp"

using namespace std;

class RiderMgr {
	RiderMgr(){}
	static RiderMgr* riderMgrInstance;
	static mutex mtx;
	unordered_map<string, Rider*> ridersMap;
public:
	static RiderMgr* getRiderMgr(){
        // if (riderMgrInstance == nullptr) {
        //     mtx.lock();
            if (riderMgrInstance == nullptr) {
                riderMgrInstance = new RiderMgr();
            }
        //     mtx.unlock();
        // }
        return riderMgrInstance;
    }
	void addRider(string pRiderName, Rider* pRider) {
        ridersMap[pRiderName] = pRider; 
    }
    Rider* getRider(string pRiderName) {
        return ridersMap[pRiderName];
    }
};

RiderMgr* RiderMgr::riderMgrInstance;

#endif