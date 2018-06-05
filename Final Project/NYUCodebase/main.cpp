#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#define STB_IMAGE_IMPLEMENTATION
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <vector>
#include <string>
#include "Matrix.h"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "Entity.h"
#include "SheetSprite.h"
#include "FlareMap.h"
#include "GameState.h"
#include <SDL_mixer.h>
#include "ParticleEmitter.h"

// Any collision with enemy results in starting back at level 1
// Collect keys to unlock doors to the next world!
// In red world, enemies can only be defeated by blue player
// In blue world, enemies can only be defeated by green player
// In green world, enemies can only be defeated by red player

ShaderProgram program;
ShaderProgram unTextProgram;
Matrix viewMatrix;
Matrix projectionMatrix;
Matrix modelMatrix;
const Uint8 *keys = SDL_GetKeyboardState(NULL);
GameState state;
float viewX = 0.0f;
float viewY = 0.0f;
int textie;
int tilesheet;
int playerSheet;
int enemySheet;
int bulletSheet;
int backgroundOne;
int backgroundTwo;
int backgroundThree;
int tilesOne;
int tilesThree;
int hillsOne;
int hillsThree;
int blueStar;
int redStar;
int greenStar;
int powerStar;
Entity keyOutline;
Mix_Music *music;
float timer = 0.0f;
float alpha = 0.0f;
float coolDown = 0.0f;
bool nextState = false;
bool nextStateEnd = false;
ParticleEmitter gameOverRed;
ParticleEmitter gameOverBlue;
ParticleEmitter gameOverGreen;

Mix_Chunk *bulletSound;
Mix_Chunk *jumpSound;
Mix_Chunk *powerUpSound;
Mix_Chunk *deathSound;
Mix_Chunk *bouncingWall;
Mix_Chunk *keySound;

bool win = false;
enum GameMode { STATE_MAIN_MENU, STATE_GAME_LEVEL, STATE_GAME_OVER};
GameMode mode = STATE_MAIN_MENU;

void LoadingTextures();
void Setup();
void ProcessEvents (SDL_Event& event, bool& done);
void Update (float elapsed, int& direction);
void Render ();
void DrawUnTexture (float alpha );
void DrawText(ShaderProgram *program, int fontTexture, std::string text, float size, float spacing);
void DrawWords (ShaderProgram* program, int fontTexture, std::string text, float size, float spacing, float x, float y);
void DrawTexture (int textureID, float x, float y, float width, float height);

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#define FIXED_TIMESTEP 0.0166666f
#define MAP_WIDTH 102
#define MAP_HEIGHT 22
#define TILE_SIZE 0.3f

float lerp(float v0, float v1, float t) {
    return (1.0-t)*v0 + t*v1;
}

