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
    int GetLevel ();
    void Draw (ShaderProgram* program);
    void Collision ();
    void CollisionX ();
    void CollisionY ();
    
    
    Entity player;
    Entity key;
    std::vector <Entity> enemies;
    FlareMap* mappy;
    
private:
    int sprites = 0;
    int spritePlayer = 0;
    int spriteEnemy = 0;
    int level = 0;
    int lives = 5;
    std::vector <int> solidTiles = {0, 1, 2, 3, 5, 6, 7};
};


#endif /* GameState_hpp */
