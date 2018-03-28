//
//  GameState.hpp
//  NYUCodebase
//
//  Created by Cindy Lee on 3/6/18.
//  Copyright © 2018 Ivan Safrin. All rights reserved.
//

#ifndef GameState_hpp
#define GameState_hpp

#include <vector>
#include <stdio.h>
#include "Entity.h"
#include "SheetSprite.h"
#include "FlareMap.h"


class GameState {
public:
    GameState ();
    void Initiate (int sprites, const std::string fileName);
    void Draw (ShaderProgram* program);
    void Collision ();
    
    
    Entity player;
    Entity key;
    std::vector <Entity> enemies;
    int sprites = 0;
    FlareMap mappy;
};


#endif /* GameState_hpp */
