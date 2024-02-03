#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <thread>
#include <mutex>
#include <functional>
using namespace std;

class ElevatorCar;
class InternalDispatcher;
class ElevatorController;
class ElevatorCreator;

enum Direction{
    UP,
    DOWN
};

enum ElevatorState{
    IDLE,
    MOVING
};

class ElevatorDisplay{
    int floor;
    Direction direction;

    public:

    void setDisplay(int floor, Direction direction){
        this->floor = floor;
        this->direction = direction;
    }

    void showDisplay(){
        cout<<"Floor is "<<floor<<endl;
        string dir = direction == 0 ? "UP" : "DOWN";
        cout<<"Direction is "<<dir<<endl;
    }
};

class ElevatorDoor {

    public:

    void openDoor(){
        cout<<"Opening the Elevator door "<<endl;
    }

    void closeDoor(){
        cout<<"Closing the Elevator door"<<endl;
    }

};

class ElevatorCreator {

    public:

    static vector<ElevatorController*> elevatorControllerList;

    ElevatorCreator(){
        initialize();
    }

    void initialize();
};

class InternalDispatcher{

    public:

    // vector<ElevatorController*>  elevatorControllerList = ElevatorCreator::elevatorControllerList;

    void submitInternalRequest(int startFloor, int destFloor, ElevatorCar* elevatorCar);

    void pushInternalRequest(ElevatorCar* elevatorCar);
};

class InternalButtons{

    public:

    InternalDispatcher* dispatcher = new InternalDispatcher();

    vector<int> availableButtons = {1,2,3,4,5,6,7,8,9,10};
    int buttonSelected;

    public:

    void pressButton(int startFloor, int destination, ElevatorCar* elevatorCar);

    void pushInternalRequest(ElevatorCar* elevatorCar);

};

class ElevatorCar {

    public:

    int id;
    ElevatorDisplay* display;
    InternalButtons* internalButtons;
    ElevatorState elevatorState;
    int currentFloor;
    Direction elevatorDirection;
    ElevatorDoor* elevatorDoor;

    ElevatorCar(int id){
        this->id = id;
        display = new ElevatorDisplay();
        internalButtons = new InternalButtons();
        elevatorState = ElevatorState::IDLE;
        currentFloor = 0;
        elevatorDirection = Direction::UP;
        elevatorDoor = new ElevatorDoor();

    }
    void showDisplay() {
         display->showDisplay();
    }

    void pressButton(int startFloor, int destination) {
        internalButtons->pressButton(startFloor, destination, this);
    }

    void notifyForInternalReuests(){
        internalButtons->pushInternalRequest(this);
    }

    void setDisplay() {
        this->display->setDisplay(currentFloor, elevatorDirection);
    }

    void moveElevator(Direction dir, int destinationFloor){
        int startFloor = currentFloor;
        if(dir == Direction::UP) {
            for(int i = startFloor; i<=destinationFloor; i++) {

                this_thread::sleep_for(std::chrono::milliseconds(1000));

                this->currentFloor = i;
                setDisplay();
                showDisplay();
                if(i == destinationFloor) {
                    elevatorDoor->openDoor();
                    cout<<"Reached Destination Floor: " <<destinationFloor<<endl;
                    elevatorDoor->closeDoor();
                    notifyForInternalReuests();
                    cout<<"-----------------------------------------------------"<<endl;
                    return;
                }
            }
        }

        if(dir == Direction::DOWN) {
            for(int i = startFloor; i>=destinationFloor; i--) {

                this_thread::sleep_for(std::chrono::milliseconds(1000));

                this->currentFloor = i;
                setDisplay();
                showDisplay();
                if(i == destinationFloor) {
                    elevatorDoor->openDoor();
                    cout<<"Reached Destination Floor: " <<destinationFloor<<endl;
                    elevatorDoor->closeDoor();
                    notifyForInternalReuests();
                    cout<<"-----------------------------------------------------"<<endl;
                    return;
                }
            }
        }
    }

};