GLuint LoadTexture(const char *filePath) {
    int w,h,comp;
    unsigned char* image = stbi_load(filePath, &w, &h, &comp, STBI_rgb_alpha);
    if(image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    
    GLuint retTexture;
    glGenTextures(1, &retTexture);
    glBindTexture(GL_TEXTURE_2D, retTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    stbi_image_free(image);
    return retTexture;
}

SDL_Window* displayWindow;

int main(int argc, char *argv[])
{
    
    Setup();
    
    SDL_Event event;
    bool done = false;

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    Mix_VolumeMusic(25);
    Mix_PlayMusic (music, -1);
    
    bulletSound = Mix_LoadWAV (RESOURCE_FOLDER"Resources/slimeball.wav");
    jumpSound = Mix_LoadWAV (RESOURCE_FOLDER"Resources/Jump_01.wav");
    powerUpSound = Mix_LoadWAV(RESOURCE_FOLDER"Resources/sfx_sounds_powerup2.wav");
    deathSound = Mix_LoadWAV(RESOURCE_FOLDER"Resources/sfx_deathscream_android8.wav");
    bouncingWall = Mix_LoadWAV(RESOURCE_FOLDER"Resources/sfx_movement_jump13_landing.wav");
    keySound = Mix_LoadWAV (RESOURCE_FOLDER"Resources/Collect_Point_01.wav");
    
    float lastFrameTicks = 0.0f;
    float elapsed = 0.0f;
    float accumulator = 0.0f;

    int direction = 1;
    
    while (!done) {
        ProcessEvents (event, done);
        //glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
        glClearColor(135.0f / 255.0f, 206.0f/ 255.0f, 250.0f/255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        float ticks = (float)SDL_GetTicks()/1000.0f;
        elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        elapsed += accumulator;
        if(elapsed < FIXED_TIMESTEP) {
            accumulator = elapsed;
            continue; }
        while(elapsed >= FIXED_TIMESTEP) {
            coolDown += FIXED_TIMESTEP;
            Update (FIXED_TIMESTEP, direction);
            elapsed -= FIXED_TIMESTEP;
        }
        accumulator = elapsed;
        Render ();
        
        SDL_GL_SwapWindow(displayWindow);
    }
    Mix_FreeMusic(music);
    Mix_FreeChunk(deathSound);
    Mix_FreeChunk(bouncingWall);
    Mix_FreeChunk(keySound);
    Mix_FreeChunk(powerUpSound);
    Mix_FreeChunk(bulletSound);
    Mix_FreeChunk(jumpSound);
    SDL_Quit();
    return 0;
}

// load all textures for use 
void LoadingTextures () {
    tilesheet = LoadTexture(RESOURCE_FOLDER"Resources/tilesheet_complete.png");
    playerSheet = LoadTexture(RESOURCE_FOLDER"Resources/spritesheet_complete.png");
    enemySheet = LoadTexture(RESOURCE_FOLDER"Resources/enemies.png");
    bulletSheet = LoadTexture (RESOURCE_FOLDER"Resources/sheet.png");
    textie = LoadTexture(RESOURCE_FOLDER "Resources/font1.png");
    backgroundOne = LoadTexture(RESOURCE_FOLDER"Resources/set1_background.png");
    backgroundTwo = LoadTexture(RESOURCE_FOLDER"Resources/bg_castle.png");
    backgroundThree = LoadTexture(RESOURCE_FOLDER"Resources/set3_background.png");
    hillsOne = LoadTexture(RESOURCE_FOLDER"Resources/set1_hills.png");
    hillsThree = LoadTexture(RESOURCE_FOLDER"Resources/set3_hills.png");
    tilesOne = LoadTexture(RESOURCE_FOLDER"Resources/set1_tiles.png");
    tilesThree = LoadTexture(RESOURCE_FOLDER"Resources/set3_tiles.png");
    powerStar = LoadTexture(RESOURCE_FOLDER"Resources/flare_0.png");
    blueStar = LoadTexture(RESOURCE_FOLDER"Resources/bluestar.png");
    redStar = LoadTexture(RESOURCE_FOLDER"Resources/redStar.png");
    greenStar = LoadTexture(RESOURCE_FOLDER"Resources/greenstar.png");
}

// setting up game
void Setup () {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 1280, 720);
    
    program.Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    unTextProgram.Load (RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
    
    LoadingTextures();
    
    Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 4096 );
    music = Mix_LoadMUS(RESOURCE_FOLDER"Resources/awesomeness.wav");
    
    state.Initiate (tilesheet, playerSheet, enemySheet, powerStar);
    
    glUseProgram(program.programID);
    glUseProgram(unTextProgram.programID);
    
    // set view and projection matrices
    projectionMatrix.SetOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
    viewX = -3.55f;
    viewY = -1 * state.player.position.y;
    if (viewY >= state.mappy -> mapHeight * TILE_SIZE - 2.0) {
        viewY = state.mappy -> mapHeight * TILE_SIZE - 2.0;
    }
    program.SetProjectionMatrix(projectionMatrix);
    unTextProgram.SetProjectionMatrix(projectionMatrix);
    
    keyOutline = Entity (playerSheet, 0.0f, 0.0f, 0.0f, 927/1024.0f, 624/1024.0f, 36/1024.0f, 36/1024.0f, TILE_SIZE / 2, TILE_SIZE/2);
    
    // prepare the fireworks for winning page
    gameOverRed = ParticleEmitter (redStar, 40, (6 + 0.5f) * TILE_SIZE, (13 + 0.5f) * -1 * TILE_SIZE + 1.5f);
    gameOverBlue = ParticleEmitter (blueStar, 40, (26 + 0.5f) * TILE_SIZE, (13 + 0.5f) * -1 * TILE_SIZE + 1.5f);
    gameOverGreen = ParticleEmitter (greenStar, 40, (42 + 0.5f) * TILE_SIZE, (13 + 0.5f) * -1 * TILE_SIZE + 1.5f );
    
    gameOverRed.velocity = Vector3 (1.0, 1.0, 0.0);
    gameOverBlue.velocity = Vector3 (1.0, 1.0, 0.0);
    gameOverGreen.velocity = Vector3 (1.0, 1.0, 0.0);
    
    gameOverRed.deviation = Vector3 (1.0, 1.0, 0.0);
    gameOverBlue.deviation = Vector3 (1.0, 1.0, 0.0);
    gameOverGreen.deviation = Vector3 (1.0, 1.0, 0.0);

}

// processing events
void ProcessEvents (SDL_Event& event, bool& done) {
    switch (mode) {
        case STATE_MAIN_MENU:
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                    done = true;
                }
            }
            // start the game
            if (keys [SDL_SCANCODE_SPACE] ){
                nextState = true;
            }
            
            // quit game
            if (keys [SDL_SCANCODE_Q] ){
                done = true;
            }
            break;
        case STATE_GAME_LEVEL:
            state.player.acceleration.x = 0.0f;
            state.player.acceleration.y = 0.0f;
            
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                    done = true;
                }
                
                else if (event.type == SDL_KEYDOWN) {
                    if (state.player.active) {
                        // simple jumping
                        if (event.key.keysym.scancode == SDL_SCANCODE_UP) {
                            if (state.player.collidedBottom) {
                                state.player.velocity.y = 4.0f;
                                state.player.acceleration.y = 1.0f;
                                Mix_PlayChannel (-1, jumpSound, 0);
                            }
                        }
                        
                        // bounce off walls but can't combine with left and right keys
                        if (event.key.keysym.scancode == SDL_SCANCODE_A) {
                            if (state.player.collidedLeft && !keys [SDL_SCANCODE_LEFT]) {
                                Mix_PlayChannel (-1, bouncingWall, 0);
                                state.player.velocity.y = 4.0f;
                                state.player.acceleration.y = 1.0f;
                                state.player.velocity.x = 1.5f;
                                state.player.acceleration.x = 1.0f;
                                state.player.direction = 1.0f;
                            }
                            if (state.player.collidedRight && !keys[SDL_SCANCODE_RIGHT] ) {
                                Mix_PlayChannel (-1, bouncingWall, 0);
                                state.player.velocity.y = 4.0f;
                                state.player.acceleration.y = 1.0f;
                                state.player.velocity.x = -1.5f;
                                state.player.acceleration.x = -1.0f;
                                state.player.direction = -1.0f;
                            }
                        }
                    }
                }
            }
            
            // quit game
            if (keys [SDL_SCANCODE_Q] ){
                done = true;
            }
            
            if (state.player.active) {
                //move left
                if (keys [SDL_SCANCODE_LEFT]){
                    state.player.velocity.x = -1.5f;
                    state.player.acceleration.x = -1.0f;
                    state.player.direction = -1.0f;
                }
                
                // move right
                else if (keys [SDL_SCANCODE_RIGHT]){
                    state.player.velocity.x = 1.5f;
                    state.player.acceleration.x = 1.0f;
                    state.player.direction = 1.0f;
                }
                if (keys [SDL_SCANCODE_B] && coolDown >= 0.75f) {
                    Mix_PlayChannel (-1, bulletSound, 0);
                    state.shootPlayerBullet(bulletSheet);
                    coolDown = 0.0f;
                }
            }
            
            // reset the game
            if (keys [SDL_SCANCODE_R]) {
                mode = STATE_MAIN_MENU;
                state.Reset();
                viewX = -3.55f;
                viewY = -1 * state.player.position.y;
                if (viewY >= state.mappy -> mapHeight * TILE_SIZE - 2.0) {
                    viewY = state.mappy -> mapHeight * TILE_SIZE - 2.0;
                }
                
                Mix_HaltMusic();
                Mix_FreeMusic(music);
                music = Mix_LoadMUS(RESOURCE_FOLDER"Resources/awesomeness.wav");
                Mix_VolumeMusic(25);
                Mix_PlayMusic (music, -1);
            }
            
            break;
        case STATE_GAME_OVER:
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                    done = true;
                }
            }
            
            // quit game
            if (keys [SDL_SCANCODE_Q] ){
                done = true;
            }
            
            // reset the game
            if (keys [SDL_SCANCODE_R]) {

                mode = STATE_MAIN_MENU;
                state.Reset();
                viewX = -3.55f;
                viewY = -1 * state.player.position.y;
                if (viewY >= state.mappy -> mapHeight * TILE_SIZE - 2.0) {
                    viewY = state.mappy -> mapHeight * TILE_SIZE - 2.0;
                }
                Mix_HaltMusic();
                Mix_FreeMusic(music);
                music = Mix_LoadMUS(RESOURCE_FOLDER"Resources/awesomeness.wav");
                Mix_VolumeMusic(25);
                Mix_PlayMusic (music, -1);
            }
            break;
    }
}

