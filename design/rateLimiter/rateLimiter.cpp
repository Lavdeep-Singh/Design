#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <chrono>
using namespace std;

class RateLimiter{
    public:
    virtual bool grantAccess() = 0;
};

class SlidingWindow : public RateLimiter{
    public:

    queue<long> slidingWindow;
    int bucketCap;
    int time;

    SlidingWindow(int pTime, int pCap){
        time = pTime;
        bucketCap = pCap;
    }

    //how to handle concurrency here?
    // use mutex lock?
    void updateQueue(long currentTime){

        if(slidingWindow.empty()){
            return;
        }

        long timeDiff = (currentTime - slidingWindow.front())/1000;

        while(!slidingWindow.empty() and time<=timeDiff){
            slidingWindow.pop();
            timeDiff = (currentTime - slidingWindow.front())/1000;
        }
    }

    bool grantAccess(){
        auto duration = chrono::system_clock::now().time_since_epoch();//get time since epoch
        auto currentTimeInMills = chrono::duration_cast<chrono::milliseconds>(duration).count();//convert time since epoch to milliseconds

        updateQueue(currentTimeInMills);

        if(slidingWindow.size() < bucketCap){
            slidingWindow.push(currentTimeInMills);
            return true;
        }
        return false;
    }
};

class UserSlidingWindow{
    //for each user we are having a SlidingWindow RateLimiter

    unordered_map<int, SlidingWindow*> bucket;//{user-id , SlidingWindow ratelimiter}

    public:

    UserSlidingWindow(int id){
        bucket[id] = new SlidingWindow(1,10);
    }

    void accessApplication(int id){
        if(bucket[id]->grantAccess()){
            cout<<"Access Granted"<<endl;
        }else{
            cout<<"To many requests!"<<endl;
        }
    }
};

// we can extend rate limiter class for more rate limiting algos