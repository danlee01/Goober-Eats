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
    struct Node
    {
        KeyType key;
        ValueType val;
        
        Node(KeyType key, ValueType value) : key(key), val(value) {}
    };
    
    std::vector<std::list<Node>*> myHash;
    double maxLoadFactor;
    int nNodes;
    
    void expand();
};

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::ExpandableHashMap(double maximumLoadFactor)
: myHash(8)
{
    this->maxLoadFactor = maximumLoadFactor;
    this->nNodes = 0;
}

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::~ExpandableHashMap()
{
    // Our list will not be using dynamically-allocated Nodes,
    // so all we need to delete are our lists
    for (int i = 0; i < myHash.size(); i++)
    {
        delete myHash[i];
    }
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::reset()
{
    int nBuckets = myHash.size();
    
    for (int i = 0; i < nBuckets; i++)
    {
        delete myHash[i];
        myHash[i] = nullptr;
    }
    myHash.resize(8);
    
    
}

template<typename KeyType, typename ValueType>
int ExpandableHashMap<KeyType, ValueType>::size() const
{
    return myHash.size();
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
    
    // Calculate bucket number
    unsigned int hasher(const KeyType& k);
    unsigned int index = hasher(key) % myHash.size();
    
    // Check to see if bucket is initialized
    // (i.e. Make sure we are not accessing a nullptr)
    
    if (myHash[index] == nullptr)
        myHash[index] = new std::list<Node>;
    
    // If key in bucket, replace ValueType
    
    //typename std::list<Node>::iterator it = myHash[index]->begin();
    //while (it != myHash[index]->end())
    for (auto it = myHash[index]->begin(); it != myHash[index]->end(); it++)
    {
        if (it->key == key)
        {
            it->val = value;
            nNodes++;
            return;
        }
    }
    
    // If key not already in bucket, insert into bucket.
    // If adding this Node will go over our MaxLoadFactor, expand.
    if (maxLoadFactor < (nNodes+1) / myHash.size())
    {
        expand();
        unsigned int hasher(const KeyType& k);
        index = hasher(key) % myHash.size();
    }
    
    nNodes++;
    if (myHash[index] == nullptr)
        myHash[index] = new std::list<Node>;
    
    myHash[index]->push_back(Node(key, value));
    
}

template<typename KeyType, typename ValueType>
const ValueType* ExpandableHashMap<KeyType, ValueType>::find(const KeyType& key) const
{
    unsigned int hasher(const KeyType& k);
    unsigned int index = hasher(key) % myHash.size();
    
    if (myHash[index] == nullptr) return nullptr;
    
    //typename std::list<ValueType>::iterator it = myHash[index]->begin();
    for (auto p = myHash[index]->begin(); p != myHash[index]->end(); p++)
    {
        if (p->key == key)
        {
            return &(p->val);
        }
    }
    
    return nullptr;
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::expand()
{
    int nBuckets = myHash.size();
    
    std::vector<std::list<Node>*> newHash(2*nBuckets);
    
    for (int i = 0; i < nBuckets; i++)
    {
        if (myHash[i] == nullptr) continue;
        
        for (auto p : *myHash[i])
        {
            unsigned int hasher(const KeyType& k);
            int index = hasher(p.key) % (2*nBuckets);
            
            if (newHash[index] == nullptr)
                newHash[index] = new std::list<Node>;
            
            newHash[index]->push_back(Node(p.key, p.val));
            
        }
    }
    // Delete old hash table (free the memory)
    reset();

    myHash = newHash;
    
}



#endif /* ExpandableHashMap_h */





