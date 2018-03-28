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


ShaderProgram program;
Matrix viewMatrix;
Matrix projectionMatrix;
const Uint8 *keys = SDL_GetKeyboardState(NULL);
GameState state;
float distance;
int textie;
int arne;
bool win = false;


void Setup();
void ProcessEvents (SDL_Event& event, bool& done);
void Update (float elapsed);
void Render ();
void DrawText(ShaderProgram *program, int fontTexture, std::string text, float size, float spacing);
void DrawWords (ShaderProgram* program, int fontTexture, std::string text, float size, float spacing, float x, float y);

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#define FIXED_TIMESTEP 0.0166666f
#define MAP_WIDTH 128
#define MAP_HEIGHT 32
#define TILE_SIZE 0.2f

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
            Update (FIXED_TIMESTEP);
            Render ();
            elapsed -= FIXED_TIMESTEP;
        }
        accumulator = elapsed;
    
        
        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
    return 0;
}

// setting up game
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
    arne = LoadTexture(RESOURCE_FOLDER"arne_sprites.png");
    textie = LoadTexture(RESOURCE_FOLDER "font1.png");
    
    state.Initiate (arne, RESOURCE_FOLDER"NewMap.txt");
    
    glUseProgram(program.programID);
    
    // set view and projection matrices
    projectionMatrix.SetOrthoProjection(-5.25, 5.25, -3.0f, 3.0f, -1.50f, 1.5f);
    viewMatrix.Translate (state.player.position.x, 3.0f, 0.0f);
    program.SetViewMatrix(viewMatrix);
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
void Update (float elapsed) {
    state.Collision();
    
    // adjusting enemy flying movement
    for (int index = 0 ; index < state.enemies.size () ; index++ ) {
        state.enemies[index].velocity.x += state.enemies[index].acceleration.x * elapsed;
        state.enemies[index].velocity.y += state.enemies[index].acceleration.y * elapsed;
        
        state.enemies[index].position.x += state.enemies[index].velocity.x * elapsed;
        state.enemies[index].position.y += state.enemies[index].velocity.y * elapsed;
        
        if (state.enemies [index].position.y >= -3.5f) {
            state.enemies [index].position.y = -3.5f;
            state.enemies [index].velocity.y *= -1;
        }
        
        if (state.enemies [index].position.y <= -4.5f) {
            state.enemies [index].position.y = -4.5f;
            state.enemies [index].velocity.y *= -1;
        }
        
    }
    
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
    if (state.player.position.y + state.player.sizeEnt.y/2 >= 0.0f) {
        state.player.position.y = 0.0f - state.player.sizeEnt.y/2;
    }
    
    
    // if key and player positions matches
    if (!win && state.player.position.x >= state.key.position.x &&
        state.key.position.y <= state.player.position.y ){
        win = true;
        state.key.position.y = 5.0f;
        state.key.velocity.y = 0.0f;
    }
    
    // if player position aligned with key's x position
    else if (!win && state.player.position.x >= state.key.position.x) {
        
        state.player.position.x = state.key.position.x;
        state.player.velocity.x = 0.0f;
        state.key.velocity.y = -1.0f;
        state.key.position.y += state.key.velocity.y * elapsed;
    }
    
    
    // adjusting viewMatrix
    float translationX;
    viewMatrix.Identity();
    if (state.player.position.x - 5.25f <= 0) {
        translationX = 5.25f;
        viewMatrix.Translate(-1 * translationX, 3.0f, 0.0f);
    }
    else if (state.player.position.x + 5.25f >= MAP_WIDTH * TILE_SIZE) {
        
        translationX = MAP_WIDTH * TILE_SIZE - 5.25f;
        viewMatrix.Translate(-1 * translationX, 3.0f, 0.0f);
    }
    else {
        viewMatrix.Translate(-1 * state.player.position.x, 3.0f, 0.0f);
    }
    program.SetViewMatrix(viewMatrix);
    
}

// drawing game
void Render () {
    state.Draw (&program);
    
    if (win) {
        DrawWords(&program, textie, "YOU WON", 0.4f, 0.0f, MAP_WIDTH * TILE_SIZE - 5.25f - (7 / 2 * 0.4f), MAP_HEIGHT/2 * -1 * TILE_SIZE + 0.2f );
    }
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

//drawing words - needed for winning moment
void DrawWords (ShaderProgram* program, int fontTexture, std::string text, float size, float spacing, float x, float y) {
    Matrix modelMatrix;
    modelMatrix.Translate(x, y, 0.0f);
    program -> SetModelMatrix(modelMatrix);
    program -> SetViewMatrix(viewMatrix);
    
    DrawText(program, fontTexture, text, size, spacing);
}
