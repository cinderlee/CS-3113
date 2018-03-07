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
#define MAX_ENEMIES 32
GameState::GameState () {}

// initializing player and enemies
GameState::GameState(int sprites) {
    player = Entity (sprites, 0.0f, -2.5f, 0.0f, 112.0, 791.0, 112.0, 75.0f, 0.35f);
    
    float x = 0.0f;
    float y= 0.0f;
    float z = 0.0f;
    
    for (int i = 0; i < MAX_ENEMIES; i++ ) {
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
        enemies [i].velocity.y = 9.0;
    }
}

// shooting a bullet from player
void GameState::shootPlayerBullet(int sprites) {
    Entity playerBullet = Entity (sprites, 0.0f, -1.5f, 0.0f, 856.0f, 421.0f, 9.0f, 54.0f, 0.35f);
    playerBullet.alive = true;
    playerBullet.position.x = player.position.x;
    playerBullet.position.y = player.position.y + player.sizeEnt.y/2;     
    playerBullet.velocity.y = 2.0f;
    playerBullets.push_back(playerBullet);
}


// enemey shooting bullet
void GameState::shootEnemyBullet (Entity& enemy, int sprites) {
    if (enemy.active) {
        Entity bullet = Entity (sprites, 0.0f, 0.0f, 0.0f, 858.0f, 230.0f, 9.0f, 54.0f, 0.35f);
        bullet.position.x = enemy.position.x;
        bullet.position.y = enemy.position.y - enemy.sizeEnt.y/2;
        bullet.velocity.y = -2.5f;
        bullets.push_back (bullet);
    }
}

//removing player bullet
bool GameState::shouldRemoveBullet (Entity thing) {
    return (! (thing.alive)  || thing.position.y - thing.sizeEnt.y/2 >= 3.0f );
}

// removing enemy bullet
bool GameState::shouldRemoveEnemyBullet (Entity thing) {
    return (! (thing.alive)  || thing.position.y + thing.sizeEnt.y/2 <= -3.0f );
}

// drawing game
void GameState::Draw (ShaderProgram* program) {
    player.Draw (program);
    for (int i = 0; i < MAX_ENEMIES; i++){
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

// checking for any collisions in game
void GameState::Collision () {
    for (size_t index = 0 ; index < playerBullets.size () ; index++) {
        for (int enemy = 0; enemy < MAX_ENEMIES; enemy++) {
            
            // collision between enemy and player bullet
            if (playerBullets [index].Collision(& (enemies[enemy])) && enemies[enemy].alive && enemies [enemy].active && playerBullets[index].alive) {
                enemies[enemy].alive = false;
                enemies [enemy].active = false;
                if (enemy < 24) {
                    enemies [enemy + 8].active = true;
                }
                playerBullets [index].alive = false;
                
                if (enemy < 8) {
                    score += 100;
                }
                else if (enemy < 16) {
                    score += 200;
                }
                else if (enemy < 24) {
                    score += 300;
                }
                else {
                    score += 400;
                        
                }
                break;
            }
            
            // if bullet hit an enemy not in first line of defense, doesn't count
            else if (playerBullets [index].Collision(& (enemies[enemy])) && enemies[enemy].alive && playerBullets[index].alive) {
                playerBullets [index].alive = false;
                break;
            }
        }
    }
    
    // check for collision between player bullet and enemy bullet
    for (size_t index = 0 ; index < bullets.size (); index ++) {
        for (size_t index2 = 0; index2 < playerBullets.size (); index2++) {
            if (bullets [index].Collision(& (playerBullets[index2])) && playerBullets[index2].alive && bullets [index].alive){
                bullets[index].alive = false;
                playerBullets[index2].alive = false;
            }
        }
    }
    
    // check for collision between player and enemey bullet
    for (size_t index = 0; index < bullets.size (); index++) {
        if (bullets [index].Collision (&player) && bullets[index].alive) {
            bullets [index].alive = false;
            if (lives > 1 ) {
                lives -= 1;
            }
            else {
                player.alive = false;
            }
        }
    }
    
    // check for collision between enemy and player
    for (size_t index = 0; index < MAX_ENEMIES; index ++ ) {
        if (enemies [index].Collision(&player) && enemies [index].alive) {
            player.alive = false;
        }
    }
    
    // removing bullets from vector
    playerBullets.erase(std::remove_if(playerBullets.begin(), playerBullets.end(), &GameState::shouldRemoveBullet), playerBullets.end());
    
    //removing enemy bullets from vector
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), &GameState::shouldRemoveEnemyBullet), bullets.end());
    
}

// check to see if game is over, whether player won or lost
bool GameState::gameOver () {
    bool test = true;
    for (int index =0 ; index < MAX_ENEMIES; index++) {
        if (enemies [index].alive) {
            test = false;
        }
    }
    return (test || ! (player.alive));
}


// resetting game 
void GameState::reset () {
    player.position = Vector3 (0.0f, -2.5f, 0.0f);
    player.alive = true;
    float x = 0.0f;
    float y= 0.0f;
    float z = 0.0f;
    
    for (int i = 0; i < MAX_ENEMIES; i++ ) {
        if (i % 8 == 0) {
            x = 0.0f;
            y += 0.5f;
        }
        else {
            x += 0.6;
        }
        enemies [i].position = Vector3 (x, y, z);
        enemies [i].alive = true;
        enemies [i].active = true;
        if (i >= 8 && i < MAX_ENEMIES) {
            enemies [i].active = false;
        }
    }
    score = 0;
    lives = 3;
    playerBullets.clear ();
    bullets.clear();
}
