//
//  SheetSprite.hpp
//  NYUCodebase
//
//  Created by Cindy Lee on 3/4/18.
//  Copyright © 2018 Ivan Safrin. All rights reserved.
//

#ifndef SheetSprite_hpp
#define SheetSprite_hpp

#include <stdio.h>
#include "ShaderProgram.h"

class SheetSprite {
public:
    SheetSprite();
    SheetSprite(int index, unsigned int textureID, float size);

    void Draw(ShaderProgram *program);
    float size;
    unsigned int textureID;
    int index = 0;
    int spriteCountX = 16;
    int spriteCountY = 8; 
    float u = 0.0f;
    float v = 0.0f;
    float width;
    float height;
};
#endif /* SheetSprite_hpp */
