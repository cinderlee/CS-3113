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
#include "GameState.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6
#define MAX_ENEMIES 32

//global variables
enum GameMode { STATE_MAIN_MENU, STATE_GAME_LEVEL, STATE_GAME_OVER};
GameMode mode = STATE_MAIN_MENU;
ShaderProgram program;
int textie = 0;
int spriteSheet = 0;
const Uint8 *keys = SDL_GetKeyboardState(NULL);
SDL_Window* displayWindow;
SheetSprite spritester;
Matrix projectionMatrix;
Matrix viewMatrix;

//prototypes
GLuint LoadTexture(const char *filePath);
void Setup ();
void ProcessEvents (SDL_Event& event, bool& done);
void DrawText(ShaderProgram *program, int fontTexture, std::string text, float size, float spacing);
void DrawWords (ShaderProgram* program, int fontTexture, std::string text, float size, float spacing, float x, float y);
void mainRender ();
void gameLevelUpdate (float elapsed, float& coolDown, float& enemyDown);
void Update (float timestep, float& coolDown, float& enemyDown);
void gameRender ();
void gameOverRender ();
void Render ();
GameState state;


int main(int argc, char *argv[])
{
    Setup ();
    SDL_Event event;
    bool done = false;
    
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    float lastFrameTicks = 0.0f;
    float elapsed = 0.0f;
    float accumulator = 0.0f;
    float coolDown = 0.0f;
    float enemyCoolDown = 0.0f;
    
    while (!done) {
        ProcessEvents(event, done);
        
        float ticks = (float)SDL_GetTicks()/1000.0f;
        elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        elapsed += accumulator;
        if(elapsed < FIXED_TIMESTEP) {
            accumulator = elapsed;
            continue; }
        while(elapsed >= FIXED_TIMESTEP) {
            coolDown += FIXED_TIMESTEP;
            enemyCoolDown += FIXED_TIMESTEP;
            Update (FIXED_TIMESTEP, coolDown, enemyCoolDown);
            Render();
            elapsed -= FIXED_TIMESTEP;
        }
        accumulator = elapsed;
        

        SDL_GL_SwapWindow(displayWindow);
        
    }
    
    SDL_Quit();
    return 0;
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(image);
    return retTexture;
}


void Setup () {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 960, 540, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 960, 540);
    
    program.Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    spriteSheet = LoadTexture(RESOURCE_FOLDER "sheet.png");
    textie = LoadTexture(RESOURCE_FOLDER "font1.png");
    
    state = GameState(spriteSheet);
    
    srand (time (NULL));
    
    // set view and projection matrices
    program.SetViewMatrix(viewMatrix);
    projectionMatrix.SetOrthoProjection(-5.25, 5.25, -3.0f, 3.0f, -1.50f, 1.5f);
    program.SetProjectionMatrix(projectionMatrix);
}

void ProcessEvents (SDL_Event& event, bool& done) {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }
    }
}

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

void DrawWords (ShaderProgram* program, int fontTexture, std::string text, float size, float spacing, float x, float y) {
    Matrix modelMatrix;
    modelMatrix.Translate(x, y, 0.0f);
    program -> SetModelMatrix(modelMatrix);
    program -> SetViewMatrix(viewMatrix);
    
    DrawText(program, fontTexture, text, size, spacing);
}

void mainRender () {
    DrawWords(&program, textie, "SPACE INVADERS", 0.4f, 0.0f, -1*(-0.5f * 0.4) - (14*0.4/2), 0.5f );
    DrawWords(&program, textie, "PRESS SPACE TO START", 0.25f, 0.0f, -1*(-0.5f * 0.25) - (20*0.25/2), -0.5f );
    if (keys [SDL_SCANCODE_SPACE] ){
        mode = STATE_GAME_LEVEL;
    }
}