// updating title and game over screen
void mainGameOverUpdate (float elapsed, int& direction) {
    if (-viewX >= state.mappy -> mapWidth * TILE_SIZE - 3.55) {
        viewX = -(state.mappy -> mapWidth * TILE_SIZE - 3.55);
        direction *= -1;
    }
    if (viewX >= -3.55) {
        direction *= -1;
        viewX = -3.55;
    }
    viewX -= elapsed * direction;
    if (direction == -1) {
        viewX -= elapsed * 3 * direction;
    }
    // enemies move on title page and game over page
    state.Update(elapsed);
}

//updating the game
void GameLevelUpdate (float elapsed) {
    
    state.Update(elapsed);
    
    state.player.collisionBools();
    
    if (state.powerUpObtained) {
        state.player.velocity.x =0.0f;
    }
    
    
    // applying friction
    state.player.velocity.x = lerp(state.player.velocity.x, 0.0f, elapsed * 3.0);
    state.player.velocity.y = lerp(state.player.velocity.y, 0.0f, elapsed * 2.0);
    
    // calculating x velocity
    state.player.velocity.x += state.player.acceleration.x * elapsed;
    state.player.velocity.y += state.player.acceleration.y * elapsed;
    state.player.velocity.y += state.player.gravity.y * elapsed;
    
    // calculating positions
    state.player.position.y += state.player.velocity.y * elapsed;
    state.CollisionY ();
    
    state.player.position.x += state.player.velocity.x * elapsed;
    state.CollisionX ();
    
    // if moving past the left edge of game
    if (state.player.position.x - state.player.sizeEnt.x/2 <= 0.0f) {
        state.player.position.x = 0.0f + state.player.sizeEnt.x/2;
        state.player.velocity.x = 0;
    }
    
    // if moving past the right edge of game
    if (state.player.position.x + state.player.sizeEnt.x/2 >= state.mappy -> mapWidth * 0.3) {
        state.player.position.x = state.mappy -> mapWidth * 0.3 - state.player.sizeEnt.x/2;
        state.player.velocity.x = 0;
    }
    
    //if hitting the top of the window (can happen when trying to jump onto clouds)
    if (state.player.position.y + state.player.sizeEnt.y/2 >= 0) {
        state.player.position.y = 0 - state.player.sizeEnt.y/2;
        state.player.velocity.y = 0;
    }
    
    // check for player collisions with enemies
    state.CollisionEntities();
    if (state.player.Collision(&state.key)) {
        Mix_PlayChannel (-1, keySound, 0);
    }
    
    if (state.death) {
        Mix_PlayChannel (-1, deathSound, 0);
        state.LoadLevel();
        state.death = false;
    }
    
    // adjusting viewMatrix
    viewX = -state.player.position.x;
    viewY = -state.player.position.y;
    if (viewX >= -3.55) {
        viewX = -3.55;
    }
    else if (-viewX >= state.mappy -> mapWidth * TILE_SIZE - 3.55) {
        viewX = -(state.mappy -> mapWidth * TILE_SIZE - 3.55);
    }
    if (viewY >= state.mappy -> mapHeight * TILE_SIZE - 2.0) {
        viewY = state.mappy -> mapHeight * TILE_SIZE - 2.0;
    }
    else if (viewY <= 2.0) {
        viewY = 2.0;
    }
    
    // if obtained key, replace keyOutline with key
    keyOutline.position.x = - (viewX + 4.05 - 2 * TILE_SIZE) + 6 * 0.25;
    keyOutline.position.y = - (viewY - 2.5 + 3 * TILE_SIZE) ;
    if (state.keyObtained) {
        state.key.position.x = keyOutline.position.x;
        state.key.position.y = keyOutline.position.y;
    }
    
    
    if (!state.player.active && state.powerUpObtained) {
        state.partSystem.Update(elapsed);
        timer += elapsed;
    }
    
    if (timer > 4.0f){
        timer = 0.0f;
        state.player.active = true;
        state.powerUpObtained = false;
        Mix_PlayChannel (-1, powerUpSound, 0);
        //state.powerUp.position.x = -5.0f;
    }
    
    nextState = state.nextLevel;
}

