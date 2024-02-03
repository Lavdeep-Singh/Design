#include <iostream>
#include<vector>
#include <chrono>
#include <ctime>
using namespace std;

int getNextGreater(int n){
    n = n-1;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    return n+1;
}

int main(){
    vector<int> v = {3,5,12,1,9,8,16,0,23};
    vector<int> a = {4,8,16,1,16,8,16,0,32};

    for(int i=0; i<v.size(); i++){
        int got = getNextGreater(v[i]);
        if( got == a[i]){
            cout<<"PASSED"<<endl;
        }else{
            cout<<"FAILED"<<" For "<<v[i]<<" Got "<<got<<endl;
        }
    }

    // Get the current time point
    auto currentTime = std::chrono::system_clock::now();

    // Convert the time point to a time_t object
    std::time_t currentTime_t = std::chrono::system_clock::to_time_t(currentTime);

    // Convert the time_t to a local time string
    std::string timeString = std::ctime(&currentTime_t);

    // Print the current time
    std::cout << "Current time: " << timeString;
}