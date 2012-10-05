//
//  Edge.cpp
//  PAR
//
//  Created by Dominik Vesely on 10/5/12.
//  Copyright (c) 2012 Dominik Vesely,Marek Priban. All rights reserved.
//

#include "Edge.h"

Edge::Edge() {
    from = 0;
    to = 0;
}

Edge::Edge(int fromNode, int toNode) {
    from = fromNode;
    to = toNode;
}
