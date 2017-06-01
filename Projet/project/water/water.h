#pragma once
#include "icg_helper.h"
#include "glm/gtc/type_ptr.hpp"

class Water {

private:
    GLuint vertex_array_id_;        // vertex array object
    GLuint program_id_;             // GLSL shader program ID
    GLuint vertex_buffer_object_;   // memory buffer
    GLuint vertex_buffer_object_position_;  // memory buffer for positions
    GLuint vertex_buffer_object_index_;     // memory buffer for indices
    GLuint texture_id_;             // texture ID
    GLuint texture_map_;            //normalMap for water
    GLuint texture_dudv_;            //dudvMap for water
    GLuint num_indices_;            // number of vertices to render
    float move_factor = 0.003;

public:

    void Init(GLuint texture) {
        // compile the shaders
        program_id_ = icg_helper::LoadShaders("water_vshader.glsl",
                                              "water_fshader.glsl");
        if(!program_id_) {
            exit(EXIT_FAILURE);
        }
        glUseProgram(program_id_);

        // light?
        glm::vec3 La = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 Ld = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 Ls = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 light_pos = glm::vec3(5.0f, 5.0f, 2.0f);
        GLuint light_pos_id = glGetUniformLocation(program_id_, "light_pos");
        GLuint La_id = glGetUniformLocation(program_id_, "La");
        GLuint Ld_id = glGetUniformLocation(program_id_, "Ld");
        GLuint Ls_id = glGetUniformLocation(program_id_, "Ls");
        glUniform3fv(light_pos_id, ONE, glm::value_ptr(light_pos));
        glUniform3fv(La_id, ONE, glm::value_ptr(La));
        glUniform3fv(Ld_id, ONE, glm::value_ptr(Ld));
        glUniform3fv(Ls_id, ONE, glm::value_ptr(Ls));

        // alpha for shading
        float alpha = 60.0f;
        GLuint alpha_id = glGetUniformLocation(program_id_, "alpha");
        glUniform1f(alpha_id, alpha);



        // vertex one vertex Array
        glGenVertexArrays(1, &vertex_array_id_);
        glBindVertexArray(vertex_array_id_);

        // vertex coordinates and indices
        {
            std::vector<GLfloat> vertices;
            std::vector<GLuint> indices;

            int grid_dim = 512;
            float grid_size = 20.0f;

            GLuint grim_dim_id = glGetUniformLocation(program_id_, "triangles_number");
            glUniform1i(grim_dim_id, grid_dim);

            float grid_start = -grid_size/2.0;
            for(int i = 0; i < grid_dim; i++) {
                for(int j = 0; j < grid_dim; j++) {
                    vertices.push_back(grid_start+(grid_size/(float)grid_dim)*j);
                    vertices.push_back(grid_start+(grid_size/(float)grid_dim)*i);
                }
            }
            for(int i = 0; i < grid_dim-1; i++) {
                if(i%2 == 0) { //forward loop
                    for(int j = 0; j < grid_dim-1; j++) {
                        indices.push_back(grid_dim*i+j);
                        indices.push_back(grid_dim*(1+i)+j);
                        indices.push_back(grid_dim*i+j+1);
                        indices.push_back(grid_dim*(1+i)+1+j);
                    }
                } else { //backward loop
                    for(int j = grid_dim-2; j >= 0; j--) {
                        indices.push_back(grid_dim*(1+i)+1+j);
                        indices.push_back(grid_dim*i+j+1);
                        indices.push_back(grid_dim*(1+i)+j);
                        indices.push_back(grid_dim*i+j);
                    }
                }
            }



            num_indices_ = indices.size();

            // position buffer
            glGenBuffers(1, &vertex_buffer_object_position_);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_position_);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat),
                         &vertices[0], GL_STATIC_DRAW);

            // vertex indices
            glGenBuffers(1, &vertex_buffer_object_index_);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_object_index_);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
                         &indices[0], GL_STATIC_DRAW);

            // position shader attribute
            GLuint loc_position = glGetAttribLocation(program_id_, "position");
            glEnableVertexAttribArray(loc_position);
            glVertexAttribPointer(loc_position, 2, GL_FLOAT, DONT_NORMALIZE,
                                  ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }


        // load/Assign first texture
        this->texture_id_ = texture;
        glBindTexture(GL_TEXTURE_2D, texture_id_);
        GLuint tex_id = glGetUniformLocation(program_id_, "tex");
        glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);

        {
            int width;
            int height;
            int nb_component;
            string filename = "normalMap.png";
            // set stb_image to have the same coordinates as OpenGL
            stbi_set_flip_vertically_on_load(1);
            unsigned char* image = stbi_load(filename.c_str(), &width,
                                             &height, &nb_component, 0);

            if(image == nullptr) {
                throw(string("Failed to load texture"));
            }

            glGenTextures(1, &texture_map_);
            glBindTexture(GL_TEXTURE_2D, texture_map_);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            if(nb_component == 3) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                             GL_RGB, GL_UNSIGNED_BYTE, image);
            } else if(nb_component == 4) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                             GL_RGBA, GL_UNSIGNED_BYTE, image);
            }

            GLuint tex_id = glGetUniformLocation(program_id_, "normalMap");
            glUniform1i(tex_id, 1 /*GL_TEXTURE5*/);

            // cleanup
            glBindTexture(GL_TEXTURE_2D, 0);
            stbi_image_free(image);
        }

        {
            int width;
            int height;
            int nb_component;
            string filename = "waterDUDV.png";
            // set stb_image to have the same coordinates as OpenGL
            stbi_set_flip_vertically_on_load(1);
            unsigned char* image = stbi_load(filename.c_str(), &width,
                                             &height, &nb_component, 0);

            if(image == nullptr) {
                throw(string("Failed to load texture"));
            }

            glGenTextures(1, &texture_dudv_);
            glBindTexture(GL_TEXTURE_2D, texture_dudv_);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            if(nb_component == 3) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                             GL_RGB, GL_UNSIGNED_BYTE, image);
            } else if(nb_component == 4) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                             GL_RGBA, GL_UNSIGNED_BYTE, image);
            }

            GLuint tex_id = glGetUniformLocation(program_id_, "dudvMap");
            glUniform1i(tex_id, 2 /*GL_TEXTURE6*/);

            // cleanup
            glBindTexture(GL_TEXTURE_2D, 0);
            stbi_image_free(image);
        }


        // to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void Cleanup() {
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &vertex_buffer_object_);
        glDeleteBuffers(1, &vertex_buffer_object_position_);
        glDeleteBuffers(1, &vertex_buffer_object_index_);
        glDeleteProgram(program_id_);
        glDeleteVertexArrays(1, &vertex_array_id_);
        glDeleteTextures(1, &texture_id_);
        glDeleteTextures(1, &texture_map_);
        glDeleteTextures(1, &texture_dudv_);
    }

    void Draw(const glm::mat4 &model,
              const glm::mat4 &view,
              const glm::mat4 &projection,
              const int discard) {
        glUseProgram(program_id_);
        glBindVertexArray(vertex_array_id_);

        // bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id_);
        glActiveTexture(GL_TEXTURE0+1);
        glBindTexture(GL_TEXTURE_2D, texture_map_);
        glActiveTexture(GL_TEXTURE0+2);
        glBindTexture(GL_TEXTURE_2D, texture_dudv_);

        // 0: False, 1: True
        GLuint discard_loc = glGetUniformLocation(program_id_,"discard_pix");
        glUniform1i(discard_loc,discard);

        const float time = glfwGetTime();
        glUniform1f(glGetUniformLocation(program_id_, "time"), time);
        float speed = time * move_factor;
        glUniform1f(glGetUniformLocation(program_id_, "speed"), speed);

        // setup MVP
        glm::mat4 MVP = projection*view*model;
        GLuint MVP_id = glGetUniformLocation(program_id_, "MVP");
        glUniformMatrix4fv(MVP_id, 1, GL_FALSE, value_ptr(MVP));
        GLuint model_id = glGetUniformLocation(program_id_,"model");
        glUniformMatrix4fv(model_id, ONE, DONT_TRANSPOSE, glm::value_ptr(model));
        GLuint view_id = glGetUniformLocation(program_id_,"view");
        glUniformMatrix4fv(view_id, ONE, DONT_TRANSPOSE, glm::value_ptr(view));
        GLuint projection_id = glGetUniformLocation(program_id_,"projection");
        glUniformMatrix4fv(projection_id, ONE, DONT_TRANSPOSE,glm::value_ptr(projection));

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // draw
        //glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDrawElements(GL_TRIANGLE_STRIP, num_indices_, GL_UNSIGNED_INT, 0);

        glDisable(GL_BLEND);
        glBindVertexArray(0);
        glUseProgram(0);
    }
};
