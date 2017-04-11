#pragma once
#include "icg_helper.h"


using namespace glm;

class Trackball {
public:
    Trackball() : radius_(1.0f) {}

    // this function is called when the user presses the left mouse button down.
    // x, and y are in [-1, 1]. (-1, -1) is the bottom left corner while (1, 1)
    // is the top right corner.
    void BeingDrag(float x, float y) {
      anchor_pos_ = vec3(x, y, 0.0f);
      ProjectOntoSurface(anchor_pos_);
    }

    // this function is called while the user moves the curser around while the
    // left mouse button is still pressed.
    // x, and y are in [-1, 1]. (-1, -1) is the bottom left corner while (1, 1)
    // is the top right corner.
    // returns the rotation of the trackball in matrix form.
    mat4 Drag(float x, float y) {
      vec3 current_pos = vec3(x, y, 0.0f);
      ProjectOntoSurface(current_pos);

      mat4 rotation = IDENTITY_MATRIX;
      vec3 from = normalize(anchor_pos_);
      vec3 to = normalize(current_pos);
      vec3 crossP = cross(from,to);
      float sin = length(crossP);
      float cos = dot(from,to);
      mat3 smatrix = mat3(vec3(0,crossP[2],-crossP[1]),vec3(-crossP[2],0,crossP[0]),vec3(crossP[1],-crossP[0],0));
      mat3 res = mat3(1.0f)+smatrix+(smatrix*smatrix)*(1/(1+cos));
      rotation = mat4(vec4(res[0],0.0f),vec4(res[1],0.0),vec4(res[2],0.0),vec4(0.0f,0.0f,0.0f,1.0f));
      // TODO 3: Calculate the rotation given the projections of the anocher
      // point and the current position. The rotation axis is given by the cross
      // product of the two projected points, and the angle between them can be
      // used as the magnitude of the rotation.
      // you might want to scale the rotation magnitude by a scalar factor.
      // p.s. No need for using complicated quaternions as suggested inthe wiki
      // article.
      return rotation;
    }

private:
    // projects the point p (whose z coordiante is still empty/zero) onto the
    // trackball surface. If the position at the mouse cursor is outside the
    // trackball, use a hyberbolic sheet as explained in:
    // https://www.opengl.org/wiki/Object_Mouse_Trackball.
    // The trackball radius is given by 'radius_'.
    void ProjectOntoSurface(vec3& p) const {
      // TODO 2: Implement this function. Read above link for details.
        float x = p[0];
        float y = p[1];
        if((x*x) + (y*y) <= radius_*radius_/2) {
        //if(sqrt(glm::max(((radius_*radius_)-(x*x)-(y*y)),0.0f)) <= (radius_*radius_)/(2*sqrt((x*x)+(y*y)))) {
            p[2] = sqrt((radius_*radius_)-(x*x)-(y*y));
        } else {
            p[2] = (radius_*radius_)/(2*sqrt((x*x)+(y*y)));
        }
    }

    float radius_;
    vec3 anchor_pos_;
    mat4 rotation_;
};
