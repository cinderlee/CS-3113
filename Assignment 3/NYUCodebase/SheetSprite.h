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
    SheetSprite(unsigned int textureID, float u, float v, float width, float height, float
                size);

    void Draw(ShaderProgram *program);
    float size;
    unsigned int textureID;
    float u;
    float v;
    float width;
    float height;
};
#endif /* SheetSprite_hpp */
