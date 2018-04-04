//
//  Entity.hpp
//  NYUCodebase
//
//  Created by Cindy Lee on 4/4/18.
//  Copyright © 2018 Ivan Safrin. All rights reserved.
//

#ifndef Entity_hpp
#define Entity_hpp

#include <stdio.h>
#include "Shape.h"
#include "Vector3.h"
#include "Matrix.h"
#include "ShaderProgram.h"

class Entity {
public:
    Entity (Rectangle itemster) : item (itemster) {
    }
    
    void Draw (ShaderProgram *program);
    Rectangle item;
    Vector3 velocity = Vector3(1.0f, 1.0f, 0.0f);
    Matrix modelMatrix;
    
    float rotator = 0.0f;
    Vector3 scale = Vector3 (1.0f, 1.0f, 0.0f);
    Vector3 objectPosition = Vector3 (0.0f, 0.0f, 0.0f);
    
    float red = 1.0f;
    float blue = 1.0f;
    float green = 1.0f;
    
    bool topBar = false;
    bool bottomBar = false;
    bool leftBar = false;
    bool rightBar = false;
    
};

#endif /* Entity_h */
