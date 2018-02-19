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

class Entity;
void Setup (Matrix& projection);
void ProcessEvents (SDL_Event& event, bool& done);
void Update (Entity& pad1, Entity& pad2, Entity& ballster, float elapsed);
void Render (Entity& pad1, Entity& pad2, Entity& ballster,
             Matrix& model, Matrix& model2, Matrix& model3,
             Matrix& view, Matrix& view2, Matrix& view3, ShaderProgram& program);
bool Collision (Entity& pad, Entity& ballster);

class Entity {
public:
    Entity (float xcoord, float ycoord, float widthster,
            float heightster, float velx, float vely):
    x (xcoord), y (ycoord), width (widthster), height (heightster),
    velocity_x (velx), velocity_y (vely) {}
    
    void Draw(ShaderProgram &program, Matrix& model, Matrix& view) {
        glUseProgram(program.programID);
        program.SetModelMatrix(model);
        program.SetViewMatrix(view);
        program.SetColor(1.0, 1.0, 1.0, 0.0f);
        float verticesUntextured[] = {x - width/2, y + height/2, x -width/2, y - height/2, x + width/2, y -height/2, x - width/2, y + height/2, x + width/2, y - height/2, x + width/2, y + height/2};
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, verticesUntextured);
        glEnableVertexAttribArray(program.positionAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(program.positionAttribute);
    }
    
    float x;
    float y;
    float rotation;
    int textureID;
    float width;
    float height;
    float velocity_x;
    float velocity_y;

};


SDL_Window* displayWindow;

int main(int argc, char *argv[])
{
    Matrix projectionMatrix;
    Setup (projectionMatrix);
    ShaderProgram programUntextured;
    programUntextured.Load(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");

    Matrix viewMatrix;
    Matrix modelMatrix;
    Entity paddle1 = Entity (-3.0, 0.0, 0.15f, 0.5f, 0.0f, 2.5f);
    
    Matrix viewMatrix2;
    Matrix modelMatrix2;
    Entity paddle2 = Entity (3.0, 0.0, 0.15, 0.5f, 0.0f, 2.5f);
    
    Matrix viewMatrix3;
    Matrix modelMatrix3;
    Entity ball = Entity (0.0f, 0.0f, 0.15f, 0.15f, -0.025f, -0.025f);
    
    SDL_Event event;
    bool done = false;
    
    float lastFrameTicks = 0.0f;
    float elapsed = 0.0f;

    
    while (!done) {
        
        ProcessEvents(event, done);
        
        float ticks = (float)SDL_GetTicks()/1000.0f;
        elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        programUntextured.SetProjectionMatrix(projectionMatrix);
        
        Update (paddle1, paddle2, ball, elapsed);
        
        Render (paddle1, paddle2, ball,
                modelMatrix, modelMatrix2, modelMatrix3,
                viewMatrix, viewMatrix2, viewMatrix3, programUntextured);
    
        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
    return 0;
}

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

void ProcessEvents (SDL_Event& event, bool& done){
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }
    }
    glClear(GL_COLOR_BUFFER_BIT);
}

void Update (Entity& pad1, Entity& pad2, Entity& ballster, float elapsed) {
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    if (keys [SDL_SCANCODE_DOWN]){
        if (pad2.y - pad2.height/2 - (elapsed * pad2.velocity_y) < -2.0f ) {
            pad2.y = -2.0f + pad2.height/2;
        }
        else {
            pad2.y -= elapsed * pad2.velocity_y;
        }
    }
    //else if(event.key.keysym.scancode == SDL_SCANCODE_UP) {
    if (keys [SDL_SCANCODE_UP]) {
        if (pad2.y + pad2.height/2 + (elapsed * pad2.velocity_y) > 2.0f ) {
            pad2.y = 2.0f - pad2.height/2;
        }
        else {
            pad2.y += elapsed * pad2.velocity_y;
        }
    }
    //if(event.key.keysym.scancode == SDL_SCANCODE_S) {
    if (keys [SDL_SCANCODE_S]) {
        if (pad1.y - pad1.height/2 - (elapsed * pad1.velocity_y) < -2.0f ) {
            pad1.y = -2.0f + pad1.height/2;
        }
        else {
            pad1.y -= elapsed * pad1.velocity_y;
        }
    }
    //else if(event.key.keysym.scancode == SDL_SCANCODE_W) {
    if (keys [SDL_SCANCODE_W]) {
        if (pad1.y + pad1.height/2 + (elapsed * pad1.velocity_y) > 2.0f ) {
            pad1.y = 2.0f - pad1.height/2;
        }
        else {
            pad1.y += elapsed * pad1.velocity_y;
        }
    }
    
    ballster.x += 0.5 * ballster.velocity_x;
    ballster.y += 0.5 * ballster.velocity_y;
    
    if ((Collision (pad2, ballster) && ballster.velocity_x > 0) ||
        (Collision (pad1, ballster) && ballster.velocity_x < 0)){
        ballster.velocity_x *= -1;
    }
    else if (ballster.y + ballster.height/2 >= 2.0f) {
        ballster.velocity_y *= -1;
    }
    else if (ballster.y - ballster.height/2 <= -2.0f) {
        ballster.velocity_y *= -1;
    }
}

void Render (Entity& pad1, Entity& pad2, Entity& ballster,
             Matrix& model, Matrix& model2, Matrix& model3,
             Matrix& view, Matrix& view2, Matrix& view3, ShaderProgram& program) {
    pad1.Draw (program,model,view);
    pad2.Draw (program, model2, view2);
    ballster.Draw (program, model3, view3);
    
}

bool Collision (Entity& pad, Entity& ballster){
    if (! (pad.y - pad.height/2 > ballster.y + ballster.height / 2 ||
           pad.y + pad.height/2 < ballster.y - ballster.height/2 ||
           pad.x - pad.width/2 > ballster.x + ballster.width/2 ||
           pad.x + pad.width/2 < ballster.x - ballster.width/2) ) {
        return true;
    }
    return false;
}