void Update (float elapsed, int& direction ){
    switch (mode) {
        case STATE_MAIN_MENU:
            mainGameOverUpdate(elapsed, direction);
            if (nextState) {
                alpha = lerp(alpha, 1.0f, FIXED_TIMESTEP );
            }
            
            if ( alpha >= 0.995 && nextState) {
                alpha = 1.0;
                nextState = false;
                nextStateEnd = true;
                mode = STATE_GAME_LEVEL;
                Mix_HaltMusic();
                Mix_FreeMusic(music);
                music = Mix_LoadMUS(RESOURCE_FOLDER"Resources/gameMusic.wav");
                Mix_VolumeMusic(25);
                Mix_PlayMusic (music, -1);
                
                state.UpdateLevel ();
                viewX = 0.0f;
                viewY= 0.0f;
            }
            
            if (nextStateEnd) {
                alpha = lerp(alpha, 0.0f, FIXED_TIMESTEP);
            }
            if (alpha <= 0.005 && nextStateEnd) {
                nextStateEnd = false;
                alpha = 0.0f;
                state.nextLevel = false;
            }
            
            
            break;
        case STATE_GAME_LEVEL:
        
            if (nextStateEnd) {
                alpha = lerp(alpha, 0.0f, FIXED_TIMESTEP);
            }
            
            if (alpha <= 0.005 && nextStateEnd) {
                nextStateEnd = false;
                alpha = 0.0f;
                state.nextLevel = false;
            }
            
            GameLevelUpdate (FIXED_TIMESTEP);
            
            if (nextState) {
                alpha = lerp(alpha, 1.0f, FIXED_TIMESTEP );
            }
            
            if ( alpha >= 0.995 && nextState && state.GetLevel () != 4) {
                alpha = 1.0;
                nextState = false;
                nextStateEnd = true;
                state.nextLevel = false;
                state.UpdateLevel ();
                viewX = 0.0f;
                viewY= 0.0f;
            }
            
            if (state.GetLevel () == 4) {
                state.nextLevel = false;
                nextState = false;
                nextStateEnd = true;
                alpha = 1.0f;
                mode = STATE_GAME_OVER;
                Mix_HaltMusic();
                Mix_FreeMusic(music);
                
                // sad song
                if (state.GetLives() == 0) {
                    music = Mix_LoadMUS(RESOURCE_FOLDER"Resources/PianoLoop.wav");
                    Mix_VolumeMusic(40);
                }
                
                // winning song
                else {
                    music = Mix_LoadMUS(RESOURCE_FOLDER"Resources/SummerSunday.wav");
                    Mix_VolumeMusic(40);
                }
                Mix_PlayMusic (music, -1);
                viewX = -3.55;
                viewY = -state.player.position.y;
            }
            
        
            break;
        case STATE_GAME_OVER:
            if (nextState) {
                alpha = lerp(alpha, 1.0f, FIXED_TIMESTEP );
            }
            
            if ( alpha >= 0.995 && nextState ) {
                alpha = 1.0;
                nextState = false;
                nextStateEnd = true;
                viewX = -3.55;
                viewY = -state.player.position.y;
            }
            
            if (nextStateEnd) {
                alpha = lerp(alpha, 0.0f, FIXED_TIMESTEP);

            }
            if (alpha <= 0.005 && nextStateEnd) {
                nextStateEnd = false;
                alpha = 0.0f;
                state.nextLevel = false;
            }
            mainGameOverUpdate (elapsed, direction);
            gameOverRed.UpdateFireworks (elapsed);
            gameOverBlue.UpdateFireworks (elapsed);
            gameOverGreen.UpdateFireworks (elapsed);
            state.UpdateFamily(elapsed);
            break;
    }
}

