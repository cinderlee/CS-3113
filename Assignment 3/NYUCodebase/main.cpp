#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#define STB_IMAGE_IMPLEMENTATION
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <vector>
#include "Matrix.h"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "Entity.h"
#include "SheetSprite.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6


enum GameMode { STATE_MAIN_MENU, STATE_GAME_LEVEL};
GameMode mode = STATE_MAIN_MENU;
ShaderProgram program;
int textie = 0;
int spriteSheet = 0;
const Uint8 *keys = SDL_GetKeyboardState(NULL);
SDL_Window* displayWindow;
Entity player = Entity (0.0f, -1.5f, 0.0f);
SheetSprite spritester;
Matrix projectionMatrix;
Matrix viewMatrix;

class GameState {
public:
    GameState() {}
    void Draw (ShaderProgram* program) {
        player.Draw (program);
        for (int i = 0; i < 15; i++){
            enemies [i].Draw(program);
        }
    }
    Entity player;
    Entity enemies[15];
    Entity bullets[10];
    int score = 0;
};
GameState state;

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
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 880, 450, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 880, 450);
    
    program.Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    spriteSheet = LoadTexture(RESOURCE_FOLDER "sheet.png");
    textie = LoadTexture(RESOURCE_FOLDER "font1.png");
    
    program.SetViewMatrix(viewMatrix);
    projectionMatrix.SetOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
    program.SetProjectionMatrix(projectionMatrix);
    
    spritester = SheetSprite(spriteSheet, 112.0f/1024.0f, 791.0f/1024.0f, 112.0f/1024.0f, 75.0f/1024.0f, 0.2f);
    player.sprite = spritester;
    
    state.player = player;
    
    float x = 0.0f;
    float y= 0.0f;
    float z = 0.0f;
    state.enemies [0] = Entity (x, y, z);
    SheetSprite enemy  = SheetSprite(spriteSheet, 444.0f/1024.0f, 0.0/1024.0f, 91.0f/1024.0f, 91.0f/1024.0f, 0.2f);
    state.enemies [0].sprite = enemy;
    for (int i = 1; i < 15; i++ ) {
        if (i % 5 == 0) {
            x = 0.0f;
            y += 0.5f;
        }
        else {
            x += 0.5;
        }
        state.enemies [i] = Entity (x, y, z);
        state.enemies [i].sprite = enemy;
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
    DrawWords(&program, textie, "SPACE INVADERS", 0.3f, 0.0f, -1*(-0.5f * 0.3) - (14*0.3/2), 0.0f );
    DrawWords(&program, textie, "Press Space to Start", 0.2f, 0.0f, -1*(-0.5f * 0.2) - (20*0.2/2), -1.0f );
    if (keys [SDL_SCANCODE_SPACE] ){
        mode = STATE_GAME_LEVEL;
    }
}

void gameLevelUpdate (float elapsed) {
    if (keys [SDL_SCANCODE_LEFT] ){
        if (state.player.position.x - state.player.velocity.x * elapsed <= -2.5f) {
            state.player.position.x = -2.5f;
        }
        else {
            state.player.position.x -= state.player.velocity.x * elapsed;
        }
    }
    if (keys [SDL_SCANCODE_RIGHT] ){
        if (state.player.position.x + state.player.velocity.x * elapsed >= 2.5f) {
            state.player.position.x = 2.5f;
        }
        else {
            state.player.position.x += state.player.velocity.x * elapsed;
        }
    }
    
    for (int index = 0; index < 15; index++) {
        state.enemies [index].position.x += state.enemies[index].velocity.x * elapsed;
    }
    
    if (state.enemies [0].position.x <= -2.5f && state.enemies [0].velocity.x < 0) {
        float x = -2.5f;
        for (int i = 0; i < 5; i++ ) {
            state.enemies [i].position.x = x;
            x += 0.5;
            state.enemies [i].velocity.x *= -1;
        }
    }
    if (state.enemies [4].position.x >= 2.5f && state.enemies [4].velocity.x > 0) {
        float x2 = 2.5f;
        for (int i = 4; i >= 0; i-- ) {
            state.enemies [i].position.x = x2;
            std::cout << state.enemies [i].position.x;
            x2 -= 0.5f;
            state.enemies [i].velocity.x *= -1;
        }
    }
    
    if (state.enemies [5].position.x <= -2.5f && state.enemies [5].velocity.x < 0) {
        float x = -2.5f;
        for (int i = 5; i < 10; i++ ) {
            state.enemies [i].position.x = x;
            x += 0.5;
            state.enemies [i].velocity.x *= -1;
        }
    }
    if (state.enemies [9].position.x >= 2.5f && state.enemies [9].velocity.x > 0) {
        float x2 = 2.5f;
        for (int i = 9; i >= 5; i-- ) {
            state.enemies [i].position.x = x2;
            x2 -= 0.5;
            state.enemies [i].velocity.x *= -1;
        }
    }
    
    if (state.enemies [10].position.x <= -2.5f && state.enemies [10].velocity.x < 0) {
        float x = -2.5f;
        for (int i = 10; i < 15; i++ ) {
            state.enemies [i].position.x = x;
            x += 0.5;
            state.enemies [i].velocity.x *= -1;
        }
    }
    if (state.enemies [14].position.x >= 2.5f && state.enemies [14].velocity.x > 0) {
        float x2 = 2.5f;
        for (int i = 14; i >= 10; i-- ) {
            state.enemies [i].position.x = x2;
            x2 -= 0.5;
            state.enemies [i].velocity.x *= -1;
        }
    }
    
}

void Render(float elapsed) {
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(program.programID);
    switch(mode) {
        case STATE_MAIN_MENU:
            mainRender();
            break;
        case STATE_GAME_LEVEL:
            //player.Draw (&program);
            
            gameLevelUpdate (FIXED_TIMESTEP);
            state.Draw(&program);
            break;
    } }


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
    
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }
        
        float ticks = (float)SDL_GetTicks()/1000.0f;
        elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        elapsed += accumulator;
        if(elapsed < FIXED_TIMESTEP) {
            accumulator = elapsed;
            continue; }
        while(elapsed >= FIXED_TIMESTEP) {
            //Update(FIXED_TIMESTEP);
            Render(FIXED_TIMESTEP);
            elapsed -= FIXED_TIMESTEP;
        }
        accumulator = elapsed;
        
        SDL_GL_SwapWindow(displayWindow);
        
    }
    
    SDL_Quit();
    return 0;
}
