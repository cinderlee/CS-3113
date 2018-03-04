//
//  Entity.hpp
//  NYUCodebase
//
//  Created by Cindy Lee on 3/4/18.
//  Copyright © 2018 Ivan Safrin. All rights reserved.
//

#ifndef Entity_hpp
#define Entity_hpp

#include <stdio.h>
#include "SheetSprite.h"
#include "ShaderProgram.h"
#include "Vector3.h"
class Entity {
public:
    Entity ();
    Entity (float x, float y, float z);
    void Draw(ShaderProgram* program);
    void Update(float elapsed);
    Vector3 position = Vector3 (0.0f, 0.0f, 0.0f);
    Vector3 velocity = Vector3 (1.0f, 0.0f, 0.0f);
    //Vector3 size;
    float rotation;
    SheetSprite sprite;
};



#endif /* Entity_hpp */
