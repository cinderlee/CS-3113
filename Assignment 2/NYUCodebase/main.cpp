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

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

// prototypes
class Entity;
void Setup (Matrix& projection);
void ProcessEvents (SDL_Event& event, bool& done);
void Update (Entity& pad1, Entity& pad2, Entity& ballster, float elapsed, bool& win);
void Render (Entity& pad1, Entity& pad2, Entity& ballster,
             Matrix& model, Matrix& model2, Matrix& model3,
             Matrix& view, Matrix& view2, Matrix& view3, ShaderProgram& program);
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
    void Draw(ShaderProgram &program, Matrix& model, Matrix& view) {
        glUseProgram(program.programID);
        program.SetModelMatrix(model);
        program.SetViewMatrix(view);
        program.SetColor(red, green, blue, 0.0f);
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
    float green = 1.0;
};


SDL_Window* displayWindow;

int main(int argc, char *argv[])
{
    Matrix projectionMatrix;
    
    // sets up the projection matrix and display
    Setup (projectionMatrix);
    ShaderProgram programUntextured;
    programUntextured.Load(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");

    // left paddle
    Matrix viewMatrix;
    Matrix modelMatrix;
    Entity paddle1 = Entity (-3.0, 0.0, 0.15f, 0.75f, 0.0f, 2.5f);
    
    // right paddle
    Matrix viewMatrix2;
    Matrix modelMatrix2;
    Entity paddle2 = Entity (3.0, 0.0, 0.15, 0.75f, 0.0f, 2.5f);
    
    // ball
    Matrix viewMatrix3;
    Matrix modelMatrix3;
    Entity ball = Entity (0.0f, 0.0f, 0.15f, 0.15f, -1.5f, -1.5f);
    
    SDL_Event event;
    bool done = false;
    bool win = false;
    
    float lastFrameTicks = 0.0f;
    float elapsed = 0.0f;

    
    while (!done) {
        
        // polls events
        ProcessEvents(event, done);
        
        float ticks = (float)SDL_GetTicks()/1000.0f;
        elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        // set the projection matrix
        programUntextured.SetProjectionMatrix(projectionMatrix);
        
        // update objects' movement
        Update (paddle1, paddle2, ball, elapsed, win);
        
        //draw the objects
        Render (paddle1, paddle2, ball,
                modelMatrix, modelMatrix2, modelMatrix3,
                viewMatrix, viewMatrix2, viewMatrix3, programUntextured);
    
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
    
    projection.SetOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
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
        if (pad2.y - pad2.height/2 - (elapsed * pad2.velocity_y) < -2.0f ) {
            pad2.y = -2.0f + pad2.height/2;
        }
        else {
            pad2.y -= elapsed * pad2.velocity_y;
        }
    }
    
    //key up - move paddle 2 up
    //if at the border- stay at boarder
    if (keys [SDL_SCANCODE_UP]) {
        if (pad2.y + pad2.height/2 + (elapsed * pad2.velocity_y) > 2.0f ) {
            pad2.y = 2.0f - pad2.height/2;
        }
        else {
            pad2.y += elapsed * pad2.velocity_y;
        }
    }
    
    //key up - move paddle 1 down
    //if at the border- stay at boarder
    if (keys [SDL_SCANCODE_S]) {
        if (pad1.y - pad1.height/2 - (elapsed * pad1.velocity_y) < -2.0f ) {
            pad1.y = -2.0f + pad1.height/2;
        }
        else {
            pad1.y -= elapsed * pad1.velocity_y;
        }
    }
    
    //key up - move paddle 1 up
    //if at the border- stay at boarder
    if (keys [SDL_SCANCODE_W]) {
        if (pad1.y + pad1.height/2 + (elapsed * pad1.velocity_y) > 2.0f ) {
            pad1.y = 2.0f - pad1.height/2;
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
        ballster.velocity_x = -1.5f;
        ballster.velocity_y = -1.5f;
        pad1.x = -3.0f;
        pad1.y = 0.0f;
        pad2.x = 3.0f;
        pad2.y = 0.0f;
        pad1.width = 0.15f;
        pad1.height = 0.75f;
        pad2.height = 0.75f;
        pad2.width = 0.15f;
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
    else if (ballster.y >= 0.5f && win && ballster.velocity_y > 0) {
        ballster.velocity_y *= -1;
    }
    
    // collision betwen paddles and ball in middle of game
    else if ((Collision (pad2, ballster) && ballster.velocity_x > 0 && !win) ||
        (Collision (pad1, ballster) && ballster.velocity_x < 0 && !win)){
        ballster.velocity_x *= -1;
    }
    
    // check for ball at the top border
    else if (ballster.y + ballster.height/2 >= 2.0f && ballster.velocity_y > 0) {
        ballster.velocity_y *= -1;
    }
    
    // check for ball at the bottom border
    else if (ballster.y - ballster.height/2 <= -2.0f && ballster.velocity_y < 0) {
        ballster.velocity_y *= -1;
    }
    
    // check if ball is at the 'goal'
    if ( (ballster.x < -3.55f || ballster.x > 3.55f) && !win) {
        win = true;
        Win (pad1, pad2);
        if (ballster.x < -3.55f) {
            ballster.x = 1.77f;
            pad2.red = 0.0f;
            pad2.blue = 0.0f;
        }
        else {
            ballster.x = -1.77f;
            pad1.red = 0.0f;
            pad1.blue = 0.0f;
        }
        ballster.y = pad1.y + pad1.height/2;
        ballster.velocity_x = 0.0f;
        ballster.velocity_y = 1.0f;
    }
}

// draw the objects
void Render (Entity& pad1, Entity& pad2, Entity& ballster,
             Matrix& model, Matrix& model2, Matrix& model3,
             Matrix& view, Matrix& view2, Matrix& view3, ShaderProgram& program) {
    pad1.Draw (program,model,view);
    pad2.Draw (program, model2, view2);
    ballster.Draw (program, model3, view3);
    
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

// when one side wins
void Win (Entity& pad1, Entity& pad2) {
    pad1.x = -1.77f;
    pad1.y = 0.0f;
    pad1.width = 0.75f;
    pad1.height = 0.15f;
    pad2.x = 1.77f;
    pad2.y = 0.0f;
    pad2.width = 0.75f;
    pad2.height = 0.15f;
    pad1.velocity_y = 0.0f;
    pad2.velocity_y = 0.0f;
}


