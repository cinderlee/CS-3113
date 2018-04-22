// PONG Assignment
// Cindy Lee
// cl3616

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#define STB_IMAGE_IMPLEMENTATION
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "Matrix.h"
#include "ShaderProgram.h"
#include "stb_image.h"
#include <SDL_mixer.h>

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#define FIXED_TIMESTEP 0.0166666f
Mix_Chunk *paddleSound;
Mix_Chunk *winningSound;

// prototypes
class Entity;
void Setup (Matrix& projection);
void ProcessEvents (SDL_Event& event, bool& done);
void Update (Entity& pad1, Entity& pad2, Entity& ballster, float elapsed, bool& win);
void Render (Entity& pad1, Entity& pad2, Entity& ballster,
             Matrix& model, Matrix& model2, Matrix& model3,
            ShaderProgram& program);
bool Collision (Entity& pad, Entity& ballster);
void Win (Entity& pad1, Entity& pad2);

// class for paddles and ball
class Entity {
public:
    Entity (float xcoord, float ycoord, float widthster,
            float heightster, float velx, float vely):
    x (xcoord), y (ycoord), width (widthster), height (heightster),
    velocity_x (velx), velocity_y (vely) {}
    
    // draws the object
    void Draw(ShaderProgram &program, Matrix& model) {
        glUseProgram(program.programID);
        program.SetModelMatrix(model);
        program.SetColor(red, 1.0f, blue, 1.0f);
        float verticesUntextured[] = {x - width/2, y + height/2, x -width/2, y - height/2, x + width/2, y -height/2, x - width/2, y + height/2, x + width/2, y - height/2, x + width/2, y + height/2};
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, verticesUntextured);
        glEnableVertexAttribArray(program.positionAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(program.positionAttribute);
    }
    
    float x;
    float y;
    float width;
    float height;
    float velocity_x;
    float velocity_y;
    float red = 1.0;
    float blue = 1.0;
};


SDL_Window* displayWindow;

int main(int argc, char *argv[])
{
    Matrix projectionMatrix;
    Matrix viewMatrix;
    
    // sets up the projection matrix and display
    Setup (projectionMatrix);
    ShaderProgram programUntextured;
    programUntextured.Load(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");

    // set the projection matrix and view matrix
    programUntextured.SetProjectionMatrix(projectionMatrix);
    programUntextured.SetViewMatrix(viewMatrix);
    
    // left paddle
    Matrix modelMatrix;
    Entity paddle1 = Entity (-4.5, 0.0, 0.25f, 1.125f, 0.0f, 2.5f);
    
    // right paddle
    Matrix modelMatrix2;
    Entity paddle2 = Entity (4.5, 0.0, 0.25f, 1.125f, 0.0f, 2.5f);
    
    // ball
    Matrix modelMatrix3;
    Entity ball = Entity (0.0f, 0.0f, 0.25f, 0.25f, -2.5f, -2.0f);
    
    SDL_Event event;
    bool done = false;
    bool win = false;
    
    float lastFrameTicks = 0.0f;
    float elapsed = 0.0f;
    float accumulator = 0.0f;
    
    Mix_Music *music;
    music = Mix_LoadMUS ( RESOURCE_FOLDER"BossMain.wav" );
    Mix_VolumeMusic(25);
    Mix_PlayMusic (music, -1);
    
    paddleSound = Mix_LoadWAV (RESOURCE_FOLDER"slimeball.wav");

    winningSound = Mix_LoadWAV (RESOURCE_FOLDER"gold-1.wav");
   
    

    while (!done) {
        
        // polls events
        ProcessEvents(event, done);
        
        float ticks = (float)SDL_GetTicks()/1000.0f;
        elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        elapsed += accumulator;
        if(elapsed < FIXED_TIMESTEP) {
            accumulator = elapsed;
            continue;
        }
        while(elapsed >= FIXED_TIMESTEP) {
            
            Update (paddle1, paddle2, ball, FIXED_TIMESTEP, win);
            elapsed -= FIXED_TIMESTEP;
        }
        accumulator = elapsed;
        
        //draw the objects
        Render (paddle1, paddle2, ball,
                modelMatrix, modelMatrix2, modelMatrix3,
                programUntextured);
    
        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
    return 0;
}

// sets up window and projection matrix
void Setup (Matrix& projection) {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 960, 540, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif
    glViewport(0, 0, 960, 540);
    projection.SetOrthoProjection(-5.325, 5.325, -3.0f, 3.0f, -1.50f, 1.5f);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
}

// polls for events
void ProcessEvents (SDL_Event& event, bool& done){
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }
    }
    glClear(GL_COLOR_BUFFER_BIT);
}

