#pragma once
#include "icg_helper.h"
#include "glm/gtc/type_ptr.hpp"

class Quad {

    private:
        GLuint vertex_array_id_;        // vertex array object
        GLuint program_id_;             // GLSL shader program ID
        GLuint vertex_buffer_object_;   // memory buffer
        GLuint vertex_buffer_object_position_;  // memory buffer for positions
        GLuint vertex_buffer_object_index_;     // memory buffer for indices
        GLuint texture_id_;             // texture ID
        float is_water_;
        GLuint image_texture_id_[3];    // image texture ID
        GLuint num_indices_;            // number of vertices to render

    public:

        void Init(GLuint texture, float is_water) {
            // compile the shaders
            program_id_ = icg_helper::LoadShaders("quad_vshader.glsl",
                                                  "quad_fshader.glsl");
            if(!program_id_) {
                exit(EXIT_FAILURE);
            }
            glUseProgram(program_id_);

            // light?
            glm::vec3 La = glm::vec3(1.0f, 1.0f, 1.0f);
            glm::vec3 Ld = glm::vec3(1.0f, 1.0f, 1.0f);
            glm::vec3 Ls = glm::vec3(1.0f, 1.0f, 1.0f);
            glm::vec3 light_pos = glm::vec3(0.0f, 0.0f, 4.0f);
            GLuint light_pos_id = glGetUniformLocation(program_id_, "light_pos");
            GLuint La_id = glGetUniformLocation(program_id_, "La");
            GLuint Ld_id = glGetUniformLocation(program_id_, "Ld");
            GLuint Ls_id = glGetUniformLocation(program_id_, "Ls");
            glUniform3fv(light_pos_id, ONE, glm::value_ptr(light_pos));
            glUniform3fv(La_id, ONE, glm::value_ptr(La));
            glUniform3fv(Ld_id, ONE, glm::value_ptr(Ld));
            glUniform3fv(Ls_id, ONE, glm::value_ptr(Ls));

            // 0: False, 1: True
            is_water_ = is_water;
            std::cout << is_water_ << std::endl;
            glUniform1f(glGetUniformLocation(program_id_, "isWater"), is_water_);

            //material?
            glm::vec3 ka = glm::vec3(1.0f, 1.0f, 1.0f);
            glm::vec3 kd = glm::vec3(0.6f, 0.95f, 1.0f);
            glm::vec3 ks = glm::vec3(1.0f, 1.0f, 1.0f);
            float alpha = 60.0f;
            GLuint ka_id = glGetUniformLocation(program_id_, "ka");
            GLuint kd_id = glGetUniformLocation(program_id_, "kd");
            GLuint ks_id = glGetUniformLocation(program_id_, "ks");
            GLuint alpha_id = glGetUniformLocation(program_id_, "alpha");
            glUniform3fv(ka_id, ONE, glm::value_ptr(ka));
            glUniform3fv(kd_id, ONE, glm::value_ptr(kd));
            glUniform3fv(ks_id, ONE, glm::value_ptr(ks));
            glUniform1f(alpha_id, alpha);



            // vertex one vertex Array
            glGenVertexArrays(1, &vertex_array_id_);
            glBindVertexArray(vertex_array_id_);

            // vertex coordinates and indices
            {
                std::vector<GLfloat> vertices;
                std::vector<GLuint> indices;

                int grid_dim = 512;

                GLuint grim_dim_id = glGetUniformLocation(program_id_, "triangles_number");
                glUniform1i(grim_dim_id, grid_dim);

                // the given code below are the vertices for a simple quad.
                // your grid should have the same dimension as that quad, i.e.,
                // reach from [-1, -1] to [1, 1].

                //generate all the vertices for the grid
                //some vertices have to be almost handwritten to avoid having
                //unwanted edges on the board. In the border the triangles are
                //half the size because the grid must be a square, so their vertices
                //are outside the second loop.

                int index = 0;
                vertices.push_back(-1.0f);
                vertices.push_back(-1.0f);
                indices.push_back(index++);
                float l = 2.0f/(float)grid_dim;
                for(int i = 0; i<grid_dim; i++) {
                    if(i%2 != 1){
                        for(int j = 0; j<grid_dim; j++) {
                            vertices.push_back((((float)j)*l)-1.0f);
                            vertices.push_back((((float)(i+1))*l)-1.0f);
                            indices.push_back(index++);
                            vertices.push_back((((float)j+0.5f)*l)-1.0f);
                            vertices.push_back((((float)i)*l)-1.0f);
                            indices.push_back(index++);
                        }
                        vertices.push_back((((float)grid_dim)*l)-1.0f);
                        vertices.push_back((((float)(i+1))*l)-1.0f);
                        indices.push_back(index++);
                        vertices.push_back((((float)grid_dim)*l)-1.0f);
                        vertices.push_back((((float)i)*l)-1.0f);
                        indices.push_back(index++);
                        vertices.push_back((((float)grid_dim)*l)-1.0f);
                        vertices.push_back((((float)(i+1))*l)-1.0f);
                        indices.push_back(index++);
                    } else {
                        vertices.push_back((((float)grid_dim)*l)-1.0f);
                        vertices.push_back((((float)(i+1))*l)-1.0f);
                        indices.push_back(index++);
                        vertices.push_back((((float)grid_dim)*l)-1.0f);
                        vertices.push_back((((float)(i))*l)-1.0f);
                        indices.push_back(index++);
                        for(int j = 0; j<grid_dim; j++) {
                            vertices.push_back(((-((float)j+0.5f))*l)+1.0f);
                            vertices.push_back((((float)(i+1))*l)-1.0f);
                            indices.push_back(index++);
                            vertices.push_back(((-((float)j+1.0f))*l)+1.0f);
                            vertices.push_back((((float)i)*l)-1.0f);
                            indices.push_back(index++);
                        }
                        vertices.push_back(-1.0f);
                        vertices.push_back((((float)i+1)*l)-1.0f);
                        indices.push_back(index++);
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

            string texture_filename[3];
            texture_filename[0] = "RockTexture.jpg";
            texture_filename[1] = "ForestTexture.jpg";
            //texture_filename[1] = "SandTexture.jpg";
            texture_filename[2] = "SandTexture.jpg";


            GLchar* texture_uniform[3];
            texture_uniform[0] = "tex2";
            texture_uniform[1] = "tex3";
            texture_uniform[2] = "tex4";

            GLuint texture_ids[3];

            // load image texture
            for(int i = 1; i<4; i++) {
                int width;
                int height;
                int nb_component;
                stbi_set_flip_vertically_on_load(1);
                unsigned char* image = stbi_load(texture_filename[i-1].c_str(), &width, &height, &nb_component, 0);
                if(image == nullptr) {
                    throw(std::string("Failed to load texture"));
                }
                glGenTextures(1, &image_texture_id_[i-1]);
                glBindTexture(GL_TEXTURE_2D, image_texture_id_[i-1]);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                if(nb_component == 3) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                                 GL_RGB, GL_UNSIGNED_BYTE, image);
                } else if(nb_component == 4) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                                 GL_RGBA, GL_UNSIGNED_BYTE, image);
                }
                texture_ids[i-1] = glGetUniformLocation(program_id_, texture_uniform[i-1]);
                glUniform1i(texture_ids[i-1], i /*GL_TEXTURE i*/);
                // cleanup
                glBindTexture(GL_TEXTURE_2D, 0);
                stbi_image_free(image);
            }

            for(int i = 1; i<5; i++) {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, image_texture_id_[i-1]);
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

            // 0: False, 1: True
            GLuint discard_loc = glGetUniformLocation(program_id_,"discard_pix");
            glUniform1i(discard_loc,discard);

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
