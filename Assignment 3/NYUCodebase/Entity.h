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
    Entity (int sprites, float x, float y, float z, float spriteX, float spriteY, float width, float height, float size);
    void Draw(ShaderProgram* program);
    void Update(float elapsed);
    bool Collision (Entity* other);
    
    
    Vector3 position = Vector3 (0.0f, 0.0f, 0.0f);
    Vector3 velocity = Vector3 (1.0f, 0.0f, 0.0f);
    Vector3 sizeEnt = Vector3 (0.0f, 0.0f, 0.0f);
    
    float rotation;
    SheetSprite sprite;
    bool alive = true;
    bool active = true;
};



#endif /* Entity_hpp */
