//
//  Shape.cpp
//  NYUCodebase
//
//  Created by Cindy Lee on 4/4/18.
//  Copyright © 2018 Ivan Safrin. All rights reserved.
//

#include "Shape.h"
#include <vector>
#include "Vector3.h"

Rectangle::Rectangle (float width, float height): Shape (width, height)  {
        
        float verticesUN [] = {0- width/2, height/2,  0 - width/2,  0- height/2,  width/2, 0 - height/2, 0- width/2, height/2, width/2, 0- height/2,  width/2, height/2};
        for (int i = 0; i < 12; i++){
            vertices.push_back (verticesUN [i]);
        }
        for (int i = 0; i < 6; i+=2) {
            corner.push_back ( Vector3 (vertices [i], vertices[i+1], 0.0f) );
            
        }
        corner.push_back ( Vector3 (vertices [10], vertices [11], 0.0f));
    }
    
std::vector<Vector3>& Rectangle::cornerVertices () {
        
        return corner;
    }
    
std::vector <float>& Rectangle::triVertices () {
        return vertices;
    }
    
