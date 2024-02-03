#include <iostream>
#include <unordered_map>
#include <vector>
#include <mutex>
using namespace std;

enum class SEAT{
    LUXURY,
    ECONOMY
};

class Customer{
    string id;
    string email;

    public:

    Customer(string pId, string pEmail){
        id = pId;
        email = pEmail;
    }

    string getId(){
        return id;
    }
};

class Seat{
    SEAT type;
    string id;
    Customer* customer;
    int price;

    public:

    Seat(SEAT pType, string pId, Customer* pCustomer, int pPrice){
        type = pType;
        id = pId;
        customer = pCustomer;
        price = pPrice;
    }

    Customer* getCustomer(){
        return customer;
    }

    void setCustomer(Customer* pCustomer){
        customer = pCustomer;
    }

    string getId(){
        return id;
    }
};

class Aircraft{
    unordered_map<string, Seat*> seats;//{seatId, Seat}
    mutex mtx;

    public:

    Aircraft(unordered_map<string, Seat*> pSeats){
        seats = pSeats;
    }

    bool fixSeat(Seat* s, Customer* c){//this is thread safe
        // mtx.lock();
        if(seats[s->getId()]){
            cout<<"Seat already reserved!"<<endl;
            return false;
        }
        s->setCustomer(c);
        seats[s->getId()] = s;
        cout<<"Seat reservation Successful!"<<endl;
        return true;
        // mtx.unlock();
    }

    unordered_map<string, Seat*> getSeats(){
        return seats;
    }

    bool cancelForCustomer(Customer* customer){
        for(auto seat : seats){
            if(seat.second and seat.second->getCustomer()->getId() == customer->getId()){
                seat.second->setCustomer(NULL);
                seats.erase(seat.second->getId());
                cout<<"Cancellation successful!"<<endl;
                return true;
            }
        }
        cout<<"Customer not found! : No seat reserved by this customer for this flight!"<<endl;
        return false;//customer not found
    }
};

class Flight{
    string src;
    string dest;
    string date;
    Aircraft* aircraft;
    string startTime;
    string endTime;

    public:

    Flight(string pSrc, string pDest, string pDate, string pStartTime, string pEndTime, Aircraft* pAircraft){
        src = pSrc;
        dest = pDest;
        date = pDate;
        startTime = pStartTime;
        endTime = pEndTime;
        aircraft = pAircraft;
    }

    Aircraft* getAircraft(){
        return aircraft;
    }

    string getSrc(){
        return src;
    }

    string getDest(){
        return dest;
    }

    string getDate(){
        return date;
    }
};

//System instance follows singleton design pattern, because their should alwas be a single instance of system
class System{
    vector<Flight> flights;
    static System* systemInstance;
    static mutex mtx;

    System(){}

    public:

    static System* getInstance(){
        if (systemInstance == NULL){
            mtx.lock();
            if(systemInstance == NULL){
                systemInstance = new System();
            }
            mtx.unlock();
        }
        return systemInstance;
    }

    vector<Flight> fetchFlights(string src, string dest, string date){
        vector<Flight> filteredFlights;

        for(auto flight : flights){
            if(flight.getSrc() == src and flight.getDest() == dest and flight.getDate() == date){
                filteredFlights.push_back(flight);
            }
        }
        return filteredFlights;
    }

    bool fixSeat(Flight* flight, Seat* seat){
        return flight->getAircraft()->fixSeat(seat, seat->getCustomer());
    }

    bool cancelForCustomer(Flight* flight, Customer* customer){
        return flight->getAircraft()->cancelForCustomer(customer);
    }

    void addFlight(Flight flight){
        flights.push_back(flight);
    }
};

System* System::systemInstance = NULL;
mutex System::mtx;

//booking manager
class BookingManager{
    // this is b/w customer and system
    System* system;

    public:

    BookingManager(){
        // customer = pCustomer;
        system = System::getInstance();
        // seat = pSeat;
    }

    bool fixSeat(Flight* flight, Seat* seat){
        bool isBooked = system->fixSeat(flight, seat);
        if(isBooked){
            return true;
        }else{
            return false;
        }
    }

    bool cancelForCustomer(Flight* flight, Customer* customer){
        bool isCancelled = system->cancelForCustomer(flight, customer);
        if(isCancelled){
            return true;
        }else{
            return false;
        }
    }
};

int main(){
    unordered_map<string,Seat*> seats = {{"s1", NULL}, {"s2", NULL}};
    Aircraft* a1 = new Aircraft(seats);
    Flight* f1 = new Flight("Delhi", "Banglore", "13Jan2024","1500","1700",a1);

    Aircraft* a2 = new Aircraft(seats);
    Flight* f2 = new Flight("Delhi", "Mumbai", "13Jan2024","1500","1700",a2);

    Customer* cust1 = new Customer("cust1", "cust1@email.com");
    Seat* seat1 = new Seat(SEAT::LUXURY, "s1", cust1, 250);

    Customer* cust2 = new Customer("cust2", "cust2@email.com");
    Seat* seat2 = new Seat(SEAT::LUXURY, "s2", cust2, 250);

    Customer* cust12 = new Customer("cust1", "cust1@email.com");
    Seat* seat12 = new Seat(SEAT::LUXURY, "s1", cust1, 250);

    Customer* cust22 = new Customer("cust2", "cust2@email.com");
    Seat* seat22 = new Seat(SEAT::LUXURY, "s2", cust22, 250);

    Customer* cust32 = new Customer("cust3", "cust2@email.com");
    Seat* seat32 = new Seat(SEAT::LUXURY, "s2", cust32, 250);

    Customer* cust3 = new Customer("cust3", "cust2@email.com");
    Seat* seat3 = new Seat(SEAT::LUXURY, "s2", cust3, 250);

    BookingManager* bookingManager = new BookingManager();

    bookingManager->fixSeat(f1,seat1);
    bookingManager->fixSeat(f2,seat12);
    bookingManager->fixSeat(f2,seat22);
    bookingManager->fixSeat(f1,seat1);
    bookingManager->fixSeat(f1,seat2);
    bookingManager->cancelForCustomer(f2,cust12);
    bookingManager->cancelForCustomer(f2,cust12);
    bookingManager->fixSeat(f1,seat3);
    bookingManager->cancelForCustomer(f1,cust1);
    bookingManager->fixSeat(f2,seat12);
    bookingManager->fixSeat(f2,seat32);
    bookingManager->cancelForCustomer(f1,cust1);
    return 0;
}