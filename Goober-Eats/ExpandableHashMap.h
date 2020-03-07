//
//  ExpandableHashMap.h
//  Goober-Eats
//
//  Created by Dan Lee on 3/5/20.
//  Copyright © 2020 Dan Lee. All rights reserved.
//

#ifndef ExpandableHashMap_h
#define ExpandableHashMap_h

#include <vector>
#include <list>

// ExpandableHashMap.h

// Skeleton for the ExpandableHashMap class template.  You must implement the first six
// member functions.

template<typename KeyType, typename ValueType>
class ExpandableHashMap
{
public:
    ExpandableHashMap(double maximumLoadFactor = 0.5);
    ~ExpandableHashMap();
    void reset(); // resets the hashmap back to 8 buckets, deletes all items
    int size() const;
    void associate(const KeyType& key, const ValueType& value);
    
    // for a map that can't be modified, return a pointer to const ValueType
    const ValueType* find(const KeyType& key) const;
    
    // for a modifiable map, return a pointer to modifiable ValueType
    ValueType* find(const KeyType& key)
    {
        return const_cast<ValueType*>(const_cast<const ExpandableHashMap*>(this)->find(key));
    }
    
    // C++11 syntax for preventing copying and assignment
    ExpandableHashMap(const ExpandableHashMap&) = delete;
    ExpandableHashMap& operator=(const ExpandableHashMap&) = delete;
    
private:
    std::vector<list<Node>*> myHash;
    double maxLoadFactor;
    
    struct Node
    {
        typename KeyType;
        typename ValueType;
    }
};

ExpandableHashMap::ExpandableHashMap(double maximumLoadFactor)
: myHash(8)
{
    this->maxLoadFactor = maximumLoadFactor;
}

ExpandableHashMap::~ExpandableHashMap()
{
    for (int i = 0; i < myHash.size(); i++)
    {
        std::list<ValueType>::iterator it = myHash[i]->begin();
        for ( ; j < listSize; )
        {
            it = myHash[i]->erase(it);
        }
        delete myHash[i];
    }
}

void ExpandableHashMap::reset()
{
}

int ExpandableHashMap::size() const
{
    return myHash.size();
}

void ExpandableHashMap::associate(const KeyType& key, const ValueType& value)
{
    
}

const ValueType* ExpandableHashMap::find(const KeyType& key) const
{
    return nullptr;  // Delete this line and implement this function correctly
}



#endif /* ExpandableHashMap_h */
