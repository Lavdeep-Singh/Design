#ifndef DRIVER_MANAGER
#define DRIVER_MANAGER

#include <iostream>
#include <unordered_map>
#include "Driver.cpp"
#include "mutex"
using namespace std;

//singelton
class DriverMgr {
	DriverMgr(){}
	static DriverMgr* driverMgrInstance;
	static mutex mtx;
	unordered_map<string, Driver*> driversMap;

    public:
	static DriverMgr* getDriverMgr(){
        if (driverMgrInstance == nullptr) {
            mtx.lock();
            if (driverMgrInstance == nullptr) {
                driverMgrInstance = new DriverMgr();
            }
            mtx.unlock();
        }
        return driverMgrInstance;
    }
	void addDriver(string pDriverName, Driver * pDriver) {
	    driversMap[pDriverName] = pDriver;
    }
	Driver* getDriver(string pDriverName) {
	    return driversMap[pDriverName];
    }
	unordered_map<string, Driver*> getDriversMap() {
	    return driversMap;
    }
};

DriverMgr* DriverMgr::driverMgrInstance;
mutex DriverMgr::mtx;

#endif

