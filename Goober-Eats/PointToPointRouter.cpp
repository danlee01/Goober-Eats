#include "provided.h"
#include <list>
#include <set>
#include <map>

#include "support.h"


using namespace std;

class PointToPointRouterImpl
{
public:
    PointToPointRouterImpl(const StreetMap* sm);
    ~PointToPointRouterImpl();
    DeliveryResult generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const;
    
private:
    const StreetMap* sm;
    
    double getTotalDist(const list<StreetSegment>& route) const
    {
        double totalDist = 0;
        
        for (auto p : route)
        {
            totalDist  += distanceEarthMiles(p.start, p.end);
        }
        
        return totalDist;
    }
    
    
};



PointToPointRouterImpl::PointToPointRouterImpl(const StreetMap* sm)
: sm(sm)
{
}

PointToPointRouterImpl::~PointToPointRouterImpl()
{
}

DeliveryResult PointToPointRouterImpl::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    // Validate GeoCoord
    vector<StreetSegment> streetSegs;
    if (!sm->getSegmentsThatStartWith(start, streetSegs))
        return BAD_COORD;
    
    if (start == end)
    {
        route.clear();
        return DELIVERY_SUCCESS;
    }
    
    //  set q (open, )
    set<GeoCoord> open;
    
    //  map <coord this, StreetSegment parentEdge>
    map<GeoCoord, StreetSegment> cameFrom;
    
    map<GeoCoord, Score> gScore;
    map<GeoCoord, Score> fScore;
    
    open.insert(start);
    gScore.insert(pair<GeoCoord, Score>(start, 0));
    fScore.insert(pair<GeoCoord, Score>(start, distanceEarthMiles(start, end)));
    
    while (!open.empty())
    {
        // Get lowest fScore GeoCoord from open
        GeoCoord current;
        Score lowestF;
        for (auto p : open)
        {
            if (fScore[p] < lowestF)
            {
                current = p;
                lowestF = fScore[p];
            }
        }
        
        if (current == end)
        {
            // construct path
            route.clear();
            totalDistanceTravelled = gScore[current].score;
            
            while (cameFrom[current].start != start)
            {
                route.push_front(cameFrom[current]);
                current = cameFrom[current].start;
            }
            
            route.push_front(cameFrom[current]);
            
            return DELIVERY_SUCCESS;
        }
        
        open.erase(open.find(current));
        
        vector<StreetSegment> streetSegs;
        sm->getSegmentsThatStartWith(current, streetSegs);
        for ( auto p : streetSegs )
        {
            Score tentativeG(gScore[current].score + distanceEarthMiles(current, p.end));
            
            if ( tentativeG < gScore[p.end] )
            {
                cameFrom[p.end] = p;
                gScore[p.end] = tentativeG;
                fScore[p.end] = Score(gScore[p.end].score + distanceEarthMiles(current, end));
                
                if (open.find(p.end) == open.end())
                    open.insert(p.end);
            }
        }
        
    }
    
    cerr << "No route was found!" << endl;
    return NO_ROUTE;
                
}

//******************** PointToPointRouter functions ***************************

// These functions simply delegate to PointToPointRouterImpl's functions.
// You probably don't want to change any of this code.

PointToPointRouter::PointToPointRouter(const StreetMap* sm)
{
    m_impl = new PointToPointRouterImpl(sm);
}

PointToPointRouter::~PointToPointRouter()
{
    delete m_impl;
}

DeliveryResult PointToPointRouter::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    return m_impl->generatePointToPointRoute(start, end, route, totalDistanceTravelled);
}

