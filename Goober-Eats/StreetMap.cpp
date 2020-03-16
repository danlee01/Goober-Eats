#include "provided.h"
#include <string>
#include <vector>
#include <functional>

#include "ExpandableHashMap.h"

// C++ Facilities for File I/O
#include <iostream>
#include <fstream>

using namespace std;

unsigned int hasher(const GeoCoord& g)
{
    return hash<string>()(g.latitudeText + g.longitudeText);
}

class StreetMapImpl
{
public:
    StreetMapImpl();
    ~StreetMapImpl();
    bool load(string mapFile);
    bool getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const;
    
private:
    ExpandableHashMap<GeoCoord, vector<StreetSegment>> map;
};

StreetMapImpl::StreetMapImpl()
{
    load("/Users/danlee/Documents/CS32/Goober-Eats/Goober-Eats/mapdata.txt");
}

StreetMapImpl::~StreetMapImpl()
{
}

bool StreetMapImpl::load(string mapFile)
{
    ifstream infile(mapFile);
    
    if (!infile)
    {
        cerr << "Cannot open " << mapFile << "!";
        return false;
    }
    
    // Begin loading map data.
    string streetName = "";
    int nSegments = 0;
    string startLatText, startLongText, endLatText, endLongText;
    
    // loop this after you implement
    // Read in street name and store in streetName.
    
    while( true )
    {
        //Read in street name.
        getline(infile, streetName);
        if (!infile) // Reached end of file trying to take street name
            break;
        
        // Read in number of street segments for this street.
        infile >> nSegments;
        infile.ignore(10000, '\n');
        
        for (int i = 0; i < nSegments; i++)
        {
            // Extract start GeoCoord.
            infile >> startLatText;
            infile >> startLongText;
            GeoCoord s(startLatText, startLongText);
            
            // Extract end GeoCoord.
            infile >> endLatText;
            infile >> endLongText;
            GeoCoord e(endLatText, endLongText);
            infile.ignore(10000, '\n');
            
            // Create two StreetSegments
            StreetSegment seg(s, e, streetName);
            StreetSegment rev(e, s, streetName);
            
            // Get pointers to our StreetSegment vectors within map so that
            // we may push_back our StreetSegments into said vectors
            vector<StreetSegment>* segVec = map.find(s);
            
            
            // Create an association if respective vector does not yet exist,
            // Otherwise, push_back the StreetSegment to the existing vector
            if (segVec != nullptr)
                segVec->push_back(seg);
            else
            {
                vector<StreetSegment> addSeg;
                addSeg.push_back(seg);
                map.associate(s, addSeg);
            }
            
            vector<StreetSegment>* revVec = map.find(e);
            if (revVec != nullptr)
                revVec->push_back(rev);
            else
            {
                vector<StreetSegment> addRev;
                addRev.push_back(rev);
                map.associate(e, addRev);
            }
            
        }
    }
    
    return true;  
}

bool StreetMapImpl::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
    const vector<StreetSegment>* segVec = map.find(gc);
    if (!segVec)
        return false;
    
    // Copy the vector to which gc maps to segs.
    segs = *segVec;
    
    return true;
}


//******************** StreetMap functions ************************************

// These functions simply delegate to StreetMapImpl's functions.
// You probably don't want to change any of this code.

StreetMap::StreetMap()
{
    m_impl = new StreetMapImpl;
}

StreetMap::~StreetMap()
{
    delete m_impl;
}

bool StreetMap::load(string mapFile)
{
    return m_impl->load(mapFile);
}

bool StreetMap::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
   return m_impl->getSegmentsThatStartWith(gc, segs);
}
