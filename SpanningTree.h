//
//  Edge.h
//  PAR
//
//  Created by Dominik Vesely on 10/5/12.
//  Copyright (c) 2012 Dominik Vesely,Marek Priban. All rights reserved.
//

#ifndef __PAR__SPT__
#define __PAR__SPT__

#define FRESH 0
#define OPEN 1
#define CLOSED 2

#include <iostream>
#include <vector>
#include "Edge.h"

using namespace std;
class SpanningTree {
    
public:
  //  int maxNodes;
  //  int
  //  int* degrees;
    vector<Edge*> vector;
    char **matrix ;
    int * vertices;
    int * degrees;
    int maxNodes ;
    int currentNodes;
    int degree;
    int edges;
    SpanningTree(int MaxSize);
    ~SpanningTree();
    bool isValid();
    void add(Edge*);
    void remove(Edge*);
    void reset();


    

};

#endif /* defined(__PAR__Edge__) */
