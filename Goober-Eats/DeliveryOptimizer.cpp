#include "provided.h"
#include <vector>
#include <random>
#include <utility>
using namespace std;

class DeliveryOptimizerImpl
{
public:
    DeliveryOptimizerImpl(const StreetMap* sm);
    ~DeliveryOptimizerImpl();
    void optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const;
private:
    const StreetMap* sm;
    double getCrowDistance(vector<DeliveryRequest> deliveries) const
    {
        double distance = 0;
        
        for (int i = 0; i < deliveries.size() - 1; i++)
        {
            distance += distanceEarthMiles(deliveries[i].location, deliveries[i+1].location);
        }
        
        return distance;
    }
    
    // Return a uniformly distributed random int from min to max, inclusive
    int randInt(int min, int max) const
    {
        if (max < min)
            std::swap(max, min);
        static random_device rd;
        static default_random_engine generator(rd());
        std::uniform_int_distribution<> distro(min, max);
        return distro(generator);
    }
    
};

DeliveryOptimizerImpl::DeliveryOptimizerImpl(const StreetMap* sm)
: sm(sm)
{
}

DeliveryOptimizerImpl::~DeliveryOptimizerImpl()
{
}

void DeliveryOptimizerImpl::optimizeDeliveryOrder(
    const GeoCoord& depot,
    vector<DeliveryRequest>& deliveries,
    double& oldCrowDistance,
    double& newCrowDistance) const
{
    // Create a PointToPointRouter so we can generate routes
    PointToPointRouter pp(sm);
    oldCrowDistance = getCrowDistance(deliveries);
    
    DeliveryRequest temp("temp", depot); // Will be used to swap in vector
    list<StreetSegment> dummyList;
    
    for (int i = 0; i < deliveries.size() - 1; i++)
    {
        double shortestDist = 0;
        pp.generatePointToPointRoute(deliveries[i].location, deliveries[i+1].location, dummyList, shortestDist);
        for (int j = i; j < deliveries.size(); j++)
        {
            //double currDist = distanceEarthMiles(deliveries[i].location, deliveries[j].location);
            double currDist = 0;
            
            pp.generatePointToPointRoute(deliveries[i].location, deliveries[j].location, dummyList, currDist);
            
            // if distance between jth delivery and ith delivery is less than what we currently have,
            // swap i+1th delivery and jth delivery
            if (currDist < shortestDist)
            {
                shortestDist = currDist;
                temp = deliveries[j];
                deliveries[j] = deliveries[i+1];
                deliveries[i+1] = temp;
            }
        }
        
    }
    
    newCrowDistance = getCrowDistance(deliveries);
}

//******************** DeliveryOptimizer functions ****************************

// These functions simply delegate to DeliveryOptimizerImpl's functions.
// You probably don't want to change any of this code.

DeliveryOptimizer::DeliveryOptimizer(const StreetMap* sm)
{
    m_impl = new DeliveryOptimizerImpl(sm);
}

DeliveryOptimizer::~DeliveryOptimizer()
{
    delete m_impl;
}

void DeliveryOptimizer::optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const
{
    return m_impl->optimizeDeliveryOrder(depot, deliveries, oldCrowDistance, newCrowDistance);
}
