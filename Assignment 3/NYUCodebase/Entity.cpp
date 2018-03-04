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

Entity::Entity (float x, float y, float z) : position (Vector3 (x,y,z)) {}

void Entity::Draw(ShaderProgram *program ) {
    Matrix modelMatrix;
    Matrix viewMatrix;
    modelMatrix.Translate (position.x, position.y, position.z);
    program -> SetModelMatrix(modelMatrix);
    program -> SetViewMatrix(viewMatrix);
    sprite.Draw (program);
}
