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
Entity::Entity (int sprites, int index, float x, float y, float z, float sizeX, float sizeY) : position (Vector3 (x,y,z)), sizeEnt (Vector3 (sizeX, sizeY, 0.0f)) {
    sprite = SheetSprite (index, sprites, sizeX, sizeY );
}

Entity::Entity (int sprites, float x, float y, float z, float spriteX, float spriteY, float width, float height, float sizeX, float sizeY) : position (Vector3 (x,y,z)), sizeEnt (Vector3 (sizeX * width/height , sizeY, 0.0f)) {
    sprite = SheetSprite (sprites, spriteX, spriteY, width, height, sizeX, sizeY);
}
// drawing an entity
void Entity::Draw(ShaderProgram *program ) {
        modelMatrix.Identity();

    modelMatrix.Scale (direction, 1.0f, 1.0f);
        modelMatrix.SetPosition(position.x, position.y, position.z);
    if (rotation != 0.0f) {
        modelMatrix.Rotate(rotation);
    }
        program -> SetModelMatrix(modelMatrix);
        sprite.Draw (program);
}

// calculating distance horizontally to another entity
float Entity::DistanceToX (Entity* other) {
    return position.x - other -> position.x;
}

// calculating distance vertically to another entity
float Entity::DistanceToY (Entity* other) {
    return position.y - other -> position.y;
}

// calculating distance to another entity 
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

bool Entity::CollisionPlatformX (Entity* other){
    if (Collision (other)) {
        if ( (position.x - sizeEnt.x/2 < other->position.x + other->sizeEnt.x / 2 ) &&
             !(position.x > other->position.x - other->sizeEnt.x/2 ) ) {
            position.x = other -> position.x + other -> sizeEnt.x/2  + this -> sizeEnt.x/2;
            collidedLeft = true;
            std:: cout << "HAPPENED2" << std::endl;
            return true;
        }
        else if ( (position.x + sizeEnt.x/2 > other -> position.x - other -> sizeEnt.x/2) &&
                 !( position.x  < other->position.x + other->sizeEnt.x/2 ) ) {
            position.x = other -> position.x - other -> sizeEnt.x/2 - this -> sizeEnt.x/2;
            collidedRight = true;
            std:: cout << "HAPPENED" << std::endl;
            return true;
        }
    }
    return false;
}

bool Entity::CollisionPlatformY (Entity* other) {
    if (Collision (other)) {
        if (position.y - sizeEnt.y/2 <= other->position.y + other->sizeEnt.y / 2) {
            return true;
        }
        else if (position.y + sizeEnt.y/2 >= other -> position.y - other -> sizeEnt.y/2) {
            return true;
        }
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
    collidedPlatform = false;
}

