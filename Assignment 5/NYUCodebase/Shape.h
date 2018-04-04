//
//  Shape.hpp
//  NYUCodebase
//
//  Created by Cindy Lee on 4/4/18.
//  Copyright © 2018 Ivan Safrin. All rights reserved.
//

#ifndef Shape_h
#define Shape_h

#include <stdio.h>
#include <vector>
#include "Vector3.h"

class Shape {
public:
    Shape (float width, float height) :width (width), height (height) {}
    virtual std::vector<Vector3>& cornerVertices () = 0;
    virtual std::vector <float>& triVertices () = 0;
    std::vector <float> vertices;
    std::vector <Vector3> corner;
    float width;
    float height;
};

class Rectangle: public Shape{
public:
    Rectangle (float width, float height);
    
    std::vector<Vector3>& cornerVertices ();
    
    std::vector <float>& triVertices ();
    
};



#endif /* Shape_h */
