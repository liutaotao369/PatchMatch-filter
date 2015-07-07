/********************************************************
 *                                                      *
 * Efficient Subwindow Search (ESS) implemention in C++ *
 * entry point and commmand line interface              *
 *                                                      *
 *  Performs a branch-and-bound search for an arbitrary *
 *  quality function, provided a routine to bound it,   *
 *  see  C. H. Lampert, M. B. Blaschko and T. Hofmann:  *
 *       Beyond Sliding Windows: Object Localization    *
 *       by Efficient Subwindow Search, CVPR (2008)     *
 *                                                      *
 *   Copyright 2006-2008 Christoph Lampert              *
 *   Contact: <mail@christoph-lampert.org>              *
 *                                                      *
 *  Licensed under the Apache License, Version 2.0 (the *
 *  "License"); you may not use this file except in     *
 *  compliance with the License. You may obtain a copy  *
 *  of the License at                                   *
 *                                                      *
 *     http://www.apache.org/licenses/LICENSE-2.0       *
 *                                                      *
 *  Unless required by applicable law or agreed to in   *
 *  writing, software distributed under the License is  *
 *  distributed on an "AS IS" BASIS, WITHOUT WARRANTIES *
 *  OR CONDITIONS OF ANY KIND, either express or        *
 *  implied. See the License for the specific language  *
 *  governing permissions and limitations under the     *
 *  License.                                            *
 *                                                      *
 ********************************************************/

#ifndef _ESS_H
#define _ESS_H

#include <stdio.h>
#include <limits>
#include <string>
#include <queue>

#include "helper.hh"

// structure holding a single box
typedef struct { 
        int left;
        int top;
        int right;
        int bottom;
        double score;
} Box;

// structure to hold a set of boxes ( = a state during search)
class sstate {
public:
        float upper; 
        short low[4];   // there can be millions of sstates, better save some memory
        short high[4];

        // construct empty
        sstate() { }
    
        // construct as full search space
        sstate(int argwidth, int argheight, int qbits=0) {
                upper = std::numeric_limits<float>::max();
                low[0] = 0; // no padding
                low[1] = 0;
                low[2] = 0;
                low[3] = 0;
                // this is quantised. if qbits is zero, it takes the image size, otherwise it rounds it to a multiple of 2^qbits
                high[0] = (((argwidth-1)>>qbits)<<qbits);
                high[1] = (((argheight-1)>>qbits)<<qbits);
                high[2] = (((argwidth-1)>>qbits)<<qbits);
                high[3] = (((argheight-1)>>qbits)<<qbits);
        }

        std::string tostring() const {
                char cstring[80];
                sprintf(cstring, "low < %d %d %d %d > high < %d %d %d %d >\n", 
                        low[0],low[1],low[2],low[3],high[0],high[1],high[2],high[3]);
                return std::string(cstring);
        }
        
        // calculate argmax_i( high[i]-low[i] ), or -1 if high==low
        int maxindex(const int qbits) const { 
                int splitindex=-1;
                int maxwidth=0;
                for (unsigned int i=0;i<4;i++) {
                        // comparison only to the precision of 2^qbits
                        int interval_width = (high[i]>>qbits) - (low[i]>>qbits);
                        //BLINK(0) << i << " " << interval_width << " ";
                        if (interval_width > maxwidth) {
                                splitindex=i;
                                maxwidth = interval_width;
                        }
                }
                //BLINK(0) << std::endl;
                
                return splitindex;
        }

        bool islegal() const {
                return ((low[0] <= high[2]) && (low[1] <= high[3]));
        }

        // "less" is needed to compare states in stl_priority queue
        bool less(const sstate* other) const {
                return upper < other->upper;
        }
};


// helper routine for comparing elements in priority queue
class sstate_comparisson {
public:
        bool operator() (const sstate* lhs, const sstate* rhs) const {
                return lhs->less(rhs);
        }
};

// data structure for priority queue
typedef std::priority_queue<const sstate*, 
                            std::vector<const sstate*>, 
                            sstate_comparisson> sstate_heap;


extern "C" {
        Box pyramid_search(int W, int H, const double * SSH,    int DS1, int DS2, const double *weights, int qbits=0, int verbose_arg=0);
}

#endif