void mainRender () {
    
    viewMatrix.Identity();
    viewMatrix.SetPosition (viewX, viewY, 0.0f);
    program.SetViewMatrix(viewMatrix);
    unTextProgram.SetViewMatrix(viewMatrix);
    DrawTexture (backgroundOne, MAP_WIDTH/6 *0.3, -MAP_HEIGHT/2 * 0.3, MAP_WIDTH/3 * 0.3, MAP_HEIGHT * 0.3);
    DrawTexture (tilesOne, MAP_WIDTH/6 *0.3, -MAP_HEIGHT/2 * 0.3, MAP_WIDTH/3 * 0.3, MAP_HEIGHT * 0.3);
    DrawTexture (hillsOne, MAP_WIDTH/6 *0.3, -MAP_HEIGHT/2 * 0.3, MAP_WIDTH/3 * 0.3, MAP_HEIGHT * 0.3);

    DrawTexture (backgroundTwo, MAP_WIDTH/6 *0.3 + (MAP_WIDTH/3 * 0.3), -MAP_HEIGHT/2 * 0.3, MAP_WIDTH/3 * 0.3, MAP_HEIGHT * 0.3);
    DrawTexture (backgroundThree, MAP_WIDTH/6 *0.3 + 2 * (MAP_WIDTH/3) * 0.3, -MAP_HEIGHT/2 * 0.3, MAP_WIDTH/3 * 0.3, MAP_HEIGHT * 0.3);
    DrawTexture (tilesThree, MAP_WIDTH/6 *0.3 + 2 * (MAP_WIDTH/3) * 0.3, -MAP_HEIGHT/2 * 0.3, MAP_WIDTH/3 * 0.3, MAP_HEIGHT * 0.3);
    DrawTexture (hillsThree, MAP_WIDTH/6 *0.3 + 2 * (MAP_WIDTH/3) * 0.3, -MAP_HEIGHT/2 * 0.3, MAP_WIDTH/3 * 0.3, MAP_HEIGHT * 0.3);
    state.Draw (&program);
    DrawWords (&program, textie, "Alien Invaders", 0.4, 0.0f, (viewX + (-0.5*0.4) + (14*0.4/2)), viewY - 0.5f);
    DrawWords (&program, textie, "Press Space to Start", 0.25f, 0.0f, viewX + (-0.5f*0.25) + (20*0.25/2), viewY );
    DrawWords (&program, textie, "ARROWS  MOVE     ", 0.25f, 0.0f, viewX + (-0.5f*0.25) + (17*0.25/2), viewY + 0.5f );
    DrawWords (&program, textie, "A       WALL JUMP", 0.25f, 0.0f, viewX + (-0.5f*0.25) + (17*0.25/2), viewY + 0.8f );
    DrawWords (&program, textie, "B       BULLETS  ", 0.25f, 0.0f, viewX + (-0.5f*0.25) + (17*0.25/2), viewY + 1.1f );
    DrawWords (&program, textie, "Q       QUIT     ", 0.25f, 0.0f, viewX + (-0.5f*0.25) + (17*0.25/2), viewY + 1.4f );
    
    
    if (nextState) {
        DrawUnTexture(alpha);
    }
}

