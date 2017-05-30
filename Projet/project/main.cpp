// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"
#include "glm/gtc/matrix_transform.hpp"

#include "framebuffer.h"
#include "waterbuffer.h"

#include "skybox/skybox.h"
#include "screenquad/screenquad.h"
#include "quad/quad.h"
#include "water/water.h"

Skybox skybox;

Quad quad;
Water water;

int window_width = 800;
int window_height = 600;

FrameBuffer framebuffer;
WaterBuffer waterbuffer;
ScreenQuad screenquad;

using namespace glm;

mat4 projection_matrix;
mat4 view_matrix;
mat4 reflect_mat;

//double water_height = 0.0;
vec4 r_plane(0.0,0.0,1.0,0.0);
vec4 initial_rplane(0.0,0.0,1.0,0);
vec3 translate_vector_mir(0.0f, 0.0f, 4.0f);
vec3 cam_pos(5.0f, 2.0f, 10.0f);
vec3 cam_pos_mir(2.0f, 2.0f, -2.9f);
vec3 cam_look(0.0f, 0.0f, 0.0f);
vec3 cam_up(0.0f, 0.0f, 1.0f);
bool cam_forward = false;
bool cam_backward = false;
bool cam_left = false;
bool cam_right = false;
vec2 mouse_anchor(0.0f);

float filter = 2.0f;

void Init(GLFWwindow* window) {
    glClearColor(1.0, 1.0, 1.0 /*white*/, 1.0 /*solid*/);
    glEnable(GL_DEPTH_TEST);

    view_matrix = lookAt(cam_pos, cam_look, cam_up);
    //view_matrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, -4.0f));
    float ratio = window_width / (float) window_height;
    projection_matrix = perspective(45.0f, ratio, 0.001f, 80.0f);

    reflect_mat = mat4( vec4( 1.0-2*r_plane.x*r_plane.x, -2*r_plane.x*r_plane.y, -2*r_plane.x*r_plane.z, 0.0),
                        vec4( -2*r_plane.x*r_plane.y, 1.0-2*r_plane.y*r_plane.y, -2*r_plane.y*r_plane.z, 0.0),
                        vec4( -2*r_plane.x*r_plane.z, -2*r_plane.y*r_plane.z, 1.0-2*r_plane.z*r_plane.z, 0.0),
                        vec4( -2*r_plane.x*r_plane.w, -2*r_plane.y*r_plane.w, -2*r_plane.z*r_plane.w, 1.0));

    // on retina/hidpi displays, pixels != screen coordinates
    // this unsures that the framebuffer has the same size as the window
    glfwGetFramebufferSize(window, &window_width, &window_height);
    GLuint framebuffer_texture_id = framebuffer.Init(window_width, window_height, true);
    GLuint waterbuffer_texture_id = waterbuffer.Init(window_width, window_height, true);

    screenquad.Init(window_width, window_height);
    quad.Init(framebuffer_texture_id, 0.0);
    water.Init(waterbuffer_texture_id);
    skybox.Init();
}

void RecomputeReflectionViewMat() {
    r_plane = initial_rplane;
    vec3 r_normal = normalize(vec3(r_plane.x,r_plane.y,r_plane.z));
    cam_pos_mir = cam_pos - 2*dot(cam_pos,r_normal)*r_normal;
    reflect_mat = IDENTITY_MATRIX;
    reflect_mat[2][2] = -1.0;
}

void UpdateCamera() {
    /** Update camera position according to the pressed button*/
    vec3 translate_vector;
    if(cam_forward) {
        translate_vector = normalize(cam_look-cam_pos)/3.0f;
        cam_look = cam_look+translate_vector;
        cam_pos = cam_pos+translate_vector;
    }
    if(cam_backward) {
        translate_vector = normalize(cam_look-cam_pos)/3.0f;
        cam_look = cam_look-translate_vector;
        cam_pos = cam_pos-translate_vector;
    }
    if(cam_left) {
        translate_vector = normalize(cross(cam_look-cam_pos,cam_up))/3.0f;
        cam_look = cam_look-translate_vector;
        cam_pos = cam_pos-translate_vector;
    }
    if(cam_right) {
        translate_vector = normalize(cross(cam_look-cam_pos,cam_up))/3.0f;
        cam_look = cam_look+translate_vector;
        cam_pos = cam_pos+translate_vector;
    }
    view_matrix = lookAt(cam_pos, cam_look, cam_up);
}
void Display() {
    UpdateCamera();

    //Perlin Noise
    framebuffer.Clear();
    framebuffer.Bind();
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        screenquad.Draw();
    }
    framebuffer.Unbind();

    RecomputeReflectionViewMat();

    //Draw Reflection
    waterbuffer.Clear();
    waterbuffer.Bind();
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        quad.Draw(reflect_mat,view_matrix, projection_matrix,1);
        skybox.Draw(projection_matrix * view_matrix * reflect_mat);
    }
    waterbuffer.Unbind();

    //Draw terrain and water plane
    glViewport(0, 0, window_width, window_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    skybox.Draw(/*IDENTITY_MATRIX * */ projection_matrix * view_matrix );
    quad.Draw(IDENTITY_MATRIX, view_matrix, projection_matrix, 0); //I is used as the model matrix
    water.Draw(IDENTITY_MATRIX, view_matrix, projection_matrix, 0);
}




