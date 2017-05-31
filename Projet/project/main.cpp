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
#include "clouds/cloud.h"

Skybox skybox;

Quad quad;
Water water;
Cloud clouds;

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
vec3 cam_pos(0.0f, 0.0f, 5.0f);
//vec3 cam_pos_mir(2.0f, 2.0f, -2.9f);
vec3 cam_look(0.0f, 1.0f, 5.0f);
vec3 cam_up(0.0f, 0.0f, 1.0f);
bool cam_forward = false;
bool cam_backward = false;
bool cam_left = false;
bool cam_right = false;
bool cam_upward = false;
bool cam_downward = false;
vec2 mouse_anchor(0.0f);
int terrainMode = 1;

float filter = 2.0f;
uint8 camera_mode = 0;
float bezierT = 0.0f;
int bezierCurve = 0;
vec3* bezierPoints;
float speed = 0.0;

vec3 Bezier(float t, vec3 p1, vec3 p2, vec3 p3, vec3 p4) {
    float tt = t*t;
    float ttt = tt*t;
    float u = 1 - t;
    float uu = u*u;
    float uuu = uu*u;
    vec3 res = (uuu*p1) + (3*uu*t*p2) + (3*u*tt*p3) + (ttt*p4);
    return res;
}

vec3* AframeConstruction(vec3 start, vec3 left, vec3 end) {
    vec3 leftBranch = left-start;
    vec3 top = left+leftBranch;
    vec3 rightBranch = (top-end)/2.0f;
    vec3 right = end+rightBranch;
    vec3 mid = (left+right)/2.0f;
    vec3* res = (vec3*)calloc(3,sizeof(vec3));
    res[0] = mid;
    res[1] = right;
    res[2] = end;
    return &res[0];
}

void draw_perlin(){
    //Perlin Noise
    framebuffer.Clear();
    framebuffer.Bind();
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        screenquad.Draw(terrainMode);
    }
    framebuffer.Unbind();
}

void Init(GLFWwindow* window) {
    glClearColor(1.0, 1.0, 1.0 /*white*/, 1.0 /*solid*/);
    glEnable(GL_DEPTH_TEST);

    vec3* bezierNexts = (vec3*)calloc(20,sizeof(vec3));
    bezierNexts[0] = vec3(10.0,5.0,6.0);
    bezierNexts[1] = vec3(0.0,2.0,6.0);
    bezierNexts[2] = vec3(-10.0,0.0,6.0);
    bezierNexts[3] = vec3(-7.0,-4.0,7.0);
    bezierNexts[4] = vec3(-5.0,-8.0,1.5);
    bezierNexts[5] = vec3(-3.0,-6.0,2.0);
    bezierNexts[6] = vec3(-3.0,-4.0,3.0);
    bezierNexts[7] = vec3(-5.0,-1.0,4.0);
    bezierNexts[8] = vec3(-4.0,5.0,5.0);
    bezierNexts[9] = vec3(-7.0,9.0,6.0);
    bezierNexts[10] = vec3(-10.0,11.0,7.0);
    bezierNexts[11] = vec3(-8.0,12.0,6.0);
    bezierNexts[12] = vec3(-5.0,12.0,3.0);
    bezierNexts[13] = vec3(0.0,11.0,2.0);
    bezierNexts[14] = vec3(6.0,10.0,3.0);
    bezierNexts[15] = vec3(10.0,7.0,4.0);
    bezierNexts[16] = vec3(11.0,3.0,5.0);
    bezierNexts[17] = vec3(11.0,-1.0,2.0);
    bezierNexts[18] = vec3(7.0,-6.0,4.0);
    bezierNexts[19] = vec3(4.0,-8.0,6.0);


    bezierPoints = (vec3*)calloc(60,sizeof(vec3)); //0: start/end, 1: control1, 2: control2
    bezierPoints[0] = vec3(0.0f,0.0f,6.0f);
    bezierPoints[1] = vec3(0.0f,1.0f,6.0f);
    bezierPoints[2] = vec3(1.0f,1.0f,6.0f);

    for(int i = 3; i < 60; i+= 3) {
        //vec3 next = vec3(7*sin(i*i/8), 5*cos(i/4.0), ((3*i)%7)+1.5);
        vec3 next = bezierNexts[i/3];
        vec3* temp = AframeConstruction(bezierPoints[i-2],bezierPoints[i-1],next);
        bezierPoints[i] = temp[0];
        bezierPoints[i+1] = temp[1];
        bezierPoints[i+2] = temp[2];
        free(temp);
    }

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

    quad.Init(framebuffer_texture_id);
    clouds.Init(framebuffer_texture_id);
    water.Init(waterbuffer_texture_id);
    skybox.Init();
    draw_perlin();
}

