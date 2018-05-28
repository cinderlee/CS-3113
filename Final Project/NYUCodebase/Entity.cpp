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
#include <math.h> 

Entity::Entity () {}

// creating an entity
Entity::Entity (int sprites, int index, float x, float y, float z, float size) : position (Vector3 (x,y,z)), sizeEnt (Vector3 (size, size, 0.0f)) {
    sprite = SheetSprite (index, sprites, size ); 
}

Entity::Entity (int sprites, float x, float y, float z, float spriteX, float spriteY, float width, float height, float size) : position (Vector3 (x,y,z)), sizeEnt (Vector3 (size * width/height , size, 0.0f)) {
    sprite = SheetSprite (sprites, spriteX, spriteY, width, height, size);
}
// drawing an entity
void Entity::Draw(ShaderProgram *program ) {
        modelMatrix.Identity();
        modelMatrix.SetPosition(position.x, position.y, position.z);
        program -> SetModelMatrix(modelMatrix);
        sprite.Draw (program);
}

float Entity::DistanceToX (Entity* other) {
    return position.x - other -> position.x;
}

float Entity::DistanceToY (Entity* other) {
    return position.y - other -> position.y;
}

float Entity::DistanceTo (Entity* other) {
    float distancex = DistanceToX(other);
    float distancey = DistanceToY (other);
    distancex *= distancex;
    distancey *= distancey;
    return sqrt (distancex + distancey);
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


//converting to Tile coordinates
void Entity::worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY) {
    *gridX = (int)(worldX / 0.3f);
    *gridY = (int)(-worldY / 0.3f);
}

void Entity::collisionBools () {
    collidedLeft = false;
    collidedRight = false;
    collidedTop = false;
    collidedBottom = false;
}