// gets called when the windows/framebuffer is resized.
void ResizeCallback(GLFWwindow* window, int width, int height) {
    window_width = width;
    window_height = height;

    float ratio = window_width / (float) window_height;
    projection_matrix = perspective(45.0f, ratio, 0.001f, 40.0f);

    glViewport(0, 0, window_width, window_height);

    //TODO : check with Boris, might solve resize issue
    reflect_mat = mat4( vec4( 1.0-2*r_plane.x*r_plane.x, -2*r_plane.x*r_plane.y, -2*r_plane.x*r_plane.z, 0.0),
                        vec4( -2*r_plane.x*r_plane.y, 1.0-2*r_plane.y*r_plane.y, -2*r_plane.y*r_plane.z, 0.0),
                        vec4( -2*r_plane.x*r_plane.z, -2*r_plane.y*r_plane.z, 1.0-2*r_plane.z*r_plane.z, 0.0),
                        vec4( -2*r_plane.x*r_plane.w, -2*r_plane.y*r_plane.w, -2*r_plane.z*r_plane.w, 1.0));

    // when the window is resized, the framebuffer and the screenquad
    // should also be resized
    framebuffer.Cleanup();
    framebuffer.Init(window_width, window_height, true);
    screenquad.UpdateSize(window_width, window_height);
}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    switch(key){
        case GLFW_KEY_ESCAPE :
            if(action == GLFW_PRESS) {glfwSetWindowShouldClose(window, GL_TRUE);}
        case GLFW_KEY_W: //forward
            if(action == GLFW_PRESS) {
                cam_forward = true;
            } else if(action == GLFW_RELEASE) {
                cam_forward = false;
            }
            break;
        case GLFW_KEY_S : //backward
            if(action == GLFW_PRESS) {
                cam_backward = true;
            } else if(action == GLFW_RELEASE) {
                cam_backward = false;
            }
            break;
        case GLFW_KEY_A : //left
            if(action == GLFW_PRESS) {
                cam_left = true;
            } else if(action == GLFW_RELEASE) {
                cam_left = false;
            }
            break;
        case GLFW_KEY_D: //right
            if(action == GLFW_PRESS) {
                cam_right = true;
            } else if(action == GLFW_RELEASE) {
                cam_right = false;
            }
            break;
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
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        double x_i, y_i;
        glfwGetCursorPos(window, &x_i, &y_i);
        mouse_anchor = TransformScreenCoords(window, x_i, y_i);
    }
}

void MousePos(GLFWwindow* window, double x, double y) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        /*Update camera position, act as an "FPS" cam*/
        vec2 mouse_dif = mouse_anchor - TransformScreenCoords(window, x, y);
        if(length(mouse_dif) > 0.0) {
            mouse_anchor = TransformScreenCoords(window, x, y);
            vec3 old_cam_look = cam_look; //backup current cam_look
            vec3 look_direction = cam_look-cam_pos;
            vec3 rotation_axis = cross(normalize(cam_up)*mouse_dif.y,look_direction) +
                                 cross(normalize(cross(look_direction,cam_up)) * mouse_dif.x,look_direction);
            mat4 rotation = rotate(IDENTITY_MATRIX,0.04f,rotation_axis);
            vec3 new_cam_look = glm::vec3(rotation * glm::vec4(cam_look-cam_pos, 0.0));
            vec3 new_cam_up = vec3(0.0,0.0,1.0);
            cam_up = normalize(new_cam_up);
            cam_look = cam_pos+(normalize(new_cam_look)*2.0f);
            if(abs(normalize(cam_look-cam_pos).z) > cam_up.z-0.05){
                /*If fail, restore previous cam_look*/
                cam_look = old_cam_look;
            }
        }
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
    skybox.Cleanup();
    framebuffer.Cleanup();
    screenquad.Cleanup();
    water.Cleanup();

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
