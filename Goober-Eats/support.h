//
//  support.h
//  Goober-Eats
//
//  Created by Dan Lee on 3/9/20.
//  Copyright © 2020 Dan Lee. All rights reserved.
//

#ifndef support_h
#define support_h

#include <climits>

struct Score
{
    double score;
    
    Score(double score=INT_MAX) : score(score) {}
};

bool operator<(const Score& lhs, const Score& rhs)
{
    return lhs.score < rhs.score;
}




#endif /* support_h */