void gameLevelUpdate (float elapsed, float& coolDown, float& enemyDown) {
    if (keys [SDL_SCANCODE_LEFT] ){
        if (state.player.position.x - state.player.velocity.x * elapsed <= -4.0f) {
            state.player.position.x = -4.0f;
        }
        else {
            state.player.position.x -= state.player.velocity.x * elapsed;
        }
    }
    if (keys [SDL_SCANCODE_RIGHT] ){
        if (state.player.position.x + state.player.velocity.x * elapsed >= 4.0f) {
            state.player.position.x = 4.0f;
        }
        else {
            state.player.position.x += state.player.velocity.x * elapsed;
        }
    }
    
    if (keys [SDL_SCANCODE_UP] && coolDown >= 0.75f) {
        state.shootPlayerBullet(spriteSheet);
        coolDown = 0.0f;
    }
    
    if (enemyDown >= 0.1f) {
        state.shootEnemyBullet(state.enemies [rand() % MAX_ENEMIES], spriteSheet);
        enemyDown = 0.0f;
    }
    
    for (int i = 0; i < state.playerBullets.size (); i++) {
        state.playerBullets[i].position.y += state.playerBullets [i].velocity.y * elapsed;
    }
    
    for (int i = 0; i < state.bullets.size(); i++ ) {
        state.bullets [i].position.y += state.bullets [i].velocity.y * elapsed;
    }
    
    for (int index = 0; index < MAX_ENEMIES; index++) {
        state.enemies [index].position.x += state.enemies[index].velocity.x * elapsed;
    }
    
    if (state.enemies [0].position.x <= -4.0f && state.enemies [0].velocity.x < 0) {
        float x = -4.0f;
        for (int i = 0; i < 8; i++ ) {
            state.enemies [i].position.x = x;
            x += 0.6;
            state.enemies [i].position.y -= state.enemies[i].velocity.y * elapsed ;
            state.enemies [i].velocity.x *= -1;
        }
    }
    if (state.enemies [7].position.x >= 4.0f && state.enemies [7].velocity.x > 0) {
        float x2 = 4.0f;
        for (int i = 7; i >= 0; i-- ) {
            state.enemies [i].position.x = x2;
            x2 -= 0.6f;
            state.enemies [i].position.y -= state.enemies[i].velocity.y * elapsed ;
            state.enemies [i].velocity.x *= -1;
        }
    }
    
    if (state.enemies [8].position.x <= -4.0f && state.enemies [8].velocity.x < 0) {
        float x = -4.0f;
        for (int i = 8; i < 16; i++ ) {
            state.enemies [i].position.x = x;
            x += 0.6;
            state.enemies [i].position.y -= state.enemies[i].velocity.y * elapsed ;
            state.enemies [i].velocity.x *= -1;
        }
    }
    if (state.enemies [15].position.x >= 4.0f && state.enemies [15].velocity.x > 0) {
        float x2 = 4.0f;
        for (int i = 15; i >= 8; i-- ) {
            state.enemies [i].position.x = x2;
            x2 -= 0.6;
            state.enemies [i].position.y -= state.enemies[i].velocity.y * elapsed ;
            state.enemies [i].velocity.x *= -1;
        }
    }
    
    if (state.enemies [16].position.x <= -4.0f && state.enemies [16].velocity.x < 0) {
        float x = -4.0f;
        for (int i = 16; i < 24; i++ ) {
            state.enemies [i].position.x = x;
            x += 0.6;
            state.enemies [i].position.y -= state.enemies[i].velocity.y * elapsed ;
            state.enemies [i].velocity.x *= -1;
        }
    }
    if (state.enemies [23].position.x >= 4.0f && state.enemies [23].velocity.x > 0) {
        float x2 = 4.0f;
        for (int i = 23; i >= 16; i-- ) {
            state.enemies [i].position.x = x2;
            x2 -= 0.6;
            state.enemies [i].position.y -= state.enemies[i].velocity.y * elapsed ;
            state.enemies [i].velocity.x *= -1;
        }
    }
    
    if (state.enemies [24].position.x <= -4.0f && state.enemies [24].velocity.x < 0) {
        float x = -4.0f;
        for (int i = 24; i < MAX_ENEMIES; i++ ) {
            state.enemies [i].position.x = x;
            x += 0.6;
            state.enemies [i].position.y -= state.enemies[i].velocity.y * elapsed ;
            state.enemies [i].velocity.x *= -1;
        }
    }
    if (state.enemies [31].position.x >= 4.0f && state.enemies [31].velocity.x > 0) {
        float x2 = 4.0f;
        for (int i = 31; i >= 24; i-- ) {
            state.enemies [i].position.x = x2;
            x2 -= 0.6;
            state.enemies [i].position.y -= state.enemies[i].velocity.y * elapsed ;
            state.enemies [i].velocity.x *= -1;
        }
    }
    if (state.gameOver()) {
        mode = STATE_GAME_OVER;
    }
    if (keys [SDL_SCANCODE_R]) {
        state.reset();
        mode = STATE_MAIN_MENU;
    }
}

void Update (float timestep, float& coolDown, float& enemyDown) {
    switch (mode) {
        case STATE_MAIN_MENU:
            break;
        case STATE_GAME_LEVEL:
            gameLevelUpdate (FIXED_TIMESTEP, coolDown, enemyDown);
            state.Collision ();
            if (keys [SDL_SCANCODE_R]) {
                state.reset();
                mode = STATE_MAIN_MENU;
            }
            break;
        case STATE_GAME_OVER:
            if (keys [SDL_SCANCODE_R]) {
                state.reset();
                mode = STATE_MAIN_MENU;
            }
            break;
    }
}

void gameRender () {
    state.Draw(&program);
    DrawWords(&program, textie, "SCORE: " + std::to_string (state.score), 0.3f, 0.0f, -1*(-0.5f * 0.3) - (15*0.3/2) - 2.5f, 2.5f );
    DrawWords(&program, textie, "LIVES: " + std::to_string (state.lives), 0.3f, 0.0f, -1*(-0.5f * 0.3) - (15*0.3/2) + 2.5f, 2.5f );
}

void gameOverRender (){
    if (state.player.alive == false) {
        DrawWords(&program, textie, "GAME OVER", 0.4f, 0.0f, -1*(-0.5f * 0.4) - (9*0.4/2), 0.5f );
        DrawWords(&program, textie, "You Lost", 0.4f, 0.0f, -1*(-0.5f * 0.4) - (8*0.4/2), -0.5f );
    }
    else {
        DrawWords(&program, textie, "CONGRATULATIONS", 0.4f, 0.0f, -1*(-0.5f * 0.4) - (15*0.4/2), 0.5f );
        DrawWords(&program, textie, "YOU WON", 0.4f, 0.0f, -1*(-0.5f * 0.4) - (7*0.4/2), -0.5f );
    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(program.programID);
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
}

