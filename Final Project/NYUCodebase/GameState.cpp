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
#include "FlareMap.h"
#include "ShaderProgram.h"
#include <vector>
#include <string>
#include <math.h>
#define TILE_SIZE 0.3f

GameState::GameState () {}

float GameState::lerp(float v0, float v1, float t) {
    return (1.0-t)*v0 + t*v1;
}

// initializing player and enemies
void GameState::Initiate(int spriteTiles, int spritesterPlayer, int spritesterEnemy, int textureID){
    sprites = spriteTiles;
    spritePlayer = spritesterPlayer;
    spriteEnemy = spritesterEnemy;
    partTexture = textureID;
    partSystem = ParticleEmitter (partTexture, 30, 0.0f, 0.0f );
    LoadLevel();
}

// load map and entities of the map
void GameState::LoadLevel () {
    if (mappy != nullptr) {
        delete mappy;
    }
    mappy = new FlareMap();
    enemies.clear ();
    platforms.clear ();
    powerUp.clear ();
    family.clear ();
    std::stringstream stream;
    stream << "NYUCodebase.app/Contents/Resources/Resources/FinalLevel" << level << ".txt";
    mappy -> Load (stream.str());
    for (size_t index = 0; index < mappy -> entities.size (); index++ ){
        //create the player
        if (mappy -> entities [index].type == "playerRed" && ( level == 0 || level == 1 || level == 4) ){
        
            if (level == 4  && lives == 0) {
                player = Entity (spritePlayer, (mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 850/1024.0f, 297/1024.0f, 39/1024.0f, 46/1024.0f, TILE_SIZE, TILE_SIZE);
            }
            if (level == 4 && lives != 0) {
                player = Entity (spritePlayer, (mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 650/1024.0f, 685/1024.0f, 56/1024.0f, 38/1024.0f, TILE_SIZE, TILE_SIZE);
            }
            if (level == 0 || level == 1) {
                player = Entity (spritePlayer, (mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 850/1024.0f, 518/1024.0f, 39/1024.0f, 48/1024.0f, TILE_SIZE, TILE_SIZE);
                player.gravity.y = -3.5f;
                player.type = "playerRed";
            }
        }
        
        // set position of level 2 and 3
        if (level != 1) {
            if (mappy -> entities [index].type == "playerBlue" ) {
                player.position = Vector3 ((mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f);
            }
            if (mappy -> entities [index].type == "playerGreen") {
                player.position = Vector3((mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f);
            }
        }
        
        // creating the family members for level 3
        // lives = 0 - use dead form
        // else - use alive form
        if (mappy -> entities [index].type == "redFamily" && lives > 0) {
            family.push_back (Entity (spritePlayer, (mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 850/1024.0f, 518/1024.0f, 39/1024.0f, 48/1024.0f, TILE_SIZE, TILE_SIZE));
            family [family.size () - 1].velocity = Vector3 (0.5, 0.0, 0.0);
            family [family.size () - 1].direction = 1.0f;
            family [family.size () - 1].type = "family";
        }
        
        if (mappy -> entities [index].type == "redFamily" && lives == 0) {
            family.push_back (Entity (spritePlayer, (mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 850/1024.0f, 297/1024.0f, 39/1024.0f, 46/1024.0f, TILE_SIZE, TILE_SIZE));
            family [family.size () - 1].velocity = Vector3 (0.0, 0.0, 0.0);
            family [family.size () - 1].direction = 1.0f;
            family [family.size () - 1].type = "family";
        }
        
        if (mappy -> entities [index].type == "blueFamily" && lives > 0) {
            family.push_back (Entity (spritePlayer, (mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 762/1024.0f, 203/1024.0f, 45/1024.0f, 54/1024.0f, TILE_SIZE, TILE_SIZE));
            family [family.size () - 1].velocity = Vector3 (-0.5, 0.0, 0.0);
            family [family.size () - 1].direction = -1.0f;
            family [family.size () - 1].type = "family";
        }
        
        if (mappy -> entities [index].type == "blueFamily" && lives == 0) {
            family.push_back (Entity (spritePlayer, (mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 762/1024.0f, 258/1024.0f, 45/1024.0f, 47/1024.0f, TILE_SIZE, TILE_SIZE));
            family [family.size () - 1].velocity = Vector3 (0.0, 0.0, 0.0);
            family [family.size () - 1].direction = -1.0f;
            family [family.size () - 1].type = "family";
        }
        
        if (mappy -> entities [index].type == "greenFamily" && lives > 0) {
            family.push_back (Entity (spritePlayer, (mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 890/1024.0f, 0/1024.0f, 38/1024.0f, 50/1024.0f, TILE_SIZE, TILE_SIZE));
            family [family.size () - 1].velocity = Vector3 (0.25, 0.0, 0.0);
            family [family.size () - 1].direction = 1.0f;
            family [family.size () - 1].type = "family";
        }
        
        if (mappy -> entities [index].type == "greenFamily" && lives == 0) {
            family.push_back (Entity (spritePlayer, (mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 890/1024.0f, 272/1024.0f, 38/1024.0f, 43/1024.0f, TILE_SIZE, TILE_SIZE));
            family [family.size () - 1].velocity = Vector3 (0.0, 0.0, 0.0);
            family [family.size () - 1].direction = 1.0f;
            family [family.size () - 1].type = "family";
        }
        
        // creating the enemies - categorized into enemyGround, enemyAir, ghost, smasher
        
        // walking enemy - enemyGround
        if (mappy -> entities [index].type == "enemyWalking" ) {
            enemies.push_back (Entity (spritePlayer, (mappy -> entities [index].x + 0.5) * TILE_SIZE, (mappy -> entities[index].y + 0.5) * -1 * TILE_SIZE, 0.0f, 928.0f/1024.0f, 949/1024.0f, 32/1024.0f, 44/1024.0f, TILE_SIZE, TILE_SIZE));
            enemies [enemies.size () - 1].velocity.x = 0.5f;
            enemies[enemies.size () - 1].type = "enemyGround";
        }
        
        // spider enemy - enemyGround
        if (mappy -> entities [index].type == "spider") {
            enemies.push_back (Entity (spriteEnemy, (mappy -> entities [index].x + 0.5) * TILE_SIZE, (mappy -> entities[index].y + 0.5) * -1 * TILE_SIZE, 0.0f, 0.0f, 326/512.0f, 71/1024.0f, 45/512.0f, TILE_SIZE * 2, TILE_SIZE));
            enemies [enemies.size () - 1].velocity.x = 0.5f;
            enemies [enemies.size () - 1].direction = -1.0f;
            enemies[enemies.size () - 1].type = "enemyGround";
        }
        
        // flying enemy - enemyAir
        if (mappy -> entities [index].type == "enemyFlying") {
            enemies.push_back (Entity (spritePlayer, (mappy -> entities [index].x + 0.5) * TILE_SIZE, (mappy -> entities[index].y + 0.5) * -1 * TILE_SIZE, 0.0f, 706.0f/1024.0f, 839/1024.0f, 52/1024.0f, 36/1024.0f, TILE_SIZE, TILE_SIZE));
            enemies [enemies.size () - 1].velocity.x = 0.1f;
            enemies [enemies.size () - 1].type = "enemyAir";
        }
        
        // ghost enemy - ghost
        // different from enemyAir in that it cannot die
        if (mappy -> entities [index].type == "ghost") {
            enemies.push_back (Entity (spriteEnemy, (mappy -> entities [index].x + 0.5) * TILE_SIZE, (mappy -> entities[index].y + 0.5) * -1 * TILE_SIZE, 0.0f, 528.0f/1024.0f, 147/512.0f, 51/1024.0f, 73/512.0f, 2* TILE_SIZE, TILE_SIZE));
            enemies [enemies.size () - 1].velocity.x = 0.1f;
            enemies[enemies.size () - 1].type = "ghost";
        }
        
        // smasher - enemy
        if (mappy -> entities [index].type == "smasher") {
            enemies.push_back (Entity (spritePlayer, (mappy -> entities [index].x + 0.5) * TILE_SIZE, (mappy -> entities[index].y + 0.5) * -1 * TILE_SIZE, 0.0f, 807.0f/1024.0f, 306/1024.0f, 42/1024.0f, 40/1024.0f, TILE_SIZE, TILE_SIZE));
            enemies [enemies.size () - 1].velocity.y = 0.0f;
            enemies[enemies.size () - 1].type = "smasher";
        }
        
        // keys, needed to unlock next level 
        if (mappy -> entities [index].type == "keyRed") {
            key = Entity (spritePlayer, (mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 962/1024.0f, 252/1024.0f, 29/1024.0f, 30/1024.0f, TILE_SIZE /2, TILE_SIZE/2 );
            key.type = "keyRed";
        }
        
        if (mappy -> entities [index].type == "keyGreen") {
            key = Entity (spritePlayer, (mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 961/1024.0f, 495/1024.0f, 29/1024.0f, 30/1024.0f, TILE_SIZE /2, TILE_SIZE/2);
            key.type = "keyGreen";
        }
        
        // power-up plants
        if (mappy -> entities [index].type == "plantRed") {
            powerUp.push_back (Entity (spritePlayer, (mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 809/1024.0f, 0/1024.0f, 40/1024.0f, 50/1024.0f, TILE_SIZE, TILE_SIZE));
            powerUp [powerUp.size () - 1].type = "plantRed";
        }
        
        if (mappy -> entities [index].type == "plantBlue") {
            powerUp.push_back (Entity (spritePlayer, (mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 961/1024.0f, 949/1024.0f, 29/1024.0f, 55/1024.0f, TILE_SIZE, TILE_SIZE));
            powerUp [powerUp.size () - 1].type = "plantBlue";
        }
        
        if (mappy -> entities [index].type == "plantGreen") {
            powerUp.push_back (Entity (spritePlayer, (mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 963/1024.0f, 194/1024.0f, 26/1024.0f, 50/1024.0f, TILE_SIZE, TILE_SIZE));
            powerUp[powerUp.size () - 1].type = "plantGreen";
        }
        
        // platforms for level 3
        // H - horizontal
        // V - vertical
        if (mappy -> entities [index].type == "platformH") {
            platforms.push_back (Entity (spritePlayer, (mappy -> entities [index].x + 0.5) * TILE_SIZE, (mappy -> entities[index].y + 0.5) * -1 * TILE_SIZE, 0.0f, 195.0f/1024.0f, 65/1024.0f, 64/1024.0f, 64/1024.0f, TILE_SIZE * 4, TILE_SIZE));
            platforms [platforms.size () - 1].velocity = Vector3 ( 1.0f, 0.0f, 0.0f);
            platforms [platforms.size () - 1].type = "platformH";
            //platforms [platforms.size () - 1].direction = -1;
        }
        if (mappy -> entities [index].type == "platformV") {
            platforms.push_back (Entity (spritePlayer, (mappy -> entities [index].x + 0.5) * TILE_SIZE, (mappy -> entities[index].y + 0.5) * -1 * TILE_SIZE, 0.0f, 195.0f/1024.0f, 65/1024.0f, 64/1024.0f, 64/1024.0f, TILE_SIZE * 4, TILE_SIZE));
            platforms [platforms.size () - 1].velocity = Vector3 ( 0.0f, 1.0f, 0.0f);
            platforms [platforms.size () - 1].type = "platformV";
        }
        
    }
}

// if family exists, update movements
void GameState::UpdateFamily(float elapsed) {
    for (int i = 0; i < family.size (); i++) {
        int TileY;
        int TileLeftX;
        int TileRightX;
        int TileBottomY;
        
        bool right = false;
        bool left = false;
        
        // calculate the above Tile values
        family[i].worldToTileCoordinates(family[i].position.x + family[i].sizeEnt.x/2, family[i].position.y, &TileRightX, &TileY);
        family[i].worldToTileCoordinates(family[i].position.x - family[i].sizeEnt.x/2, family[i].position.y - family[i].sizeEnt.y/2 , &TileLeftX, &TileBottomY);
        
        for (int x: solidTiles) {
            if (x == mappy -> mapData [TileY][TileRightX] - 1) {
                right = true;
            }
            if (x == mappy -> mapData [TileY] [TileLeftX] - 1) {
                left = true;
            }
        }
        // if hit a left or right boundary, reverse
        if ( left || right ) {
            family[i].velocity.x *= -1;
            family[i].direction *= -1;
        }
        family[i].position.x += family[i].velocity.x * elapsed;
    }
}

void GameState::Update (float elapsed) {
    UpdateEnemyMovement(elapsed);
    // update bullets
    for (int i = 0; i < playerBullets.size (); i++) {
        playerBullets[i].position.x += playerBullets [i].velocity.x * elapsed;
        if (playerBullets [i].velocity.x > 0) {
            playerBullets[i].distance += playerBullets [i].velocity.x * elapsed;
        }
        else {
            playerBullets[i].distance += -playerBullets [i].velocity.x * elapsed;
        }
    }
    UpdatePlants(elapsed);
    UpdatePlatforms (elapsed);
    UpdateFamily (elapsed);
}


// update enemy movements
void GameState::UpdateEnemyMovement(float elapsed) {
    for (int index = 0; index < enemies.size (); index++) {
    
        // Ground AIs
        if (enemies [index].type == "enemyGround") {
            int TileY;
            int TileLeftX;
            int TileRightX;
            int TileBottomY;
            
            bool right = false;
            bool left = false;
            
            // calculate the above Tile values
            enemies[index].worldToTileCoordinates(enemies[index].position.x + enemies[index].sizeEnt.x/2, enemies[index].position.y, &TileRightX, &TileY);
            enemies[index].worldToTileCoordinates(enemies[index].position.x - enemies[index].sizeEnt.x/2, enemies[index].position.y - enemies[index].sizeEnt.y/2 , &TileLeftX, &TileBottomY);
            
            for (int x: solidTiles) {
                if (x == mappy -> mapData [TileBottomY][TileRightX] - 1) {
                    right = true;
                }
                if (x == mappy -> mapData [TileBottomY] [TileLeftX] - 1) {
                    left = true;
                }
            }
            
            // if end of a road, reverse
            if ( !left || !right ) {
                enemies [index].velocity.x *= -1;
                enemies [index].direction *= -1;
            }
            enemies[index].position.x += enemies[index].velocity.x * elapsed;
        }
        
        // Air and Ghost AIs
        if (enemies [index].type == "enemyAir" || enemies [index].type == "ghost" ) {
            if (enemies [index].DistanceTo(&player) <= 1.0f) {
                float x = enemies [index].DistanceToX (&player);
                float y = enemies [index].DistanceToY (&player);
                
                //adjust velocities based on value of distances
                if ( x < 0 ) {
                    enemies [index].velocity.x = 0.5f;
                    if (enemies [index].type == "enemyAir") {
                        enemies [index].direction = 1.0;
                    }
                    else {
                        enemies [index].direction = -1.0f;
                    }
                }
                else if (x > 0){
                    enemies [index].velocity.x = -0.5f;
                    if (enemies [index].type == "enemyAir") {
                        enemies [index].direction = -1.0;
                    }
                    else {
                        enemies [index].direction = 1.0f;
                    }
                }
                else {
                    enemies [index].velocity.x = 0.0f;
                }
                if ( y < 0 ) {
                    enemies [index].velocity.y = 0.5f;
                }
                else if ( y > 0) {
                    enemies [index].velocity.y = -0.5f;
                }
                else {
                    enemies [index].velocity.y = 0.0f;
                }
            }
            
            // if player not in range, don't move
            else {
                enemies [index].velocity.x = 0.0f;
                enemies [index].velocity.y = 0.0f;
            }
            enemies [index].position.x += enemies [index].velocity.x * elapsed;
            enemies [index].position.y += enemies [index].velocity.y * elapsed;
        }
        
        // smasher AIS
        if (enemies [index].type == "smasher") {
            
            if (level == 1 || level == 2) {
                if (enemies [index].DistanceTo(&player) <= 1.5f && enemies [index].velocity.y <= 0.0f) {
                    enemies[index].velocity.y = -2.0f;
                }
                
                enemies[index].position.y += enemies[index].velocity.y * elapsed;
                
                int TileX;
                int TileTopY;
                int TileBottomY;
                
                bool top = false;
                bool bottom = false;
                
                // calculate the above Tile values
                enemies[index].worldToTileCoordinates(enemies[index].position.x, enemies[index].position.y - enemies[index].sizeEnt.y/2, &TileX, &TileBottomY);
                enemies[index].worldToTileCoordinates(enemies[index].position.x, enemies[index].position.y + enemies[index].sizeEnt.y/2 , &TileX, &TileTopY);
                
                for (int x: solidTiles) {
                    if (x == mappy -> mapData [TileBottomY][TileX] - 1) {
                        bottom = true;
                    }
                    if (x == mappy -> mapData [TileTopY] [TileX] - 1) {
                        top = true;
                    }
                }
                
                // if end of a road, reverse
                if ( bottom ) {
                    enemies [index].velocity.y = 0.5f;
                }
                
                if (top) {
                    enemies [index].velocity.y = 0.0f;
                }
            }
            
            if (level == 3 ) {
                // if player in range, update velocity
                if (enemies [index].DistanceTo(&player) <= 1.5f && enemies [index].velocity.y == 0.0f) {
                    enemies[index].velocity.y = -2.0f;
                }
                
                // update position
                enemies [index].position.y += enemies [index].velocity.y * elapsed;
                
                // bottom boundary
                if (enemies [index].position.y <= (20 + 0.5) * -TILE_SIZE &&
                    enemies [index].velocity.y < 0) {
                    enemies [index].position.y = (20 + 0.5) * -TILE_SIZE;
                    enemies [index].velocity.y = 0.5f;
                }
                
                // top boundary
                if (enemies [index].position.y >= (15 + 0.5) * -TILE_SIZE &&
                    enemies [index].velocity.y > 0) {
                    enemies [index].position.y = (15 + 0.5) * -TILE_SIZE;
                    enemies [index].velocity.y = 0.0f;
                }
            }
        }
    }
    
    
    // check for collision between enemy entities
    for (int index = 0; index < enemies.size (); index++) {
        for (int index2 = 0; index2 < enemies.size (); index2++ ){
            if (index != index2 && enemies [index].active && enemies [index2].active) {
                if (enemies [index].Collision (&enemies[index2])) {
                    enemies [index].velocity.x *= -1;
                    enemies [index].direction *= -1;
                }
                
            }
        }
    
    }
}

void GameState::UpdatePlants(float elapsed) {
    for (int i = 0; i < powerUp.size (); i++) {
        if (powerUpObtained && (powerUp[i].type == "plantRedObtained" || powerUp[i].type == "plantBlueObtained" || powerUp[i].type == "plantGreenObtained") && powerUp[i].DistanceToY(&player) < 0.6f) {
            powerUp [i].position.y += 0.5 * elapsed;
        }
        if (powerUpObtained && powerUp [i].DistanceToY(&player) >= 0.6f && player.active && (powerUp[i].type == "plantRedObtained" || powerUp[i].type == "plantBlueObtained" || powerUp[i].type == "plantGreenObtained")) {
            player.active = false;
            partSystem.ResetLocations(powerUp [i].position.x, powerUp [i].position.y);
            if (powerUp [i].DistanceToX(&player) > 0 && partSystem.velocity.x > 0) {
                partSystem.velocity.x *= -1;
            }
            if (powerUp [i].DistanceToX(&player) < 0 && partSystem.velocity.x < 0) {
                partSystem.velocity.x *= -1;
            }
        }
        
        if (!powerUpObtained) {
            if (powerUp [i].type == "plantBlueObtained") {
                player = Entity (spritePlayer, player.position.x, player.position.y, 0.0f, 762/1024.0f, 203/1024.0f, 45/1024.0f, 54/1024.0f, TILE_SIZE, TILE_SIZE);
                player.type = "playerBlue";
                player.gravity.y = -3.5f;
                powerUp [i].type = "plantDead";
                break;
            }
            
            if (powerUp [i].type == "plantGreenObtained") {
                player = Entity (spritePlayer, player.position.x, player.position.y, 0.0f, 890/1024.0f, 0/1024.0f, 38/1024.0f, 50/1024.0f, TILE_SIZE, TILE_SIZE);
                player.type = "playerGreen";
                player.gravity.y = -3.5f;
                powerUp [i].type = "plantDead";
                break;
            }
            
            if (powerUp [i].type == "plantRedObtained") {
                player = Entity (spritePlayer, player.position.x, player.position.y, 0.0f, 850/1024.0f, 518/1024.0f, 39/1024.0f, 48/1024.0f, TILE_SIZE, TILE_SIZE);
                player.gravity.y = -3.5f;
                player.type = "playerRed";
                powerUp [i].type = "plantDead";
                break;
            }
        }
    }
}


void GameState::UpdatePlatforms (float elapsed) {
    for (int index= 0 ; index < platforms.size () ; index ++ ) {
        
        if (player.CollisionPlatformY(&platforms [index])) {
            player.position.x += platforms[index].velocity.x * elapsed;
            player.position.y += platforms[index].velocity.y * elapsed;
        }
        
        platforms [index].position.x += platforms[index].velocity.x * elapsed;
        platforms [index].position.y += platforms[index].velocity.y * elapsed;
        if (platforms [index].velocity.x < 0 || platforms [index].velocity.y < 0 ) {
            platforms [index].distance += -platforms [index].velocity.x * elapsed + -platforms [index].velocity.y * elapsed;
        }
        else {
            platforms [index].distance += platforms [index].velocity.x * elapsed + platforms [index].velocity.y * elapsed;
        }
        
        if (platforms [index].distance >= 3.0) {
            platforms [index].distance = 0.0f;
            platforms [index].velocity.x *= -1;
            platforms [index].velocity.y *= -1;
        }
    }
    
}

// update when player can move on to next level
void GameState::UpdateLevel() {
    
    level += 3 ;
    keyObtained = false;
    
    LoadLevel();
    player.active = true;
}


// return the game level
int GameState::GetLevel(){
    return level;
}

// return the number of lives left
int GameState::GetLives (){
    return lives;
}

// drawing game
void GameState::Draw (ShaderProgram* program) {
    mappy -> Draw (program, sprites);
    key.Draw (program);
    for (int i = 0; i < powerUp.size (); i++) {
        if (powerUp[i].type != "plantDead") {
            powerUp [i].Draw (program);
        }
    }
    for (int i = 0; i < family.size (); i++) {
        family [i].Draw (program);
    }
    player.Draw (program);
    for (int i = 0; i < platforms.size (); i++) {
        platforms [i].Draw (program);
    }
    for (int i = 0; i < enemies.size (); i++){
        if (enemies [i].active) {
            enemies[i].Draw (program);
        }
    }
    for (int i = 0; i < playerBullets.size() ; i++ ) {
        playerBullets [i].Draw (program);
    }
}


bool GameState::shouldRemoveBullet (Entity thing) {
    return (! (thing.active)  || thing.distance >= 0.5f );
}

// checking for any collisions in game between entities
void GameState::CollisionEntities () {
    
    // if collision between enemy and player
    // reset player to start of the game
    for (int index = 0; index < enemies.size (); index++) {
        if (enemies [index].active && player.Collision(&enemies[index]) ) {
            lives--;
            keyObtained = false;
            if (!lives ) {
                level = 4;
                LoadLevel();
            }
            else {
                level = 1;
                death = true;
            }
        }
    }
    
    for (size_t index = 0 ; index < playerBullets.size () ; index++) {
        for (int enemy = 0; enemy < enemies.size (); enemy++) {
            if (playerBullets [index].Collision(& (enemies[enemy])) && enemies[enemy].active && enemies [enemy].active && playerBullets[index].active) {
                // collision between enemy and player bullet
                if ( ( enemies [enemy].type != "ghost" && enemies [enemy].type != "smasher") && ( (level == 1 && player.type == "playerBlue") || (level == 2 && player.type == "playerGreen") || (level == 3 && player.type == "playerRed") ) ){
                    enemies[enemy].active = false;
                    
                    playerBullets [index].active = false;
                    break;
                }
                else {
                    playerBullets [index].active = false;
                }
            }
        }
    }
    
    // removing bullets from vector
    playerBullets.erase(std::remove_if(playerBullets.begin(), playerBullets.end(), &GameState::shouldRemoveBullet), playerBullets.end());
            
    
    if (player.Collision (&key)) {
        keyObtained = true; 
    }
    
    for (int i = 0; i < powerUp.size (); i++) {
        if (player.Collision (&powerUp [i]) && powerUp[i].type != "plantDead" &&
            ((player.type == "playerRed" && powerUp[i].type != "plantRed") ||
             (player.type == "playerBlue" && powerUp [i].type != "plantBlue") ||
             (player.type == "playerGreen" && powerUp [i].type != "plantGreen"))) {
                powerUpObtained = true;
                if (powerUp [i].type == "plantRed") {
                    powerUp [i].type = "plantRedObtained";
                }
                if (powerUp [i].type == "plantBlue") {
                    powerUp [i].type = "plantBlueObtained";
                }
                if (powerUp [i].type == "plantGreen") {
                    powerUp [i].type = "plantGreenObtained";
                }
                break;
        }
    }
    
    // move on to next level if at the door with key
    int TileX;
    int TileY;
    player.worldToTileCoordinates(player.position.x, player.position.y, &TileX, &TileY);
    if (keyObtained && (mappy -> mapData [TileY] [TileX] - 1 == 152 || mappy -> mapData [TileY] [TileX] - 1 == 153)) {
        //UpdateLevel ();
        player.active = false;
        player.velocity.x = 0.0f;
        player.velocity.y = 0.0f;
        nextLevel = true;
    }
    
    int TileBottom;
    player.worldToTileCoordinates(player.position.x, player.position.y - player.sizeEnt.y/2, &TileX, &TileBottom);
    if (mappy -> mapData [TileBottom] [TileX] - 1 == 234 || mappy -> mapData [TileBottom] [TileX] - 1 == 232) {
        lives --;
        keyObtained = false;
        if (!lives) {
            level = 4;
            nextLevel = true;
            LoadLevel();
        }
        else {
            level = 1;
            death = true;
        }
    }
}

// check and resolve horizontal collision
void GameState::CollisionX () {
    int TileX;
    int TileY;
    int TileLeftX;
    int TileRightX;
    int TileTopY;
    int TileBottomY;
    
    bool rightTile = false;
    bool leftTile = false;
    
    // calculate the above Tile values of center, left, right, top, bottom
    player.worldToTileCoordinates(player.position.x, player.position.y, &TileX, &TileY);
    player.worldToTileCoordinates(player.position.x - player.sizeEnt.x/2, player.position.y - player.sizeEnt.y/2 , &TileLeftX, &TileBottomY);
    player.worldToTileCoordinates(player.position.x + player.sizeEnt.x/2, player.position.y + player.sizeEnt.y/2, &TileRightX, &TileTopY);
    
    for (int x: solidTiles) {
        if (x == mappy -> mapData [TileY][TileRightX] - 1) {
            rightTile = true;
        }
        if (x == mappy -> mapData [TileY][TileLeftX] - 1) {
            leftTile = true;
        }
    }
    
    if (rightTile) {
        float worldRightX = TILE_SIZE * TileRightX;
        if (worldRightX < player.position.x + player.sizeEnt.x/2) {
            player.position.x -= (player.position.x + player.sizeEnt.x/2 - worldRightX) ;
        }
        player.collidedRight = true;
    }
    
    // if left tile is solid
    if (leftTile) {
        float worldLeftX = TILE_SIZE * TileLeftX;
        if (worldLeftX + TILE_SIZE > player.position.x - player.sizeEnt.x/2) {
            player.position.x += worldLeftX + TILE_SIZE - (player.position.x - player.sizeEnt.x/2);
        }
        player.collidedLeft = true;
    }
    
}

// check and adjust vertical collision
void GameState::CollisionY () {
    
    int TileX;
    int TileY;
    int TileLeftX;
    int TileRightX;
    int TileTopY;
    int TileBottomY;
    // calculate the above Tile values of center, left, right, top, bottom
    player.worldToTileCoordinates(player.position.x, player.position.y, &TileX, &TileY);
    player.worldToTileCoordinates(player.position.x - player.sizeEnt.x/2, player.position.y - player.sizeEnt.y/2 , &TileLeftX, &TileBottomY);
    player.worldToTileCoordinates(player.position.x + player.sizeEnt.x/2, player.position.y + player.sizeEnt.y/2, &TileRightX, &TileTopY);
    
    bool bottomTile = false;
    bool topTile = false;

    for (int x: solidTiles) {
        if (x == mappy -> mapData[TileBottomY] [TileX] - 1) {
            bottomTile = true;
        }
        if (x == mappy -> mapData [TileTopY][TileX] - 1) {
            topTile = true;
        }
    }
    

    // if tile below is solid, reset bottom to be on top
    if (bottomTile) {
        float worldBotY = -1 * TILE_SIZE * TileBottomY;
        if (worldBotY > player.position.y - player.sizeEnt.y/2) {
            player.position.y += (worldBotY - player.position.y - player.sizeEnt.y/2) + TILE_SIZE;
        }
        player.collidedBottom = true;
        player.velocity.y = 0.0f;
    }
    
    
    // if tile above is solid
    if (topTile) {
        float worldTopY = -1 * TILE_SIZE * TileTopY;
        if (worldTopY - TILE_SIZE < player.position.y + player.sizeEnt.y/2) {
            player.position.y -= (player.position.y + player.sizeEnt.y/2 - worldTopY) + TILE_SIZE;
        }
        player.collidedTop = true;
        player.velocity.y = 0.0f;
    }
    
    for (int i =0 ; i < platforms.size (); i++ ) {
        if (player.CollisionPlatformY(&platforms[i])) {
            if (player.position.y - player.sizeEnt.y/2 <= platforms[i].position.y + platforms[i].sizeEnt.y / 2 &&
                player.position.y + player.sizeEnt.y/2 > platforms[i].position.y + platforms[i].sizeEnt.y/2 ) {
                player.position.y = platforms[i].position.y + platforms[i].sizeEnt.y/2 + player.sizeEnt.y/2;
                player.collidedBottom = true;
                player.collidedPlatform = true;
                
                player.velocity.y = 0.0f;
            }
            if (player.position.y + player.sizeEnt.y/2 >= platforms[i].position.y - platforms[i].sizeEnt.y/2 &&  player.position.y != platforms[i].position.y + platforms[i].sizeEnt.y/2 + player.sizeEnt.y/2) {
                player.position.y = platforms[i].position.y - platforms[i].sizeEnt.y/2 - player.sizeEnt.y/2 ;
                player.collidedTop = true;
                player.velocity.y = -1.0f;
            }
            
            if (player.collidedBottom && player.collidedTop) {
                lives--;
                keyObtained = false;
                if (!lives ) {
                    level = 4;
                    LoadLevel();
                }
                else {
                    level = 1;
                    death = true;
                }
            }
        }
    }
}

// shooting a bullet from player
void GameState::shootPlayerBullet(int sprites) {
    Entity playerBullet = Entity (sprites, 0.0f, -1.5f, 0.0f, 858.0f/1024.0f, 230.0f/1024.0f, 9.0f/1024.0f, 54.0f/1024.0f, 0.3f, 0.3f);
    if (player.type == "playerBlue") {
        playerBullet = Entity (sprites, 0.0f, -1.5f, 0.0f, 856.0f/1024.0f, 421.0f/1024.0f, 9.0f/1024.0f, 54.0f/1024.0f, 0.3f, 0.3f);
    }
    if (player.type == "playerGreen") {
        playerBullet = Entity (sprites, 0.0f, -1.5f, 0.0f, 849/1024.0f, 310/1024.0f, 9.0f/1024.0f, 54.0f/1024.0f, 0.3f, 0.3f);
    }
    
    // initializing bullet properties
    playerBullet.active = true;
    if (player.direction == 1) {
        playerBullet.position.x = player.position.x + player.sizeEnt.x;
        playerBullet.rotation = 3 * M_PI / 2 ;
    }
    else {
        playerBullet.position.x = player.position.x - player.sizeEnt.x;
        playerBullet.rotation = M_PI/2;
    }
    
    playerBullet.direction = player.direction;
    playerBullet.position.y = player.position.y;
    playerBullet.velocity.x = player.direction;
    playerBullet.distance = 0.0f;
    playerBullet.rotation = M_PI/2;
    
    playerBullets.push_back(playerBullet);
}

// reset game state
void GameState::Reset () {
    lives = 5;
    level = 0;
    LoadLevel();
}


