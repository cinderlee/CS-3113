//
//  Entity.cpp
//  NYUCodebase
//
//  Created by Cindy Lee on 3/4/18.
//  Copyright © 2018 Ivan Safrin. All rights reserved.
//

#include "Entity.h"
#include "ShaderProgram.h"
#include "SheetSprite.h"

Entity::Entity () {}

// creating an entity
Entity::Entity (int sprites, float x, float y, float z, float spriteX, float spriteY, float width, float height, float size) : position (Vector3 (x,y,z)), sizeEnt (Vector3 (size * width/height , size, 0.0f)) {
    sprite = SheetSprite (sprites, spriteX/1024.0f, spriteY/1024.0f, width/1024.0f, height/1024.0f, size);
}

// drawing an entity
void Entity::Draw(ShaderProgram *program ) {
        Matrix modelMatrix;
        modelMatrix.Translate (position.x, position.y, position.z);
        program -> SetModelMatrix(modelMatrix);
        sprite.Draw (program);
}

//checking for entity collision
bool Entity::Collision (Entity* other){
    if (!  (position.y - sizeEnt.y/2 > other->position.y + other->sizeEnt.y / 2 ||
           position.y + sizeEnt.y/2 < other->position.y - other->sizeEnt.y/2 ||
           position.x - sizeEnt.x/2 > other->position.x + other->sizeEnt.x/2 ||
           position.x + sizeEnt.x/2 < other->position.x - other->sizeEnt.x/2) ) {
        return true;
    }
    return false;
}


