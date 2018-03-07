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

class GameState {
public:
    GameState ();
    GameState(int sprites);
    void Draw (ShaderProgram* program);
    bool static shouldRemoveBullet (Entity thing);
    void shootPlayerBullet(int sprites);
    void shootEnemyBullet (Entity& enemy, int sprites);
    bool static shouldRemoveEnemyBullet (Entity thing);
    void Collision ();
    
    
    Entity player;
    std::vector <Entity> playerBullets;
    Entity enemies[32];
    std::vector <Entity> bullets;
    int score = 0;
};


#endif /* GameState_hpp */
