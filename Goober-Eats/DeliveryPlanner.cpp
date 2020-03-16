#include "provided.h"
#include <vector>
using namespace std;

class DeliveryPlannerImpl
{
public:
    DeliveryPlannerImpl(const StreetMap* sm);
    ~DeliveryPlannerImpl();
    DeliveryResult generateDeliveryPlan(
        const GeoCoord& depot,
        const vector<DeliveryRequest>& deliveries,
        vector<DeliveryCommand>& commands,
        double& totalDistanceTravelled) const;
private:
    const StreetMap* sm;
    
    string getDirection(const StreetSegment& seg) const
    {
        const string dir[9] {"east", "northeast", "north", "northwest", "west", "southwest", "south", "southeast", "east"};
        
        double angle = angleOfLine(seg);
        
        for (int i = 0; i < 9; i++)
        {
            if (angle < 22.5+40*i)
                return dir[i];
        }
        
        return "east";
    }
    
};

DeliveryPlannerImpl::DeliveryPlannerImpl(const StreetMap* sm)
: sm(sm)
{
}

DeliveryPlannerImpl::~DeliveryPlannerImpl()
{
}

DeliveryResult DeliveryPlannerImpl::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    DeliveryOptimizer opt(sm);
    double dummy = 0;
    vector<DeliveryRequest> optDeliveries(deliveries);
    opt.optimizeDeliveryOrder(depot, optDeliveries, dummy, dummy);
    
    // Reset our commands vector
    commands.clear();
    
    // Generate point-to-point routes between depot and successive delivery points
    PointToPointRouter pp(sm);
    list<StreetSegment> route;
    
    GeoCoord start = depot;
    
    double distanceTravelled = 0;
    totalDistanceTravelled = 0;
    
    for (auto p : optDeliveries)
    {
        // Reset routeSegment and distanceTravelled
        route.clear();
        distanceTravelled = 0;
        
        DeliveryResult result = pp.generatePointToPointRoute(start, p.location, route, distanceTravelled);
        
        if (result != DELIVERY_SUCCESS)
            return result;
        
        // Iterate through completeRoute and produce DeliveryCommands
        string currStreet = route.begin()->name;
        string prevType = "start";
        StreetSegment prevSeg;
        for (auto seg : route)
        {
            DeliveryCommand command;
            
            // Always proceed on the first DeliveryCommand after leaving a start location
            if (prevType == "start")
            {
                string direction = getDirection(seg);
                double segDist = distanceEarthMiles(seg.start, seg.end);
                command.initAsProceedCommand(direction, seg.name, segDist);
                commands.push_back(command);
                prevType = "proceed";
            }
            else
            {
                if (seg.name != currStreet)  // Different street
                {
                    // If we need to turn
                    if (angleBetween2Lines(prevSeg, seg) >= 1 && angleBetween2Lines(prevSeg, seg) <= 359)
                    {
                            string direction = (angleBetween2Lines(prevSeg, seg) < 180) ? "left" : "right";
                            command.initAsTurnCommand(direction, seg.name);
                            commands.push_back(command);
                            DeliveryCommand afterTurn;
                            
                            direction = getDirection(seg);
                            double segDist = distanceEarthMiles(seg.start, seg.end);
                            afterTurn.initAsProceedCommand(direction, seg.name, segDist);
                            commands.push_back(afterTurn);
                    }
                    else
                    {
                        string direction = getDirection(seg);
                        double segDist = distanceEarthMiles(seg.start, seg.end);
                        command.initAsProceedCommand(direction, seg.name, segDist);
                        commands.push_back(command);
                        prevType = "proceed";
                    }
                }
                else // We catch sequential proceeds on same street here
                {
                    command = commands.back();
                    commands.pop_back();
                    double segDist = distanceEarthMiles(seg.start, seg.end);
                    command.increaseDistance(segDist);
                    commands.push_back(command);
                    
                }
                
                prevSeg = seg;
                currStreet = seg.name;
            }
            
        }
        DeliveryCommand delivery;
        delivery.initAsDeliverCommand(p.item);
        commands.push_back(delivery);
        
        totalDistanceTravelled += distanceTravelled;
        start = p.location;
    }
    
    // Return to depot. Same code as before.
    route.clear();
    distanceTravelled = 0;
    pp.generatePointToPointRoute(deliveries.back().location, depot, route, distanceTravelled);
    totalDistanceTravelled += distanceTravelled;
    string currStreet = route.begin()->name;
    string prevType = "start";
    StreetSegment prevSeg;
    for (auto seg : route)
    {
        DeliveryCommand command;
        
        // Repeated logic here :(
        if (prevType == "start")
        {
            string direction = getDirection(seg);
            double segDist = distanceEarthMiles(seg.start, seg.end);
            command.initAsProceedCommand(direction, seg.name, segDist);
            commands.push_back(command);
            prevType = "proceed";
        }
        else
        {
            if (seg.name != currStreet)
            {
                if (angleBetween2Lines(prevSeg, seg) >= 1 && angleBetween2Lines(prevSeg, seg) <= 359)
                {
                    string direction = (angleBetween2Lines(prevSeg, seg) < 180) ? "left" : "right";
                    command.initAsTurnCommand(direction, seg.name);
                    commands.push_back(command);
                    DeliveryCommand afterTurn;
                        
                    direction = getDirection(seg);
                    double segDist = distanceEarthMiles(seg.start, seg.end);
                    afterTurn.initAsProceedCommand(direction, seg.name, segDist);
                    commands.push_back(afterTurn);
                }
                else
                {
                    string direction = getDirection(seg);
                    double segDist = distanceEarthMiles(seg.start, seg.end);
                    command.initAsProceedCommand(direction, seg.name, segDist);
                    commands.push_back(command);
                    prevType = "proceed";
                }
            }
            else
            {
                command = commands.back();
                commands.pop_back();
                double segDist = distanceEarthMiles(seg.start, seg.end);
                command.increaseDistance(segDist);
                commands.push_back(command);
                
            }
            prevSeg = seg;
            currStreet = seg.name;
        }
    }
    return DELIVERY_SUCCESS;
}

//******************** DeliveryPlanner functions ******************************

// These functions simply delegate to DeliveryPlannerImpl's functions.
// You probably don't want to change any of this code.

DeliveryPlanner::DeliveryPlanner(const StreetMap* sm)
{
    m_impl = new DeliveryPlannerImpl(sm);
}

DeliveryPlanner::~DeliveryPlanner()
{
    delete m_impl;
}

DeliveryResult DeliveryPlanner::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    return m_impl->generateDeliveryPlan(depot, deliveries, commands, totalDistanceTravelled);
}
