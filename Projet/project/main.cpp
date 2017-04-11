// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"
#include "glm/gtc/matrix_transform.hpp"

#include "framebuffer.h"

#include "cube/cube.h"
#include "quad/quad.h"
#include "screenquad/screenquad.h"
#include "trackball.h"

Cube cube;
Quad quad;

int window_width = 800;
int window_height = 600;

FrameBuffer framebuffer;
ScreenQuad screenquad;
Trackball trackball;

using namespace glm;

mat4 projection_matrix;
mat4 view_matrix;
mat4 cube_model_matrix;
mat4 trackball_matrix;
mat4 old_trackball_matrix;

double zoom;
float filter = 2.0f;

void Init(GLFWwindow* window) {
    glClearColor(1.0, 1.0, 1.0 /*white*/, 1.0 /*solid*/);
    glEnable(GL_DEPTH_TEST);

    // setup view and projection matrices
    vec3 cam_pos(2.0f, 2.0f, 2.0f);
    vec3 cam_look(0.0f, 0.0f, 0.0f);
    vec3 cam_up(0.0f, 0.0f, 1.0f);
    view_matrix = lookAt(cam_pos, cam_look, cam_up);
    view_matrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, -4.0f));
    float ratio = window_width / (float) window_height;
    projection_matrix = perspective(45.0f, ratio, 0.001f, 10.0f);

    // create the model matrix (remember OpenGL is right handed)
    // accumulated transformation
    cube_model_matrix = scale(IDENTITY_MATRIX, vec3(0.5));
    cube_model_matrix = translate(cube_model_matrix, vec3(0.0, 0.0, 0.6));

    // on retina/hidpi displays, pixels != screen coordinates
    // this unsures that the framebuffer has the same size as the window
    // (see http://www.glfw.org/docs/latest/window.html#window_fbsize)
    glfwGetFramebufferSize(window, &window_width, &window_height);
    GLuint framebuffer_texture_id = framebuffer.Init(window_width, window_height);
    screenquad.Init(window_width, window_height);
    quad.Init(framebuffer_texture_id);

}

void Display() {
    // render to framebuffer
    framebuffer.Clear();
    framebuffer.Bind();
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        screenquad.Draw();
    }
    framebuffer.Unbind();

    // render to Window
    glViewport(0, 0, window_width, window_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    quad.Draw(trackball_matrix, view_matrix, projection_matrix);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

// gets called when the windows/framebuffer is resized.
void ResizeCallback(GLFWwindow* window, int width, int height) {
    window_width = width;
    window_height = height;

    float ratio = window_width / (float) window_height;
    projection_matrix = perspective(45.0f, ratio, 0.001f, 10.0f);

    glViewport(0, 0, window_width, window_height);

    // when the window is resized, the framebuffer and the screenquad
    // should also be resized
    framebuffer.Cleanup();
    framebuffer.Init(window_width, window_height);
    screenquad.UpdateSize(window_width, window_height);
}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
       filter -= 0.25;
    }
    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        filter += 0.25f;
    }
}

// transforms glfw screen coordinates into normalized OpenGL coordinates.
vec2 TransformScreenCoords(GLFWwindow* window, int x, int y) {
    // the framebuffer and the window doesn't necessarily have the same size
    // i.e. hidpi screens. so we need to get the correct one
    int width;
    int height;
    glfwGetWindowSize(window, &width, &height);
    return vec2(2.0f * (float)x / width - 1.0f,
                1.0f - 2.0f * (float)y / height);
}

void MouseButton(GLFWwindow* window, int button, int action, int mod) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double x_i, y_i;
        glfwGetCursorPos(window, &x_i, &y_i);
        vec2 p = TransformScreenCoords(window, x_i, y_i);
        trackball.BeingDrag(p.x, p.y);
        old_trackball_matrix = trackball_matrix;
        // Store the current state of the model matrix.
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
        double x_i, y_i;
        glfwGetCursorPos(window, &x_i, &y_i);
        zoom = TransformScreenCoords(window, x_i, y_i)[1];
    }
}

void MousePos(GLFWwindow* window, double x, double y) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        vec2 p = TransformScreenCoords(window, x, y);
        // TODO 3: Calculate 'trackball_matrix' given the return value of
        // trackball.Drag(...) and the value stored in 'old_trackball_matrix'.
        // See also the mouse_button(...) function.
        trackball_matrix = trackball.Drag(p.x,p.y)*old_trackball_matrix ;
    }

    // zoom
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        // TODO 4: Implement zooming. When the right mouse button is pressed,
        // moving the mouse cursor up and down (along the screen's y axis)
        // should zoom out and it. For that you have to update the current
        // 'view_matrix' with a translation along the z axis.
        // view_matrix = ...
        const float constant_factor = 4.0f;
        vec2 p = TransformScreenCoords(window, x, y);
        vec3 translate_vector = vec3(0.0,0.0,view_matrix[3][2]+((p[1]-zoom)*constant_factor));
        view_matrix = translate(mat4(1.0f),translate_vector);
        zoom = p[1];
        //I'd find it more intuitive to zoom out and in by scaling the object, why don't we do that?

    }
}

int main(int argc, char *argv[]) {
    // GLFW Initialization
    if(!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return EXIT_FAILURE;
    }

    glfwSetErrorCallback(ErrorCallback);

    // hint GLFW that we would like an OpenGL 3 context (at least)
    // http://www.glfw.org/faq.html#how-do-i-create-an-opengl-30-context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // attempt to open the window: fails if required version unavailable
    // note some Intel GPUs do not support OpenGL 3.2
    // note update the driver of your graphic card
    GLFWwindow* window = glfwCreateWindow(window_width, window_height,
                                          "framebuffer", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // makes the OpenGL context of window current on the calling thread
    glfwMakeContextCurrent(window);

    // set the callback for escape key
    glfwSetKeyCallback(window, KeyCallback);

    // set the framebuffer resize callback
    glfwSetFramebufferSizeCallback(window, ResizeCallback);

    // set the mouse press and position callback
    glfwSetMouseButtonCallback(window, MouseButton);
    glfwSetCursorPosCallback(window, MousePos);

    // GLEW Initialization (must have a context)
    // https://www.opengl.org/wiki/OpenGL_Loading_Library
    glewExperimental = GL_TRUE; // fixes glew error (see above link)
    if(glewInit() != GLEW_NO_ERROR) {
        fprintf( stderr, "Failed to initialize GLEW\n");
        return EXIT_FAILURE;
    }

    cout << "OpenGL" << glGetString(GL_VERSION) << endl;

    // initialize our OpenGL program
    Init(window);

    // render loop
    while(!glfwWindowShouldClose(window)){
        Display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // cleanup
    quad.Cleanup();
    cube.Cleanup();
    framebuffer.Cleanup();
    screenquad.Cleanup();

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
