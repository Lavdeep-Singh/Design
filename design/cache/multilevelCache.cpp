#include <iostream>
#include <unordered_map>
#include<vector>
#include "cache.cpp"
using namespace std;

class LayerTimeData{
    double readTime;
    double writeTime;

    public:

    LayerTimeData(double pReadTime, double pWriteTime){
        readTime = pReadTime;
        writeTime = pWriteTime;
    }

    double getReadTime(){
        return readTime;
    }

    double getWriteTime(){
        return writeTime;
    }
};

class ReadResponse{
    double timeTaken;
    string value;

    public:

    ReadResponse(double pTimeTaken, string pValue){
        timeTaken = pTimeTaken;
        value = pValue;
    }

    double getTimeTaken(){
        return timeTaken;
    }

    string getValue(){
        return value;
    }
};

class ICacheLayer{
    public:
    virtual double write(string key, string value) = 0;//returns total time taken by current and all layers below it to write data

    virtual ReadResponse* read(string key) = 0;

    virtual vector<double> usages() = 0;
};

class DefaultLayer : public ICacheLayer{
    LayerTimeData* layerTimeData;
    Cache* cache;
    ICacheLayer* nextLayer;

    public:

    DefaultLayer(LayerTimeData* pLayerTimeData, Cache* pCache, ICacheLayer* pNextLayer){
        layerTimeData = pLayerTimeData;
        cache = pCache;
        nextLayer = pNextLayer;
    }

    double write(string key, string value){
        double timeTaken = 0.0;
        string currentLevelValue = cache->get(key);
        timeTaken+=layerTimeData->getReadTime();

        if(value!=currentLevelValue){
            cache->put(key,value);
            timeTaken+=layerTimeData->getWriteTime();
        }

        timeTaken+=nextLayer->write(key, value);
        return timeTaken;

    }

    ReadResponse* read(string key){
        double timeTaken = 0.0;
        string currentLevelValue = cache->get(key);
        timeTaken+=layerTimeData->getReadTime();

        if(currentLevelValue==""){
            ReadResponse* nextLayerReadResponse = nextLayer->read(key);
            timeTaken+=nextLayerReadResponse->getTimeTaken();
            currentLevelValue = nextLayerReadResponse->getValue();
            if(nextLayerReadResponse->getValue()!=""){
                cache->put(key,nextLayerReadResponse->getValue());
                timeTaken+=layerTimeData->getReadTime();
            }
        }
        return new ReadResponse(timeTaken, currentLevelValue);
    }

    vector<double> usages(){
        vector<double> usage;

        if(nextLayer){
            usage = nextLayer->usages();
        }
        usage.push_back(cache->getCurrentUsage());
        return usage;
    }
};

class NullEffectLastLayer : public ICacheLayer{

    public:

    double write(string key, string value){
        return 0.0;
    }

    ReadResponse* read(string key){
        return new ReadResponse(0.0, "");
    }

    vector<double> usages(){
        return {};
    }
};

class Stats{
    public:
    double avgReadTime;
    double avgWriteTime;
    vector<double> usages;

    Stats(double rt, double wt, vector<double>& u){
        avgReadTime = rt;
        avgWriteTime = wt;
        usages = u;
    }
};

class CacheService{
    ICacheLayer* cacheLayer;
    vector<double> readTimes;
    vector<double> writeTimes;
    int requiredCount=0;

    public:

    CacheService(ICacheLayer* pCacheLayer, int pAvgCount){
        cacheLayer = pCacheLayer;
        requiredCount = pAvgCount;
    }

    void set(string key, string value){
        double writeTimeTaken = cacheLayer->write(key, value);
        addTimes(writeTimes, writeTimeTaken);

    }

    string get(string key){
        ReadResponse* resp = cacheLayer->read(key);
        addTimes(readTimes,resp->getTimeTaken());
        return resp->getValue();
    }

    double getAvgReadTime(){
        double sum = getSum(readTimes);
        return sum/readTimes.size();
    }

    double getAvgWriteTime(){
        double sum = getSum(writeTimes);
        return sum/writeTimes.size();
    }

    double getSum(vector<double>& v){
        double s = 0.0;
        for(int i=0; i<v.size(); i++){
            s+=v[i];
        }
        return s;
    }

    void addTimes(vector<double>& write, double writeTime){
        if(write.size()>=requiredCount){
            write.erase(write.begin());
        }
        write.push_back(writeTime);
    }

    Stats* getStats(){
        vector<double> usages = cacheLayer->usages();
        return new Stats(getAvgReadTime(),getAvgWriteTime(), usages);
    }


};

int main(){
    //storage choosen is map storage, we can use in mem cache if implemented
    Storage* s1 = new MapStorage(5);
    Storage* s2 = new MapStorage(4);
    //using lru eviction policy, can use other eviction policy
    EvictionPolicy* e = new LruEvictionPolicy();
    //creating caches, having their respective storage and eviction policies
    Cache* c1 = new Cache(s1, e);
    Cache* c2 = new Cache(s2, e);
    //This layer time data indicates how much time respective cache takes to read and write
    LayerTimeData* ltd1 = new LayerTimeData(2,4);
    LayerTimeData* ltd2 = new LayerTimeData(3,6);
    //creating layers and threading them like L1 -> L2 -> NULL Layer
    DefaultLayer* defaultLayer2 = new DefaultLayer(ltd2, c2, new NullEffectLastLayer());
    DefaultLayer* defaultLayer1 = new DefaultLayer(ltd1, c1, defaultLayer2);
    //initialising cache service, with over cache layer and count
    CacheService* cacheService = new CacheService(defaultLayer1, 10);


    cacheService->set("1","halwa1");
    for( auto it : cacheService->getStats()->usages){
        cout<<it<<" ";
    }
    cout<<endl;
    cacheService->set("2","pudi2");
    for( auto it : cacheService->getStats()->usages){
        cout<<it<<" ";
    }
    cout<<endl;
    cacheService->set("3","halwa3");
    for( auto it : cacheService->getStats()->usages){
        cout<<it<<" ";
    }
    cout<<endl;
    cacheService->set("8","pudi8");
    for( auto it : cacheService->getStats()->usages){
        cout<<it<<" ";
    }
    cout<<endl;
    cacheService->get("1");
    for( auto it : cacheService->getStats()->usages){
        cout<<it<<" ";
    }
    cout<<endl;
    cacheService->set("4","pudi4");
    for( auto it : cacheService->getStats()->usages){
        cout<<it<<" ";
    }
    cout<<endl;
    cacheService->set("5","halwa5");
    for( auto it : cacheService->getStats()->usages){
        cout<<it<<" ";
    }
    cout<<endl;
    cacheService->set("6","pudi6");
    for( auto it : cacheService->getStats()->usages){
        cout<<it<<" ";
    }
    cout<<endl;
    cacheService->set("7","halwa7");
    for( auto it : cacheService->getStats()->usages){
        cout<<it<<" ";
    }
    cout<<endl;
    cacheService->get("3");
    for( auto it : cacheService->getStats()->usages){
        cout<<it<<" ";
    }
    cout<<endl;
    cacheService->get("8");
    for( auto it : cacheService->getStats()->usages){
        cout<<it<<" ";
    }
    cout<<endl;

    Stats* stats = cacheService->getStats();

    cout<<"avg readtime is "<<stats->avgReadTime<<" avg write time is "<<stats->avgWriteTime<<endl;
}