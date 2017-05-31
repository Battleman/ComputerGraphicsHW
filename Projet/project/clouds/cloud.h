#pragma once
#include "icg_helper.h"
#include "glm/gtc/type_ptr.hpp"

class Cloud {

    private:
        GLuint vertex_array_id_;        // vertex array object
        GLuint program_id_;             // GLSL shader program ID
        GLuint vertex_buffer_object_;   // memory buffer
        GLuint vertex_buffer_object_position_;  // memory buffer for positions
        GLuint vertex_buffer_object_index_;     // memory buffer for indices
        GLuint texture_id_;             // texture ID
        GLuint num_indices_;            // number of vertices to render
        float move_factor = 0.006;


    public:

        void Init(GLuint texture) {
            // compile the shaders
            program_id_ = icg_helper::LoadShaders("cloud_vshader.glsl",
                                                  "cloud_fshader.glsl");
            if(!program_id_) {
                exit(EXIT_FAILURE);
            }
            glUseProgram(program_id_);

            // vertex one vertex Array
            glGenVertexArrays(1, &vertex_array_id_);
            glBindVertexArray(vertex_array_id_);

            // vertex coordinates and indices
            {
                std::vector<GLfloat> vertices;
                std::vector<GLuint> indices;

                int grid_dim = 1024;
                float grid_size = 30.0f;

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
        }

        void Cleanup() {
            glBindVertexArray(0);
            glUseProgram(0);
            glDeleteBuffers(1, &vertex_buffer_object_);
            glDeleteBuffers(1, &vertex_buffer_object_position_);
            glDeleteBuffers(1, &vertex_buffer_object_index_);
            glDeleteProgram(program_id_);
            glDeleteTextures(1, &texture_id_);
        }

        void Draw(const glm::mat4 &model,
                  const glm::mat4 &view,
                  const glm::mat4 &projection) {
            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            // bind textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_id_);

            const float time = glfwGetTime();
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

            glDrawElements(GL_TRIANGLE_STRIP, num_indices_, GL_UNSIGNED_INT, 0);

            glDisable(GL_BLEND);
            glBindVertexArray(0);
            glUseProgram(0);
        }
};