class ElevatorController {
    priority_queue<int> upMinPQ;
    priority_queue<int> downMaxPQ;
    mutex mtx;
    condition_variable cv;
    unordered_map<int,queue<int>> internalRequestsPending;

    public:
    ElevatorCar* elevatorCar;

    ElevatorController(ElevatorCar* elevatorCar){
        this->elevatorCar = elevatorCar;
    }

    ~ElevatorController(){
        cout<<"destructor called!"<<endl;
    }

    void submitExternalRequest(int floor, Direction direction){
        lock_guard<mutex> lock(mtx);
        if(direction == Direction::DOWN) {
            downMaxPQ.push(floor);
        } else {
            upMinPQ.push(floor);
        }
        cv.notify_one();
    }

    void pushInternalRequest(){
        lock_guard<mutex> lock(mtx);
        // cout<<"current elevator floor is "<<elevatorCar->currentFloor<<endl;
        while(!internalRequestsPending[elevatorCar->currentFloor].empty()){
                int floor = internalRequestsPending[elevatorCar->currentFloor].front();
                // cout<<"pushing floor internal reuest"<<floor<<endl;
                if(elevatorCar->currentFloor > floor){
                    downMaxPQ.push(floor);
                } else {
                    upMinPQ.push(floor);
                }
                internalRequestsPending[elevatorCar->currentFloor].pop();
                if(internalRequestsPending[elevatorCar->currentFloor].empty()){
                    internalRequestsPending.erase(elevatorCar->currentFloor);
                }
        }
        cv.notify_one();
    }

    void submitInternalRequest(int startFloor, int destFloor){
            internalRequestsPending[startFloor].push(destFloor);
    }

    void controlElevator(){
        while(true) {
            unique_lock<mutex> lock(mtx); // Acquire the lock
            cv.wait(lock, [this] { // Wait for a signal while holding the lock
                return !(upMinPQ.empty() && downMaxPQ.empty());
            });

                elevatorCar->elevatorState = ElevatorState::MOVING;
                Direction currentDirection = elevatorCar->elevatorDirection;
                int currentFloor = elevatorCar->currentFloor;

                if(currentDirection == Direction::UP){
                    if(!upMinPQ.empty() && currentFloor <= upMinPQ.top()){
                        int nextFloor = upMinPQ.top(); upMinPQ.pop();
                        lock.unlock(); // Release the lock before moving
                        elevatorCar->moveElevator(Direction::UP, nextFloor);
                    } else {
                        elevatorCar->elevatorDirection = Direction::DOWN;
                    }
                } else {
                    if(!downMaxPQ.empty() && currentFloor >= downMaxPQ.top()){
                        int nextFloor = downMaxPQ.top(); downMaxPQ.pop();
                        lock.unlock(); // Release the lock before moving
                        elevatorCar->moveElevator(Direction::DOWN, nextFloor);
                    } else {
                        elevatorCar->elevatorDirection = Direction::UP;
                    }
                }
            }
        }
};

void InternalDispatcher :: submitInternalRequest(int startFloor, int endFloor, ElevatorCar* elevatorCar){
        for(auto elevatorController : ElevatorCreator::elevatorControllerList){
            if(elevatorController->elevatorCar == elevatorCar){
                elevatorController->submitInternalRequest(startFloor, endFloor);
                break;
            }
        }
    }

void InternalDispatcher :: pushInternalRequest(ElevatorCar* elevatorCar){
    for(auto elevatorController : ElevatorCreator::elevatorControllerList){
            if(elevatorController->elevatorCar == elevatorCar){
                elevatorController->pushInternalRequest();
                break;
            }
        }
}

// Definition of static member outside the class
vector<ElevatorController*> ElevatorCreator::elevatorControllerList;

// Definition of the initialize function
void ElevatorCreator :: initialize() {
    ElevatorCar* elevatorCar1 = new ElevatorCar(1);
    ElevatorController* controller1 = new ElevatorController(elevatorCar1);

    // ElevatorCar* elevatorCar2 = new ElevatorCar(2);
    // ElevatorController* controller2 = new ElevatorController(elevatorCar2);

    ElevatorCreator::elevatorControllerList.push_back(controller1);
    // elevatorControllerList.push_back(controller2);
}