// updates objects' movements
void Update (Entity& pad1, Entity& pad2, Entity& ballster, float elapsed, bool& win) {
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    // key down - move paddle 2 down
    // if at the border - stay at border
    if (keys [SDL_SCANCODE_DOWN]){
        if (pad2.y - pad2.height/2 - (elapsed * pad2.velocity_y) < -3.0f ) {
            pad2.y = -3.0f + pad2.height/2;
        }
        else {
            pad2.y -= elapsed * pad2.velocity_y;
        }
    }
    
    //key up - move paddle 2 up
    //if at the border- stay at boarder
    if (keys [SDL_SCANCODE_UP]) {
        if (pad2.y + pad2.height/2 + (elapsed * pad2.velocity_y) > 3.0f ) {
            pad2.y = 3.0f - pad2.height/2;
        }
        else {
            pad2.y += elapsed * pad2.velocity_y;
        }
    }
    
    //key up - move paddle 1 down
    //if at the border- stay at boarder
    if (keys [SDL_SCANCODE_S]) {
        if (pad1.y - pad1.height/2 - (elapsed * pad1.velocity_y) < -3.0f ) {
            pad1.y = -3.0f + pad1.height/2;
        }
        else {
            pad1.y -= elapsed * pad1.velocity_y;
        }
    }
    
    //key up - move paddle 1 up
    //if at the border- stay at boarder
    if (keys [SDL_SCANCODE_W]) {
        if (pad1.y + pad1.height/2 + (elapsed * pad1.velocity_y) > 3.0f ) {
            pad1.y = 3.0f - pad1.height/2;
        }
        else {
            pad1.y += elapsed * pad1.velocity_y;
        }
    }
    
    // reset the board
    if (keys [SDL_SCANCODE_SPACE]) {
        win = false;
        ballster.x = 0.0f;
        ballster.y = 0.0f;
        ballster.velocity_x = -2.5f;
        ballster.velocity_y = -2.0f;
        pad1.x = -4.5f;
        pad1.y = 0.0f;
        pad2.x = 4.5f;
        pad2.y = 0.0f;
        pad1.width = 0.25f;
        pad1.height = 1.125f;
        pad2.height = 1.125f;
        pad2.width = 0.25f;
        pad2.red = 1.0f;
        pad2.blue = 1.0f;
        pad1.red = 1.0f;
        pad1.blue = 1.0f;
        pad1.velocity_y = 2.5f;
        pad2.velocity_y = 2.5f;
    }
    
    // move the ball by its velocity
    ballster.x += elapsed * ballster.velocity_x;
    ballster.y += elapsed * ballster.velocity_y;
    
    // if collison between paddle and ball on winning screen
    if ( (Collision (pad1, ballster) || Collision (pad2, ballster) ) &&
             win && ballster.velocity_y < 0) {
        ballster.velocity_y *= -1;
    }
    
    // movement of ball on winning screen
    else if (ballster.y >= 1.0f && win && ballster.velocity_y > 0) {
        ballster.velocity_y *= -1;
    }
    
    // collision betwen paddles and ball in middle of game
    else if ((Collision (pad2, ballster) && ballster.velocity_x > 0 && !win) ||
        (Collision (pad1, ballster) && ballster.velocity_x < 0 && !win)){
        Mix_PlayChannel (-1, paddleSound, 0);
        ballster.velocity_x *= -1;
    }
    
    // check for ball at the top border
    else if (ballster.y + ballster.height/2 >= 3.0f && ballster.velocity_y > 0) {
        ballster.velocity_y *= -1;
    }
    
    // check for ball at the bottom border
    else if (ballster.y - ballster.height/2 <= -3.0f && ballster.velocity_y < 0) {
        ballster.velocity_y *= -1;
    }
    
    // check if ball is at the 'goal'
    if ( (ballster.x < -5.325f || ballster.x > 5.325f) && !win) {
        win = true;
        Mix_PlayChannel (-1, winningSound, 0);
        Win (pad1, pad2);
        
        //make winning paddle green
        if (ballster.x < -5.32f) {
            ballster.x = 2.25;
            pad2.red = 0.0f;
            pad2.blue = 0.0f;
        }
        else {
            ballster.x = -2.25;
            pad1.red = 0.0f;
            pad1.blue = 0.0f;
        }
        
        ballster.y = pad1.y + pad1.height/2 + ballster.height/2;
        ballster.velocity_x = 0.0f;
        ballster.velocity_y = 1.0f;
    }
}

// draw the objects
void Render (Entity& pad1, Entity& pad2, Entity& ballster,
             Matrix& model, Matrix& model2, Matrix& model3,
             ShaderProgram& program) {
    pad1.Draw (program,model);
    pad2.Draw (program, model2);
    ballster.Draw (program, model3);
    
}

// check for collision between paddle and ball
bool Collision (Entity& pad, Entity& ballster){
    if (! (pad.y - pad.height/2 > ballster.y + ballster.height / 2 ||
           pad.y + pad.height/2 < ballster.y - ballster.height/2 ||
           pad.x - pad.width/2 > ballster.x + ballster.width/2 ||
           pad.x + pad.width/2 < ballster.x - ballster.width/2) ) {
        return true;
    }
    return false;
}

// when one side wins, adjust entities' attributes
void Win (Entity& pad1, Entity& pad2) {
    pad1.x = -2.25f;
    pad1.y = 0.0f;
    pad1.width = 1.125f;
    pad1.height = 0.25;
    pad2.x = 2.25f;
    pad2.y = 0.0f;
    pad2.width = 1.125f;
    pad2.height = 0.25;
    pad1.velocity_y = 0.0f;
    pad2.velocity_y = 0.0f;
}


