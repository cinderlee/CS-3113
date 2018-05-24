//
//  SheetSprite.cpp
//  NYUCodebase
//
//  Created by Cindy Lee on 3/4/18.
//  Copyright © 2018 Ivan Safrin. All rights reserved.
//

#include "SheetSprite.h"
#include "ShaderProgram.h"

SheetSprite::SheetSprite() {};

// creating a sprite
SheetSprite::SheetSprite(int index, unsigned int textureID, float size): index (index),
                        textureID(textureID), size(size) {};

SheetSprite::SheetSprite(unsigned int textureID, float u, float v, float width, float height, float size): textureID(textureID), u(u), v(v), width(width), height (height), size(size) {};
// drawing the sprite

void SheetSprite::Draw(ShaderProgram *program) {
    glBindTexture(GL_TEXTURE_2D, textureID);
   
    float texCoords[] = {
        u, v+height,
        u+width, v,
        u, v,
        u+width, v,
        u, v+height,
        u+width, v+height
    };
    
    float aspect = width/height;
    float vertices[] = {
        -0.5f * size * aspect , -0.5f * size,
        0.5f * size * aspect, 0.5f * size,
        -0.5f * size * aspect , 0.5f * size,
        0.5f * size * aspect, 0.5f * size,
        -0.5f * size* aspect, -0.5f * size ,
        0.5f * size * aspect, -0.5f * size};
    
    glVertexAttribPointer(program -> positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program -> positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
    
}
