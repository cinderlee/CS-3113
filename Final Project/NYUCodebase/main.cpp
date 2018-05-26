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

// In this platform, if you hit an enemy or spikes you return back to the beginning
// In order to win, you must reach the end of the game and receive the key

ShaderProgram program;
Matrix viewMatrix;
Matrix projectionMatrix;
const Uint8 *keys = SDL_GetKeyboardState(NULL);
GameState state;
float viewX = 0.0f;
float viewY = 0.0f;
int textie;
int tilesheet;
int playerSheet;
int enemySheet;
int backgroundOne;
int backgroundTwo;
int backgroundThree;
int tilesOne;
int tilesThree;
int hillsOne;
int hillsThree;

bool win = false;
enum GameMode { STATE_MAIN_MENU, STATE_GAME_LEVEL, STATE_GAME_OVER};
GameMode mode = STATE_MAIN_MENU;

void LoadingTextures();
void Setup();
void ProcessEvents (SDL_Event& event, bool& done);
void Update (float elapsed, int& direction);
void Render ();
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
    
    
    float lastFrameTicks = 0.0f;
    float elapsed = 0.0f;
    float accumulator = 0.0f;
    
    int direction = 1;
    
    while (!done) {
        ProcessEvents (event, done);
        
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
            Update (FIXED_TIMESTEP, direction);
            elapsed -= FIXED_TIMESTEP;
        }
        accumulator = elapsed;
        Render ();
        
        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
    return 0;
}

void LoadingTextures () {
    tilesheet = LoadTexture(RESOURCE_FOLDER"Resources/tilesheet_complete.png");
    playerSheet = LoadTexture(RESOURCE_FOLDER"Resources/spritesheet_complete.png");
    enemySheet = LoadTexture(RESOURCE_FOLDER"Resources/enemies.png");
    textie = LoadTexture(RESOURCE_FOLDER "Resources/font1.png");
    backgroundOne = LoadTexture(RESOURCE_FOLDER"Resources/set1_background.png");
    backgroundTwo = LoadTexture(RESOURCE_FOLDER"Resources/bg_castle.png");
    backgroundThree = LoadTexture(RESOURCE_FOLDER"Resources/set3_background.png");
    hillsOne = LoadTexture(RESOURCE_FOLDER"Resources/set1_hills.png");
    hillsThree = LoadTexture(RESOURCE_FOLDER"Resources/set3_hills.png");
    tilesOne = LoadTexture(RESOURCE_FOLDER"Resources/set1_tiles.png");
    tilesThree = LoadTexture(RESOURCE_FOLDER"Resources/set3_tiles.png");
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
    LoadingTextures();
    
    state.Initiate (tilesheet, playerSheet, enemySheet);
    
    glUseProgram(program.programID);
    
    // set view and projection matrices
    projectionMatrix.SetOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
    viewX = -3.55f;
    viewY = -1 * state.player.position.y;
    if (viewY >= state.mappy -> mapHeight * TILE_SIZE - 2.0) {
        viewY = state.mappy -> mapHeight * TILE_SIZE - 2.0;
    }
    program.SetProjectionMatrix(projectionMatrix);
    
}

// processing events
void ProcessEvents (SDL_Event& event, bool& done) {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }
    }
}