void RecomputeReflectionViewMat() {
    r_plane = initial_rplane;
    vec3 r_normal = normalize(vec3(r_plane.x,r_plane.y,r_plane.z));
    //cam_pos_mir = cam_pos - 2*dot(cam_pos,r_normal)*r_normal;
    reflect_mat = IDENTITY_MATRIX;
    reflect_mat[2][2] = -1.0;
}



void UpdateCamera() {
    /** Update camera position according to the pressed button*/
    vec3 translate_vector;
    vec3 previous_pos = cam_pos;
    float* height;
    if(cam_forward || cam_backward || cam_left || cam_right || cam_downward || cam_upward){
        speed = speed >= 0.1? 0.1: speed+0.001;
    } else {
        speed = 0;
    }
    switch (camera_mode) {
    case 0: //free cam
        if(cam_forward) {
            translate_vector = normalize(cam_look-cam_pos)*speed*2.0f;
            cam_look = cam_look+translate_vector;
            cam_pos = cam_pos+translate_vector;
        }
        if(cam_backward) {
            translate_vector = normalize(cam_look-cam_pos)*speed*2.0f;
            cam_look = cam_look-translate_vector;
            cam_pos = cam_pos-translate_vector;
        }
        if(cam_left) {
            translate_vector = normalize(cross(cam_look-cam_pos,cam_up))*speed*2.0f;
            cam_look = cam_look-translate_vector;
            cam_pos = cam_pos-translate_vector;
        }
        if(cam_right) {
            translate_vector = normalize(cross(cam_look-cam_pos,cam_up))*speed*2.0f;
            cam_look = cam_look+translate_vector;
            cam_pos = cam_pos+translate_vector;
        }
        break;
    case 1: //ground cam
        if(cam_forward) {
            height = (float*)calloc(1,sizeof(float));
            translate_vector = normalize(vec3(cam_look.x,cam_look.y,1)-vec3(cam_pos.x,cam_pos.y,1))*speed*0.5f;
            cam_pos = cam_pos+translate_vector;
            vec2 position = vec2(((cam_pos.x/10.0f)+1.0f)*framebuffer.width(),((cam_pos.y/10.0f)+1.0f)*framebuffer.height())*0.5f;
            glReadPixels(position.x , position.y , 1 , 1 , GL_RED , GL_FLOAT , height);
            cam_pos[2] = glm::max((*height +0.7f),0.2f);
            //std::cout << "Camera position: "<< cam_pos[0] << " " << cam_pos[1] << " " << cam_pos[2] << std::endl;
            translate_vector = cam_pos-previous_pos;
            cam_look = cam_look+translate_vector;
            free(height);
        }
        if(cam_backward) {
            height = (float*)calloc(1,sizeof(float));
            previous_pos = cam_pos;
            translate_vector = -normalize(vec3(cam_look.x,cam_look.y,1)-vec3(cam_pos.x,cam_pos.y,1))*speed*0.5f;
            cam_pos = cam_pos+translate_vector;
            vec2 position = vec2(((cam_pos.x/10.0f)+1.0f)*framebuffer.width(),((cam_pos.y/10.0f)+1.0f)*framebuffer.height())*0.5f;
            glReadPixels(position.x , position.y , 1 , 1 , GL_RED , GL_FLOAT , height);
            cam_pos[2] = glm::max((*height +0.7f),0.2f);
            //std::cout << "Camera position: "<< cam_pos[0] << " " << cam_pos[1] << " " << cam_pos[2] << std::endl;
            translate_vector = cam_pos-previous_pos;
            cam_look = cam_look+translate_vector;
            free(height);
        }
        if(cam_left) {
            height = (float*)calloc(1,sizeof(float));
            previous_pos = cam_pos;
            translate_vector = -normalize(cross(vec3(cam_look.x,cam_look.y,1)-vec3(cam_pos.x,cam_pos.y,1),cam_up))*speed*0.5f;
            cam_pos = cam_pos+translate_vector;
            vec2 position = vec2(((cam_pos.x/10.0f)+1.0f)*framebuffer.width(),((cam_pos.y/10.0f)+1.0f)*framebuffer.height())*0.5f;
            glReadPixels(position.x , position.y , 1 , 1 , GL_RED , GL_FLOAT , height);
            cam_pos[2] = glm::max((*height +0.7f),0.2f);
            //std::cout << "Camera position: "<< cam_pos[0] << " " << cam_pos[1] << " " << cam_pos[2] << std::endl;
            translate_vector = cam_pos-previous_pos;
            cam_look = cam_look+translate_vector;
            free(height);
        }
        if(cam_right) {
            height = (float*)calloc(1,sizeof(float));
            previous_pos = cam_pos;
            translate_vector = normalize(cross(vec3(cam_look.x,cam_look.y,1)-vec3(cam_pos.x,cam_pos.y,1),cam_up))*speed*0.5f;
            cam_pos = cam_pos+translate_vector;
            vec2 position = vec2(((cam_pos.x/10.0f)+1.0f)*framebuffer.width(),((cam_pos.y/10.0f)+1.0f)*framebuffer.height())*0.5f;
            glReadPixels(position.x , position.y , 1 , 1 , GL_RED , GL_FLOAT , height);
            cam_pos[2] = glm::max((*height +0.7f),0.2f);
            //std::cout << "Camera position: "<< cam_pos[0] << " " << cam_pos[1] << " " << cam_pos[2] << std::endl;
            translate_vector = cam_pos-previous_pos;
            cam_look = cam_look+translate_vector;
            free(height);
        }
        if(!cam_right && !cam_backward && !cam_forward && !cam_left) {
            height = (float*)calloc(1,sizeof(float));
            previous_pos = cam_pos;
            vec2 position = vec2(((cam_pos.x/10.0f)+1.0f)*framebuffer.width(),((cam_pos.y/10.0f)+1.0f)*framebuffer.height())*0.5f;
            glReadPixels(position.x , position.y , 1 , 1 , GL_RED , GL_FLOAT , height);
            cam_pos[2] = glm::max((*height +0.7f),0.2f);
            //std::cout << "Camera position: "<< cam_pos[0] << " " << cam_pos[1] << " " << cam_pos[2] << std::endl;
            translate_vector = cam_pos-previous_pos;
            cam_look = cam_look+translate_vector;
            free(height);
        }
        break;
    case 2:
        if(cam_forward) {
            translate_vector = normalize(cam_look-cam_pos)*speed*2.0f;
            cam_look = cam_look+translate_vector;
            cam_pos = cam_pos+translate_vector;
        }
        if(cam_backward) {
            translate_vector = normalize(cam_look-cam_pos)*speed*2.0f;
            cam_look = cam_look-translate_vector;
            cam_pos = cam_pos-translate_vector;
        }
        if(cam_left) {
            translate_vector = normalize(cross(cam_look-cam_pos,cam_up))*speed*2.0f;
            cam_look = cam_look-translate_vector;
        }
        if(cam_right) {
            translate_vector = normalize(cross(cam_look-cam_pos,cam_up))*speed*2.0f;
            cam_look = cam_look+translate_vector;
        }
        if(cam_upward) {
            previous_pos = cam_look;
            translate_vector = normalize(cam_up)*speed*2.0f;
            cam_look = cam_look+translate_vector;
            if(normalize(cam_look-cam_pos)[2] > 0.95) {
                cam_look = previous_pos;
                speed = 0;
            }
        }
        if(cam_downward) {
            previous_pos = cam_look;
            translate_vector = -normalize(cam_up)*speed*2.0f;
            cam_look = cam_look+translate_vector;
            if(normalize(cam_look-cam_pos)[2] < -0.95) {
                cam_look = previous_pos;
                speed = 0;
            }
        }
        break;
    case 3:
        bezierT += 0.01;
        if(bezierT >= 1.0f) {
            bezierCurve++;
            bezierT = 0.0;
            //std::cout << "Change of curve" << std::endl;
        }
        if(bezierCurve < 19){
            cam_pos = Bezier(bezierT,bezierPoints[bezierCurve*3],bezierPoints[(bezierCurve*3)+1],bezierPoints[(bezierCurve*3)+2],bezierPoints[(bezierCurve*3)+3]);
            //cam_look = Bezier(bezierT,bezierPoints[(bezierCurve+1)*3],bezierPoints[(bezierCurve+1)+1],bezierPoints[(bezierCurve+1)+2],bezierPoints[(bezierCurve+1)+3]);
            //std::cout << cam_pos[0] << " " << cam_pos[1] << " " << cam_pos[2] <<std::endl;
        }
        break;
    default:
        break;
    }
    view_matrix = lookAt(cam_pos, cam_look, cam_up);

}
void Display() {
    framebuffer.Bind();
    {
        UpdateCamera();
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

    skybox.Draw(projection_matrix * view_matrix );
    quad.Draw(IDENTITY_MATRIX, view_matrix, projection_matrix, 0); //I is used as the model matrix
    water.Draw(IDENTITY_MATRIX, view_matrix, projection_matrix, 0);
    clouds.Draw(IDENTITY_MATRIX, view_matrix, projection_matrix);
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

    waterbuffer.Cleanup();
    waterbuffer.Init(window_width, window_height);
}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    switch(key){
        case GLFW_KEY_ESCAPE :
            if(action == GLFW_PRESS) {glfwSetWindowShouldClose(window, GL_TRUE);}

        /*########
         * Camera
         * #######*/
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
        case GLFW_KEY_Q: //up
            if(action == GLFW_PRESS) {
                cam_upward = true;
            } else if(action == GLFW_RELEASE) {
            cam_upward = false;
            }
            break;
        case GLFW_KEY_E: //down
            if(action == GLFW_PRESS) {
                cam_downward = true;
            } else if(action == GLFW_RELEASE) {
                cam_downward = false;
            }
            break;

        /*########
         * Terrain mode
         * #######*/
        case GLFW_KEY_F1: //Terrain 1
            if(action == GLFW_PRESS){
                terrainMode = 1;
            }
            break;
        case GLFW_KEY_F2: //Terrain 2
            if(action == GLFW_PRESS){
                terrainMode = 2;
            }
            break;
        case GLFW_KEY_F3: //Terrain 3
            if(action == GLFW_PRESS){
                terrainMode = 3;
            }
            break;
        case GLFW_KEY_F4: //Terrain 4
            if(action == GLFW_PRESS){
                terrainMode = 4;
            }
            break;
        case GLFW_KEY_F5: //Terrain 5
            if(action == GLFW_PRESS){
                terrainMode = 5;
            }
            break;
        case GLFW_KEY_F6: //Terrain 6
            if(action == GLFW_PRESS){
                terrainMode = 6;
            }
            break;

        /*########
         * Misc
         * #######*/
        case GLFW_KEY_R: //Re-randomize the noise
            if(action == GLFW_PRESS){
                screenquad.Cleanup();
                screenquad.Init(window_width, window_height);
                draw_perlin();
            }
            break;
    case GLFW_KEY_1 :
        if(action == GLFW_PRESS) {
            camera_mode = 0;
            std::cout << "Switched to free camera mode" << std::endl;
            std::cout << "Controls: right-click and move mouse to look around, wasd to move the camera" << std::endl;
            bezierT = 0.0;
            bezierCurve = 0;
            //trackball_matrix = IDENTITY_MATRIX;
        }
        break;
    case GLFW_KEY_0:
        if(action == GLFW_PRESS) {
            camera_mode = 1;
            std::cout << "Switched to ground camera mode" << std::endl;
            std::cout << "Controls: right-click and move mouse to look around, wasd to move the camera" << std::endl;
        }
        break;
    case GLFW_KEY_2:
        if(action == GLFW_PRESS) {
            camera_mode = 2;
            std::cout << "Switched to buttons-only camera mode" << std::endl;
            std::cout << "Controls: wasd to move the camera, q to look up and e to look down" << std::endl;
        }
        break;
    case GLFW_KEY_3:
        if(action == GLFW_PRESS) {
            camera_mode = 3;
            std::cout << "Switched to automatic camera mode" << std::endl;
            std::cout << "Controls: none" << std::endl;
            cam_look = vec3(0.0,0.0,0.0);
        }
        break;
    case GLFW_KEY_4:
        if(action == GLFW_PRESS) {
            camera_mode = 4;
            std::cout << "Switched to trackball mode" << std::endl;
            std::cout << "Controls: left click and drag to move the trackball" << std::endl;
            //trackball_matrix = IDENTITY_MATRIX;
        }
        break;
    }
    if((key == GLFW_KEY_F1 || key == GLFW_KEY_F2 || key == GLFW_KEY_F3 ||
        key == GLFW_KEY_F4 || key == GLFW_KEY_F5 || key == GLFW_KEY_F6) &&
        action == GLFW_PRESS) {
        draw_perlin();
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

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS && (camera_mode == 0 || camera_mode == 1)) {
        double x_i, y_i;
        glfwGetCursorPos(window, &x_i, &y_i);
        mouse_anchor = TransformScreenCoords(window, x_i, y_i);
    }
}

void MousePos(GLFWwindow* window, double x, double y) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && (camera_mode == 0 || camera_mode == 1)) {

        //std::cout << mouse_anchor.y << std::endl;

            vec2 mouse_dif = mouse_anchor - TransformScreenCoords(window, x, y);
            if(length(mouse_dif) > 0.0) {
                mouse_anchor = TransformScreenCoords(window, x, y);
                vec3 old_cam_look = cam_look;
                vec3 look_direction = cam_look-cam_pos;
                vec3 rotation_axis = cross(normalize(cam_up)*mouse_dif.y,look_direction) + cross(normalize(cross(look_direction,cam_up))*mouse_dif.x,look_direction);
                mat4 rotation = rotate(mat4(1.0f),0.04f,rotation_axis);
                vec3 new_cam_look = glm::vec3(rotation * glm::vec4(cam_look-cam_pos, 0.0));
                vec3 new_cam_up = vec3(0.0,0.0,1.0);//glm::vec3(rotation * glm::vec4(cam_up, 0.0));
                cam_up = normalize(new_cam_up);
                cam_look = cam_pos+(normalize(new_cam_look)*2.0f);
                if(abs(normalize(cam_look-cam_pos).z) > cam_up.z-0.05) cam_look = old_cam_look;
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
