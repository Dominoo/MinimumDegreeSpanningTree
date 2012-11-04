//
//  Edge.h
//  PAR
//
//  Created by Dominik Vesely on 10/5/12.
//  Copyright (c) 2012 Dominik Vesely,Marek Priban. All rights reserved.
//

#ifndef __PAR__Edge__
#define __PAR__Edge__

#include <iostream>

class Edge {
       
public:
   // Edge();  // parameterless default constructor
    Edge(int fromNode, int toNode); //  constructor with parameters
    Edge(int fromNode, int toNode, bool vis); //  constructor with parameters

    int from;
    int to;
    bool visited;

};

#endif /* defined(__PAR__Edge__) */