void gameRender () {
    viewMatrix.Identity();
    viewMatrix.SetPosition(viewX, viewY, 0.0f);
    program.SetViewMatrix(viewMatrix);
    unTextProgram.SetViewMatrix(viewMatrix);
    if (state.GetLevel() == 1) {
        DrawTexture (backgroundOne, state.mappy -> mapWidth/2 *0.3, -state.mappy -> mapHeight/2 * 0.3, state.mappy -> mapWidth * 0.3, state.mappy -> mapHeight * 0.3);
        DrawTexture (tilesOne, -viewX, -viewY, 3.55 * 2 , 2.0 * 2);
        DrawTexture (hillsOne, -viewX, -viewY, 3.55 * 2 , 2.0 * 2);
    }
    if (state.GetLevel() == 2) {
        DrawTexture (backgroundTwo, state.mappy -> mapWidth/2 *0.3, -state.mappy -> mapHeight/2 * 0.3, state.mappy -> mapWidth * 0.3, state.mappy -> mapHeight * 0.3);
    }
    if (state.GetLevel() == 3) {
        DrawTexture (backgroundThree, state.mappy -> mapWidth/2 *0.3, -state.mappy -> mapHeight/2 * 0.3, state.mappy -> mapWidth * 0.3, state.mappy -> mapHeight * 0.3);
        DrawTexture (tilesThree, -viewX, -viewY, 3.55 * 2 , 2.0 * 2);
        DrawTexture (hillsThree, -viewX, -viewY, 3.55 * 2 , 2.0 * 2);
    }
    state.Draw (&program);
    
    DrawWords (&program, textie, "Level" + std::to_string (state.GetLevel()), 0.25 , 0.0f, viewX + 4.05 - 2 * TILE_SIZE, viewY - 2.5 + 2 * TILE_SIZE );
    DrawWords (&program, textie, "Items:", 0.25, 0.0f, viewX + 4.05 - 2 * TILE_SIZE, viewY - 2.5 + 3 * TILE_SIZE);
    DrawWords (&program, textie, "Lives:" + std::to_string (state.GetLives()), 0.25, 0.0f, viewX - 1.9, viewY - 2.5 + 2 * TILE_SIZE );
    if (!state.keyObtained) {
        keyOutline.Draw (&program);
    }
    if (!state.player.active) {
        modelMatrix.Identity();
        program.SetModelMatrix(modelMatrix);
        state.partSystem.Render(&program);
    }
    
    if (nextState || nextStateEnd) {
        DrawUnTexture(alpha);
    }
}

