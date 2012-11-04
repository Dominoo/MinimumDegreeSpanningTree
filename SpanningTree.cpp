//
//  Edge.cpp
//  PAR
//
//  Created by Dominik Vesely on 10/5/12.
//  Copyright (c) 2012 Dominik Vesely,Marek Priban. All rights reserved.
//

#include "SpanningTree.h"

#pragma once

SpanningTree::SpanningTree(int size) {
    maxNodes = size;
    vertices = (int*) calloc(size, sizeof(int)*size);
    degrees = (int*) calloc(size, sizeof(int)*size);
    currentNodes = degree = edges = 0;
    matrix = (char**) calloc(size, sizeof (char*));
    for(int i = 0 ; i < size ; i++) {
        matrix[i] = (char*) calloc(size, sizeof (char));
    }
    currentNodes++;
    
}

SpanningTree::~SpanningTree() {
    free(vertices);
    free(degrees);
    for(int i = 0 ; i < maxNodes ; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

void SpanningTree::add(Edge * edge ){
   // if(edge->from > maxNodes) return;
    matrix[edge->from][edge->to] = 1;
    matrix[edge->to][edge->from] = 1;
    vertices[edge->to] = OPEN;
    degrees[edge->from]++;
    degrees[edge->to]++;
    
    if(degrees[edge->from] > degree) {
        degree = degrees[edge->from];
    }
    
    if(degrees[edge->to] > degree) {
        degree = degrees[edge->to];
    } 
    
    edges++;
    currentNodes++;
    vector.push_back(edge);
}

void SpanningTree::remove(Edge * edge ){
    matrix[edge->from][edge->to] = 0;
    matrix[edge->to][edge->from] = 0;
    vertices[edge->to] = FRESH;
    degrees[edge->from]--;
    degrees[edge->to]--;
    
    int max = 0;    
    for(int i = 0; i < maxNodes; i++) {
        if(degrees[i] > max) {
            max = degrees[i];
        }
    }
    
    degree = max;
    edges--;
    currentNodes--;
    vector.pop_back();
}

bool SpanningTree::isValid() {
    return (maxNodes == currentNodes && edges == (maxNodes -1));
}

