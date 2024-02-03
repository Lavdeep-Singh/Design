#include <iostream>
#include <unordered_map>
#include <set>
#include <queue>
using namespace std;

class Car {
    string color;
    string registrationNumber;

    public:

    Car(string pColor, string pRegistrationNumber){
        color = pColor;
        registrationNumber = pRegistrationNumber;
    }

    string getRegistrationNumber(){
        return registrationNumber;
    }

    string getColor(){
        return color;
    }
};

class Slot{
    Car* parkedCar = NULL;
    int slotNumber;

    public:

    Slot(int pSlotNumber){
        slotNumber = pSlotNumber;
    }

    Car* getParkedCar(){
        return parkedCar;
    }

    int getSlotNumber(){
        return slotNumber;
    }

    bool isSlotFree(){
        return parkedCar != NULL;
    }

    void assignCar(Car* pCar){
        parkedCar = pCar;
    }

    void unassignCar(){
        parkedCar = NULL;
    }
};

class ParkingLot{
    int capacity;
    unordered_map<int, Slot*> slots;

    public:

    ParkingLot(int pCapacity){
        capacity = pCapacity;
    }

    Slot* getSlot(int slotNumber){

        if(slots.find(slotNumber)==slots.end()){
            slots[slotNumber] = new Slot(slotNumber);
        }
        return slots[slotNumber];
    }

    unordered_map<int, Slot*> getAllSlots(){
        return slots;
    }

    Slot* park(Car* pCar, int slotNumber){

        if(slots.find(slotNumber)!=slots.end()){
            cout<<"Slot is already occupied!"<<endl;
            return NULL;
        }
        Slot* s = new Slot(slotNumber);
        s->assignCar(pCar);
        cout<<"Car is parked"<<endl;
        return slots[slotNumber] = s;

    }

    void makeSlotFree(int slotNumber){
        if(slots.find(slotNumber)==slots.end()){
            cout<<"Slot is already free!"<<endl;
            return;
        }
        slots.erase(slotNumber);
        cout<<"Slot is now free!"<<endl;
        return;
    }

    int getCapacity(){
        return capacity;
    }
};

class ParkingStrategy {

    public:

    virtual void addSlot(int slotNumber) = 0;

    virtual void removeSlot(int slotNumber) = 0;

    virtual int getNextSlot() = 0;
};

class NaturalParkingStrategy : public ParkingStrategy {
    set<int> slots;

    public:

    void addSlot(int slotNumber){
        slots.insert(slotNumber);
    }

    void removeSlot(int slotNumber){
        slots.erase(slotNumber);
    }

    int getNextSlot(){
        if(slots.empty()){
            cout<<"No slot is available!"<<endl;
            return -1;
        }
        return *slots.begin();// * return value instead of the iterator
    }
};

class ParkingLotService{
    ParkingLot* parkingLot;
    ParkingStrategy* parkingStrategy;

    public:

    ParkingLotService(ParkingLot* pParkingLot, ParkingStrategy* pParkingStrategy){
        if(parkingLot!=NULL){
            cout<<"Parking Lot Already Exists!"<<endl;
        }else{
            parkingLot = pParkingLot;
            parkingStrategy = pParkingStrategy;
            int capacity = parkingLot->getCapacity();

            for(int i=1; i<=capacity; i++){
                parkingStrategy->addSlot(i);
            }
        }
    }

    int park(Car* pCar){
        //assuming parking lot exists
        int nextFreeSlot = parkingStrategy->getNextSlot();
        if(nextFreeSlot==-1){
            return -1;
        }
        parkingLot->park(pCar, nextFreeSlot);
        parkingStrategy->removeSlot(nextFreeSlot);
        return nextFreeSlot;
    }

    void makeSlotFree(int slotNumber){
        parkingLot->makeSlotFree(slotNumber);
        parkingStrategy->addSlot(slotNumber);
    }

    vector<Slot*> getOccupiedSlots(){
        vector<Slot*> listOfSlots;
        int capacity = parkingLot->getCapacity();
        unordered_map<int, Slot*> mp = parkingLot->getAllSlots();

        for(int i=1; i<=capacity; i++){
            if(mp.find(i)!=mp.end()){
                Slot* s = mp[i];
                // cout<<"Slot num is "<<s->getSlotNumber()<<endl;
                if(s->isSlotFree()){
                    listOfSlots.push_back(s);
                }
            }
        }
        return listOfSlots;
    }

    vector<Slot*> getSlotsForColor(string color){
        vector<Slot*> listOfSlots;
        unordered_map<int, Slot*> mp = parkingLot->getAllSlots();

        for(auto s : mp){
            Car* car = s.second->getParkedCar();
            if(car and car->getColor() == color){
                listOfSlots.push_back(s.second);
            }
        }
        return listOfSlots;
    }
};

int main(){
    ParkingStrategy* parkingStrategy = new NaturalParkingStrategy();
    ParkingLot* parkingLot = new ParkingLot(3);

    ParkingLotService* parkingLotService = new ParkingLotService(parkingLot, parkingStrategy);

    Car* car1 = new Car("blue", "KA1");
    Car* car2 = new Car("red", "KA2");
    Car* car3 = new Car("red", "KA3");
    Car* car4 = new Car("blue", "KA4");
    Car* car5 = new Car("blue", "KA5");


    parkingLotService->park(car1);
    parkingLotService->park(car2);
    parkingLotService->park(car3);
    parkingLotService->park(car4);

    cout<<"Occupied slots are : "<<endl;
    vector<Slot*> listOfSlots = parkingLotService->getOccupiedSlots();
    // cout<<"Size is "<<listOfSlots.size()<<endl;
    for (auto s : listOfSlots){
        cout<<s->getSlotNumber()<<endl;
    }

    parkingLotService->makeSlotFree(2);
    parkingLotService->park(car5);

    vector<Slot*> listOfSlotsForColor = parkingLotService->getSlotsForColor("blue");
    cout<<"Slots with color blue cars are :"<<endl;
    for (auto s : listOfSlotsForColor){
        cout<<s->getSlotNumber()<<endl;
    }


    return 0;
}

//we can have an interface vehicle to add support for different type of vehicles instead of a car