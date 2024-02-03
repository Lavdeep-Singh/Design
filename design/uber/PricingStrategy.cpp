#ifndef PRICING_STRATEGY
#define PRICING_STRATEGY

#include <iostream>

#include "TripMetadata.cpp"
#include "Common.cpp"
using namespace std;

//pricing strategy interface
class PricingStrategy{
    public:
    virtual double calculatePrice(TripMetaData* pTripMetadata) = 0;
};

//implementing pricing strategy with default pricing strategy
class DefaultPricingStrategy : public PricingStrategy{
    public:
    double calculatePrice(TripMetaData* pTripMetadata){
        cout<<"Based on default pricing strategy price is 100"<<endl;
        return 100.00;
    }
};

//implementing pricing strategy with rating based pricing strategy
class RatingBasedPricingStrategy : public PricingStrategy{
    public:
    double calculatePrice(TripMetaData* pTripMetadata){
        double price = Util::isHighRating(pTripMetadata->getRiderRating()) ? 55.0 : 65.0;
        cout<<"based on "<< Util::ratingToString(pTripMetadata->getRiderRating()) << " rating of the rider, price of trip is "<<price<<endl;
        return price;
    }
};

#endif