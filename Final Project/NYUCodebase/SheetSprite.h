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
    SheetSprite(unsigned int textureID, float u, float v, float width, float height, float size);
    
    void Draw(ShaderProgram *program);
    float size;
    unsigned int textureID;
    int index = 0;
    int spriteCountX = 10;
    int spriteCountY = 10;
    float u = 0.0f;
    float v = 0.0f;
    float width = 0.0f;;
    float height = 0.0f;
};
#endif /* SheetSprite_hpp */
