//
//  GameState.cpp
//  NYUCodebase
//
//  Created by Cindy Lee on 3/6/18.
//  Copyright © 2018 Ivan Safrin. All rights reserved.
//

#include "GameState.h"
#include "Entity.h"
#include "SheetSprite.h"
#include <vector>

GameState::GameState () {}

GameState::GameState(int sprites) {
    player = Entity (sprites, 0.0f, -2.5f, 0.0f, 112.0, 791.0, 112.0, 75.0f, 0.35f);
    
    float x = 0.0f;
    float y= 1.0f;
    float z = 0.0f;
    enemies [0] = Entity (sprites, x, y, z, 120.0f, 604.0f, 104.0f, 84.0f, 0.30f);
    enemies [0].velocity.y = 2.0;
    
    for (int i = 1; i < 32; i++ ) {
        if (i % 8 == 0) {
            x = 0.0f;
            y += 0.5f;
        }
        else {
            x += 0.6;
        }
        if (i < 8) {
            enemies [i] = Entity (sprites, x, y, z, 120.0f, 604.0f, 104.0f, 84.0f, 0.3f);
        }
        else if (i < 16) {
            enemies [i] = Entity (sprites, x, y, z, 143.0f, 293.0, 104.0f, 84.0f, 0.3f);
            enemies [i].active = false;
        }
        else if ( i < 24) {
            enemies [i] = Entity (sprites, x, y, z, 133.0f, 412.0, 104.0f, 84.0f, 0.3f);
            enemies [i].active = false;
        }
        else {
            enemies [i] = Entity (sprites, x, y, z, 120.0f, 520.0f, 104.0f, 84.0f, 0.3f);
            enemies [i].active = false;
        }
        enemies [i].velocity.y = 3.0;
    }
}


void GameState::shootPlayerBullet(int sprites) {
    Entity playerBullet = Entity (sprites, 0.0f, -1.5f, 0.0f, 856.0f, 421.0f, 9.0f, 54.0f, 0.35f);
    playerBullet.alive = true;
    playerBullet.position.x = player.position.x;  // where the bullet starts X
    playerBullet.position.y = player.position.y;     // where the bullet starts Y
    playerBullet.velocity.y = 2.0f;
    playerBullets.push_back(playerBullet);
}


void GameState::shootEnemyBullet (Entity& enemy, int sprites) {
    if (enemy.active) {
        Entity bullet = Entity (sprites, 0.0f, 0.0f, 0.0f, 858.0f, 230.0f, 9.0f, 54.0f, 0.35f);
        bullet.position.x = enemy.position.x;
        bullet.position.y = enemy.position.y;
        bullet.velocity.y = -2.0f;
        bullets.push_back (bullet);
    }
}

bool GameState::shouldRemoveBullet (Entity thing) {
    return (! (thing.alive)  || thing.position.y - thing.sizeEnt.y/2 >= 3.0f );
}

bool GameState::shouldRemoveEnemyBullet (Entity thing) {
    return (! (thing.alive)  || thing.position.y + thing.sizeEnt.y/2 <= -3.0f );
}

void GameState::Draw (ShaderProgram* program) {
    player.Draw (program);
    for (int i = 0; i < 32; i++){
        if (enemies [i].alive) {
            enemies [i].Draw(program);
        }
    }
    for (int i = 0; i < playerBullets.size() ; i++ ) {
        playerBullets [i].Draw (program);
    }
    for (int i = 0; i < bullets.size() ; i++ ) {
        bullets [i].Draw (program);
    }

}

void GameState::Collision () {
    for (size_t index = 0 ; index < playerBullets.size () ; index++) {
        for (int enemy = 0; enemy < 32; enemy++) {
            if (playerBullets [index].Collision(& (enemies[enemy])) && enemies[enemy].alive && enemies [enemy].active && playerBullets[index].alive) {
                enemies[enemy].alive = false;
                enemies [enemy].active = false;
                if (enemy < 24) {
                    enemies [enemy + 8].active = true;
                }
                playerBullets [index].alive = false;
                break;
            }
        }
    }
    
    for (size_t index = 0 ; index < bullets.size (); index ++) {
        for (size_t index2 = 0; index2 < playerBullets.size (); index2++) {
            if (bullets [index].Collision(& (playerBullets[index2])) && playerBullets[index2].alive && bullets [index].alive){
                bullets[index].alive = false;
                playerBullets[index2].alive = false;
            }
        }
    }
    
    for (size_t index = 0; index < bullets.size (); index++) {
        if (bullets [index].Collision (&player) && bullets[index].alive) {
            bullets [index].alive = false;
            player.alive = false;
            
        }
    }
    playerBullets.erase(std::remove_if(playerBullets.begin(), playerBullets.end(), &GameState::shouldRemoveBullet), playerBullets.end());
    
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), &GameState::shouldRemoveEnemyBullet), bullets.end());
    
}