// render game over
void gameOverRender () {
    viewMatrix.Identity();
    viewMatrix.SetPosition(viewX, viewY, 0.0f);
    program.SetViewMatrix(viewMatrix);
    DrawTexture (backgroundOne, state.mappy -> mapWidth/6 *0.3, -state.mappy -> mapHeight/2 * 0.3, state.mappy -> mapWidth/3 * 0.3, state.mappy -> mapHeight * 0.3);
    DrawTexture (tilesOne, state.mappy -> mapWidth/6 *0.3, -state.mappy -> mapHeight/2 * 0.3, state.mappy -> mapWidth/3 * 0.3, state.mappy -> mapHeight * 0.3);
    DrawTexture (hillsOne, state.mappy -> mapWidth/6 *0.3, -state.mappy -> mapHeight/2 * 0.3, state.mappy -> mapWidth/3 * 0.3, state.mappy -> mapHeight * 0.3);
    
    DrawTexture (backgroundTwo, state.mappy -> mapWidth/6 *0.3 + (state.mappy -> mapWidth/3 * 0.3), -state.mappy -> mapHeight/2 * 0.3, state.mappy -> mapWidth/3 * 0.3, state.mappy -> mapHeight * 0.3);
    DrawTexture (backgroundThree, state.mappy -> mapWidth/6 *0.3 + 2 * (state.mappy -> mapWidth/3) * 0.3, -state.mappy -> mapHeight/2 * 0.3, state.mappy -> mapWidth/3 * 0.3, state.mappy -> mapHeight * 0.3);
    DrawTexture (tilesThree, state.mappy -> mapWidth/6 *0.3 + 2 * (state.mappy -> mapWidth/3) * 0.3, -state.mappy -> mapHeight/2 * 0.3, state.mappy -> mapWidth/3 * 0.3, state.mappy -> mapHeight * 0.3);
    DrawTexture (hillsThree, state.mappy -> mapWidth/6 *0.3 + 2 * (state.mappy -> mapWidth/3) * 0.3, -state.mappy -> mapHeight/2 * 0.3, state.mappy -> mapWidth/3 * 0.3, state.mappy -> mapHeight * 0.3);
    state.Draw (&program);
    Entity greenPlayer = Entity (playerSheet, (42 + 0.5f) * TILE_SIZE, (10 + 0.5f) * -1 * TILE_SIZE, 0.0f, 849/1024.0f, 429/1024.0f, 40/1024.0f , 39/1024.0f, TILE_SIZE, TILE_SIZE);
    Entity bluePlayer = Entity (playerSheet, (26 + 0.5f) * TILE_SIZE, (15 + 0.5f) * -1 * TILE_SIZE, 0.0f, 758/1024.0f, 771/1024.0f, 46/1024.0f, 40/1024.0f, TILE_SIZE, TILE_SIZE);
    if (state.GetLives () == 0) {
        greenPlayer = Entity (playerSheet, (42 + 0.5f) * TILE_SIZE, (10 + 0.5f) * -1 * TILE_SIZE, 0.0f, 890/1024.0f, 272/1024.0f, 38/1024.0f, 43/1024.0f, TILE_SIZE, TILE_SIZE);
        bluePlayer = Entity (playerSheet, (26 + 0.5f) * TILE_SIZE, (15 + 0.5f) * -1 * TILE_SIZE, 0.0f, 762/1024.0f, 258/1024.0f, 45/1024.0f, 46/1024.0f, TILE_SIZE, TILE_SIZE);
    }
    greenPlayer.Draw (&program);
    bluePlayer.Draw (&program);
    if (state.GetLives() == 0) {
        DrawWords (&program, textie, "YOU LOSE", 0.4, 0.0f, (viewX + (-0.5*0.4) + (8*0.4/2)), viewY - 0.5f);
    }
    else {
        DrawWords (&program, textie, "YOU WIN", 0.4, 0.0f, (viewX + (-0.5*0.4) + (7*0.4/2)), viewY - 0.5f);
    }
    DrawWords (&program, textie, "Press R to Play Again", 0.25f, 0.0f, viewX + (-0.5f*0.25) + (21*0.25/2), viewY + 0.5f );
    
    // draw fireworks only if you won
    if (state.GetLives()) {
        modelMatrix.Identity();
        program.SetModelMatrix(modelMatrix);
        gameOverRed.Render (&program);
        
        modelMatrix.Identity();
        program.SetModelMatrix(modelMatrix);
        gameOverBlue.Render (&program);
        
        modelMatrix.Identity();
        program.SetModelMatrix(modelMatrix);
        gameOverGreen.Render (&program);
    }
    
    if (nextState || nextStateEnd) {
        DrawUnTexture(alpha);
    }
}