//updating the game
void GameLevelUpdate (float elapsed) {
    state.Collision();
    
    
    // applying friction
    state.player.velocity.x = lerp(state.player.velocity.x, 0.0f, elapsed * 1.5);
    
    //move left
    if (keys [SDL_SCANCODE_LEFT] ){
        state.player.velocity.x = -1.0f;
        
        state.player.acceleration.x = 1.5f;
        
        state.player.velocity.x += state.player.acceleration.x * elapsed;
        
        
    }
    
    // move right
    if (keys [SDL_SCANCODE_RIGHT] ){
        state.player.velocity.x = 1.0f;
        
        state.player.acceleration.x = 1.5f;
        
        state.player.velocity.x += state.player.acceleration.x * elapsed;
    }
    
    // "jumping"
    if (keys [SDL_SCANCODE_UP] ){
        
        state.player.velocity.y = 1.0f;
        
        state.player.acceleration.y = 1.0f;
        
        state.player.velocity.y += state.player.acceleration.y * elapsed;
        
        state.player.velocity.y += state.player.gravity.y * elapsed;
        
    }
    
    // calculting positions
    state.player.position.x += state.player.velocity.x * elapsed;
    state.player.velocity.y += state.player.acceleration.y * elapsed;
    state.player.position.y += state.player.velocity.y * elapsed;
    state.player.acceleration.x = 0.0f;
    
    // applying gravity and changes in y
    state.player.velocity.y += state.player.acceleration.y * elapsed;
    state.player.position.y += state.player.velocity.y * elapsed;
    state.player.gravity.y = 0.0f;
    
    // if moving past the left edge of game
    if (state.player.position.x - state.player.sizeEnt.x/2 <= 0.0f) {
        state.player.position.x = 0.0f + state.player.sizeEnt.x/2;
    }
    
    //if hitting the top of the window (can happen when trying to jump onto clouds)
    if (state.player.position.y + state.player.sizeEnt.y/2 >= 0) {
        state.player.position.y = 0 - state.player.sizeEnt.y/2;
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
    
    state.player.collisionBools();
}

void mainRender () {
    viewMatrix.Identity();
    viewMatrix.SetPosition (viewX, viewY, 0.0f);
    program.SetViewMatrix(viewMatrix);
    DrawTexture (backgroundOne, MAP_WIDTH/6 *0.3, -MAP_HEIGHT/2 * 0.3, MAP_WIDTH/3 * 0.3, MAP_HEIGHT * 0.3);
    DrawTexture (tilesOne, MAP_WIDTH/6 *0.3, -MAP_HEIGHT/2 * 0.3, MAP_WIDTH/3 * 0.3, MAP_HEIGHT * 0.3);
    DrawTexture (hillsOne, MAP_WIDTH/6 *0.3, -MAP_HEIGHT/2 * 0.3, MAP_WIDTH/3 * 0.3, MAP_HEIGHT * 0.3);
   
    DrawTexture (backgroundTwo, MAP_WIDTH/6 *0.3 + (MAP_WIDTH/3 * 0.3), -MAP_HEIGHT/2 * 0.3, MAP_WIDTH/3 * 0.3, MAP_HEIGHT * 0.3);
    DrawTexture (backgroundThree, MAP_WIDTH/6 *0.3 + 2 * (MAP_WIDTH/3) * 0.3, -MAP_HEIGHT/2 * 0.3, MAP_WIDTH/3 * 0.3, MAP_HEIGHT * 0.3);
    DrawTexture (tilesThree, MAP_WIDTH/6 *0.3 + 2 * (MAP_WIDTH/3) * 0.3, -MAP_HEIGHT/2 * 0.3, MAP_WIDTH/3 * 0.3, MAP_HEIGHT * 0.3);
    DrawTexture (hillsThree, MAP_WIDTH/6 *0.3 + 2 * (MAP_WIDTH/3) * 0.3, -MAP_HEIGHT/2 * 0.3, MAP_WIDTH/3 * 0.3, MAP_HEIGHT * 0.3);
    state.Draw (&program);
    DrawWords (&program, textie, "Alien Invaders", 0.4, 0.0f, (viewX + (-0.5*0.4) + (14*0.4/2)), viewY - 0.5f);
    DrawWords (&program, textie, "Press Space to Start", 0.25f, 0.0f, viewX + (-0.5f*0.25) + (20*0.25/2), viewY + 0.5f );
}

void mainUpdate (float elapsed, int& direction) {
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
    if (keys [SDL_SCANCODE_SPACE] ){
        mode = STATE_GAME_LEVEL;
        state.UpdateLevel ();
        viewX = 0.0f;
        viewY= 0.0f;
    }
}

void gameRender () {
    viewMatrix.Identity();
    viewMatrix.SetPosition(viewX, viewY, 0.0f);
    program.SetViewMatrix(viewMatrix);
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
            //gameOverRender();
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

void Update (float elapsed, int& direction ){
    switch (mode) {
        case STATE_MAIN_MENU:
            mainUpdate(elapsed, direction);
            break;
        case STATE_GAME_LEVEL:
            GameLevelUpdate (FIXED_TIMESTEP);
//            state.Collision ();
//            if (keys [SDL_SCANCODE_R]) {
//                mode = STATE_MAIN_MENU;
//            }
            break;
        case STATE_GAME_OVER:
            if (keys [SDL_SCANCODE_R]) {
                mode = STATE_MAIN_MENU;
            }
            break;
    }
}


//drawing words - needed for winning moment
void DrawWords (ShaderProgram* program, int fontTexture, std::string text, float size, float spacing, float x, float y) {
    Matrix modelMatrix;
    modelMatrix.SetPosition(-x, -y, 0.0f);
    program -> SetModelMatrix(modelMatrix);
    program -> SetViewMatrix(viewMatrix);
    
    DrawText(program, fontTexture, text, size, spacing);
}


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
