//
//  Entity.cpp
//  NYUCodebase
//
//  Created by Cindy Lee on 4/4/18.
//  Copyright © 2018 Ivan Safrin. All rights reserved.
//

#include "Entity.h"
#include "Matrix.h"
#include "Vector3.h"
#include "ShaderProgram.h"

void Entity::Draw (ShaderProgram *program) {
        program -> SetColor(red, green, blue, 1.0f);
        glUseProgram((*program).programID);
        modelMatrix.Identity();
        
        modelMatrix.Translate(objectPosition.x, objectPosition.y, 0.0f);
        modelMatrix.Rotate(rotator);
        modelMatrix.Scale (scale.x, scale.y, 1.0f);
        
        program ->SetModelMatrix(modelMatrix);
        glVertexAttribPointer(program -> positionAttribute, 2, GL_FLOAT, false, 0, item.triVertices().data ());
        glEnableVertexAttribArray(program -> positionAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(program->positionAttribute);
        
    }
    