// drawing game
void Render () {
    
    switch(mode) {
        case STATE_MAIN_MENU:
            mainRender();
            break;
        case STATE_GAME_LEVEL:
            gameRender();
            break;
        case STATE_GAME_OVER:
            gameOverRender();
            break;
    }
    
    //state.Draw (&program);
}

//draw text
void DrawText(ShaderProgram *program, int fontTexture, std::string text, float size, float spacing) {
    
    float texture_size = 1.0/16.0f;
    std::vector<float> vertexData;
    std::vector<float> texCoordData;
    for(int i=0; i < text.size(); i++) {
        int spriteIndex = (int)text[i];
        float texture_x = (float)(spriteIndex % 16) / 16.0f;
        float texture_y = (float)(spriteIndex / 16) / 16.0f;
        vertexData.insert(vertexData.end(), {
            ((size+spacing) * i) + (-0.5f * size), 0.5f * size,
            ((size+spacing) * i) + (-0.5f * size), -0.5f * size,
            ((size+spacing) * i) + (0.5f * size), 0.5f * size,
            ((size+spacing) * i) + (0.5f * size), -0.5f * size,
            ((size+spacing) * i) + (0.5f * size), 0.5f * size,
            ((size+spacing) * i) + (-0.5f * size), -0.5f * size,
        });
        texCoordData.insert(texCoordData.end(), {
            texture_x, texture_y,
            texture_x, texture_y + texture_size,
            texture_x + texture_size, texture_y,
            texture_x + texture_size, texture_y + texture_size,
            texture_x + texture_size, texture_y,
            texture_x, texture_y + texture_size,
        }); }
    
    // draw this data (use the .data() method of std::vector to get pointer to data)
    // draw this yourself, use text.size() * 6 or vertexData.size()/2 to get number of vertices
    
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    glVertexAttribPointer(program -> positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
    glEnableVertexAttribArray(program -> positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data ());
    glEnableVertexAttribArray(program->texCoordAttribute);
    glDrawArrays(GL_TRIANGLES, 0, int (text.size()) * 6);
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}


// drawing words - needed for title and game over page
void DrawWords (ShaderProgram* program, int fontTexture, std::string text, float size, float spacing, float x, float y) {
    Matrix modelMatrix;
    modelMatrix.SetPosition(-x, -y, 0.0f);
    program -> SetModelMatrix(modelMatrix);
    program -> SetViewMatrix(viewMatrix);
    
    DrawText(program, fontTexture, text, size, spacing);
}

// drawing textures - needed for drawing backgrounds
void DrawTexture (int textureID, float x, float y, float width, float height) {
    Matrix modelMatrix;
    modelMatrix.Scale (width, height, 1.0f);
    modelMatrix.SetPosition(x, y, 0.0f);
    program.SetModelMatrix(modelMatrix);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    float vertices[] = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    float texCoords[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

// draw the black rectangle fading in and fading out
void DrawUnTexture (float alpha) {
    unTextProgram.SetColor (0.0, 0.0, 0.0, 0.0);
    unTextProgram.SetAlpha (alpha);
    Matrix modelMatrix;
    modelMatrix.Scale(state.mappy ->mapWidth * 2 , state.mappy -> mapHeight * 2 , 1.0f);
    unTextProgram.SetModelMatrix(modelMatrix);
    float verticesUntextured[] = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
    glVertexAttribPointer(unTextProgram.positionAttribute, 2, GL_FLOAT, false, 0, verticesUntextured);
    glEnableVertexAttribArray(unTextProgram.positionAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(unTextProgram.positionAttribute);
}