class ExternalDispatcher {

    // vector<ElevatorController*>  elevatorControllerList = ElevatorCreator::elevatorControllerList;

    public:

    void submitExternalRequest(int floor, Direction direction){

        //for simplicity, i am following even odd,
        for(ElevatorController* elevatorController : ElevatorCreator::elevatorControllerList) {
            elevatorController->submitExternalRequest(floor,direction);


        //    int elevatorID = elevatorController->elevatorCar->id;
        //    if (elevatorID%2==1 && floor%2==1){
        //        elevatorController->submitExternalRequest(floor,direction);
        //    } else if(elevatorID%2==0 && floor%2==0){
        //        elevatorController->submitExternalRequest(floor,direction);
        //    }
        }
    }
};

class Floor {
    int floorNumber;
    ExternalDispatcher* externalDispatcher;

    public:

    Floor(int floorNumber){
        this->floorNumber = floorNumber;
        externalDispatcher = new ExternalDispatcher();
    }
    void pressButton(Direction direction) {

        externalDispatcher->submitExternalRequest(floorNumber, direction);
    }
};

class Building {

    vector<Floor*> floorList;

    public:

    Building(vector<Floor*> floors){
        floorList = floors;
    }

    void addFloors(Floor* newFloor){
        floorList.push_back(newFloor);
    }

    // void removeFloors(Floor* removeFloor){
    //     floorList.erase(removeFloor);
    // }

    vector<Floor*> getAllFloorList(){
        return floorList;
    }
};

void InternalButtons :: pressButton(int startFloor,int destination, ElevatorCar* elevatorCar) {

        //1.check if destination is in the list of available floors

        //2.submit the request to the jobDispatcher
        dispatcher->submitInternalRequest(startFloor, destination, elevatorCar);
}

void InternalButtons :: pushInternalRequest(ElevatorCar* elevatorCar){
    dispatcher->pushInternalRequest(elevatorCar);
}

void startElevatorControllers(){
    for(auto elevatorController : ElevatorCreator::elevatorControllerList){
        thread t([elevatorController]() {elevatorController->controlElevator();});
        t.detach();
    }
}

int main() {

        vector<Floor*> floorList;
        Floor* floor0 = new Floor(0);
        Floor* floor1 = new Floor(1);
        Floor* floor2 = new Floor(2);
        Floor* floor3 = new Floor(3);
        Floor* floor4 = new Floor(4);
        Floor* floor5 = new Floor(5);
        floorList.push_back(floor0);
        floorList.push_back(floor1);
        floorList.push_back(floor2);
        floorList.push_back(floor3);
        floorList.push_back(floor4);
        floorList.push_back(floor5);

        Building* building = new Building(floorList);

        ElevatorCreator* elevatorCreator = new ElevatorCreator();

        startElevatorControllers();

        // InternalButtons* internalButtons = new InternalButtons();

        ElevatorCar* elevatorCar1 = ElevatorCreator::elevatorControllerList[0]->elevatorCar;

        vector<Floor*> floors = building->getAllFloorList();

        floors[0]->pressButton(Direction::UP);   elevatorCar1->pressButton(0,5);
        floors[4]->pressButton(Direction::DOWN); elevatorCar1->pressButton(4,2);
        floors[3]->pressButton(Direction::UP);   elevatorCar1->pressButton(3,4);
        // cout<<"s4"<<endl;
        // floors[1]->pressButton(Direction::UP);   internalButtons->pressButton(2,elevatorCar1);
        // cout<<"s5"<<endl;
        // floors[5]->pressButton(Direction::DOWN); internalButtons->pressButton(5,elevatorCar1);
        // cout<<"s6"<<endl;
        // floors[0]->pressButton(Direction::DOWN); internalButtons->pressButton(0,elevatorCar1);
        // cout<<"s7"<<endl;

        this_thread::sleep_for(std::chrono::milliseconds(25000));

}

