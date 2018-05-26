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
    void Initiate (int spriteTiles, int spritesterPlayer, int spritesterEnemy);
    void LoadLevel ();
    void UpdateLevel ();
    void Update ();
    void Draw (ShaderProgram* program);
    void Collision ();
    
    
    Entity player;
    Entity key;
    std::vector <Entity> enemies;
    int sprites = 0;
    int spritePlayer = 0;
    int spriteEnemy = 0;
    FlareMap* mappy;
    
private:
    int level = 0;
    int lives = 5;
};


#endif /* GameState_hpp */
