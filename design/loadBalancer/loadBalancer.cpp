#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <queue>
using namespace std;

enum class RequestType{
    ORDER,
    CUSTOMER,
    TXN
};

//destination signifies a node (server where a specific service is running)
// it contains context of how many requests and active connection does a destionation node has
class Destination{
    public:

    string ipAddress;
    int threshold;
    int requestBeingServed;

    bool acceptRequest(Request* request){
        if(threshold <= requestBeingServed){
            requestBeingServed++;
            return true;
        }else{
            return false;
        }
    }

    void completeRequest(){
        requestBeingServed--;
    }
};

// different services serves different type of requests
//Eg: api-order serves order related requests and api-txns serves txn related requests
class Service{
    string name;
    unordered_set<Destination*> destinations;

    public:

    void addDestination(Destination* destination){
        destinations.insert(destination);
    }

    void removeDestination(Destination* destination){
        destinations.erase(destination);
    }

    unordered_set<Destination*> getDestinations(){
        return destinations;
    }
};

//request with id, type and parameters
class Request{
    public:
    string id;
    RequestType* requestType;
    unordered_map<string,string> parameters;
};


// load balancer algos
class LoadBalancer{

    protected:

    unordered_map<RequestType*, Service*> serviceMap;

    public:

    void registerService(RequestType* requestType, Service* service){
        serviceMap[requestType] = service;
    }

    unordered_map<RequestType*, Service*> getServiceMap(){
        return serviceMap;
    }

    unordered_set<Destination*> getDestinations(Request* request){
        Service* service = serviceMap[request->requestType];
        return service->getDestinations();
    }

    virtual Destination* balanceLoad(Request* request) = 0;
};

class LeastConnectionLoadBalancer : public LoadBalancer{
    public:

    Destination* balanceLoad(Request* request){

        unordered_set<Destination*> destinations = getDestinations(request);

        Destination* destinationWithMinConnections = new Destination();
        destinationWithMinConnections->requestBeingServed = INT_MAX;

        // get the destination node with min number of active connections
        for(Destination* destionation : destinations){
            if(destinationWithMinConnections->requestBeingServed > destionation->requestBeingServed){
                destinationWithMinConnections = destionation;
            }
        }

        destinationWithMinConnections->acceptRequest(request);

        return destinationWithMinConnections;
    }
};

queue<Destination*> convertToQueue(unordered_set<Destination*> destinations){
    queue<Destination*> q;

    for(auto it : destinations){
        q.push(it);
    }
    return q;
}

class RoundRobinLoadBalancer : public LoadBalancer{
    unordered_map<RequestType*, queue<Destination*>> destinationsForRequest;
    public:

    Destination* balanceLoad(Request* request){

        if(destinationsForRequest.find(request->requestType)==destinationsForRequest.end()){
            unordered_set<Destination*> destinations = getDestinations(request);
            destinationsForRequest[request->requestType] = convertToQueue(destinations);
        }

        Destination* destination = destinationsForRequest[request->requestType].front();
        destinationsForRequest[request->requestType].pop();
        destinationsForRequest[request->requestType].push(destination);

        destination->acceptRequest(request);

        return destination;
    }
};

enum class LoadBalancerType{
    ROUND_ROBIN,
    LEAST_CONNECTION
};

class LoadBalancerFactory{

    LoadBalancer* createLoadBalancer(LoadBalancerType lbType){
        switch (lbType)
        {
            case LoadBalancerType::ROUND_ROBIN:
                return new RoundRobinLoadBalancer();
                break;
            case LoadBalancerType::LEAST_CONNECTION:
                return new LeastConnectionLoadBalancer();
            default:
                return new LeastConnectionLoadBalancer();
                break;
        }
    }
};

//each type of load balancer could be created using singelton, for eg one instance of
//round robin and one instance of least connection

//