#define GLEW_STATIC

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <OpenGL/gl.h>
#include <OpenGl/glu.h>
#include <GLUT/glut.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <SDL2_image/SDL_image.h>

static const GLuint WIDTH = 512;
static const GLuint HEIGHT = 512;

static const GLchar* vertex_shader_source_PP =
  "#version 120\n"
  "attribute vec3 coord3d;\n"
  "attribute vec2 texcoord;\n"
  "varying vec2 f_texcoord;\n"
  "void main(){\n"
  "  gl_Position = vec4(coord3d, 1.0);\n"
  "  f_texcoord = texcoord;\n"
  "}\n";

static const GLchar* fragment_shader_source_PP =
  "#version 120\n"
  "varying vec2 f_texcoord;\n"
  "uniform sampler2D mytexture;\n"
  "float k = 0.01;\n"
  "void main(){\n"
  "  vec2 flipped_texcoord = vec2(f_texcoord.y, -f_texcoord.x);\n"
  "  gl_FragColor = texture2D(mytexture, flipped_texcoord);\n"
  "  gl_FragColor = gl_FragColor + 1.0*texture2D(mytexture, flipped_texcoord + k*vec2(1.0,0.0));\n"
  "  gl_FragColor = gl_FragColor + 1.0*texture2D(mytexture, flipped_texcoord + k*vec2(-1.0,0.0));\n"
  "  gl_FragColor = gl_FragColor + 1.0*texture2D(mytexture, flipped_texcoord + k*vec2(0.0,1.0));\n"
  "  gl_FragColor = gl_FragColor + 1.0*texture2D(mytexture, flipped_texcoord + k*vec2(0.0,-1.0));\n"
  "  gl_FragColor = gl_FragColor + 1.0*texture2D(mytexture, flipped_texcoord + k*vec2(1.0,1.0));\n"
  "  gl_FragColor = gl_FragColor + 1.0*texture2D(mytexture, flipped_texcoord + k*vec2(-1.0,1.0));\n"
  "  gl_FragColor = gl_FragColor + 1.0*texture2D(mytexture, flipped_texcoord + k*vec2(1.0,-1.0));\n"
  "  gl_FragColor = gl_FragColor + 1.0*texture2D(mytexture, flipped_texcoord + k*vec2(-1.0,-1.0));\n"
  "  gl_FragColor = gl_FragColor/9.0;\n"
  "}\n";

static const GLchar* vertex_shader_source =
  "#version 120\n"
  "attribute vec3 coord3d;\n"
  "attribute vec3 v_color;\n"
  "varying vec3 f_color;\n"
  "uniform mat4 MVP;\n"
  "void main(){\n"
  "  gl_Position = MVP*vec4(coord3d, 1.0);\n"
  "}\n";

static const GLchar* fragment_shader_source =
  "#version 120\n"
  "void main(){\n"
  "  gl_FragColor[0] = 1.0;\n"
  "  gl_FragColor[1] = 1.0;\n"
  "  gl_FragColor[2] = 0.0;\n"
  "  gl_FragColor[3] = 1.0;\n"
  "}\n";

static GLfloat square[] = {
  0.8, 0.8, 0.0,
  0.8,-0.8, 0.0,
 -0.8,-0.8, 0.0,
 -0.8, 0.8, 0.0,
};

static GLfloat triangle[] = {
  0.0, 0.5, 0.0,
 -0.5,-0.5, 0.0,
  0.5,-0.5, 0.0,
};

GLuint common_get_shader_program(
const char* vertex_shader_source,
const char* fragment_shader_source){
  GLchar* log = NULL;
  GLint log_length, success;
  GLuint fragment_shader, program, vertex_shader;

  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
  glCompileShader(vertex_shader);
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
  glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &log_length);
  log = (GLchar*)malloc(log_length);
  if(log_length > 0){
    glGetShaderInfoLog(vertex_shader, log_length, NULL, log);
    printf("vertex shader log:\n\n%s\n", log);
  }
  if(!success){
    printf("vertex shader compile error\n");
    exit(EXIT_FAILURE);
  }

  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
  glCompileShader(fragment_shader);
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &log_length);
  if(log_length > 0){
    log = (GLchar*)realloc(log, log_length);
    glGetShaderInfoLog(fragment_shader, log_length, NULL, log);
    printf("fragment shader log:\n\n%s\n", log);
  }
  if(!success){
    printf("fragment shader compile error\n");
    exit(EXIT_FAILURE);
  }

  program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
  if(log_length > 0){
    log = (GLchar*)realloc(log, log_length);
    glGetProgramInfoLog(program, log_length, NULL, log);
    printf("shader link log:\n\n%s\n", log);
  }
  if(!success){
    printf("shader link error");
    exit(EXIT_FAILURE);
  }

  free(log);
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  return program;
}

int main(void){
  GLint attribute_coord3d_PP, attribute_texcoord_PP, attribute_coord3d;
  GLuint program_PP, program, vbo_PP, vbo, vbo_texcoords_PP;
  SDL_Event event;
  SDL_GLContext gl_context;
  SDL_Window* window;

  SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO);
  window = SDL_CreateWindow(__FILE__, 0, 0, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
  gl_context = SDL_GL_CreateContext(window);
  glewInit();

  unsigned int framebuffer;
  glGenFramebuffers(1, &framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

  unsigned int texColorBuffer_PP;
  GLint uniform_mytexture_PP;
  glGenTextures(1, &texColorBuffer_PP);
  glBindTexture(GL_TEXTURE_2D, texColorBuffer_PP);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer_PP, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 1);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  program_PP = common_get_shader_program(vertex_shader_source_PP, fragment_shader_source_PP);
  attribute_coord3d_PP = glGetAttribLocation(program_PP, "coord3d");
  attribute_texcoord_PP = glGetAttribLocation(program_PP, "texcoord");
  
  program = common_get_shader_program(vertex_shader_source, fragment_shader_source);
  attribute_coord3d = glGetAttribLocation(program, "coord3d");
  GLuint MatrixID = glGetUniformLocation(program, "MVP");

  glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f/3.0f, 0.1f, 100.0f);	
  glm::mat4 View = glm::lookAt(glm::vec3(0,0,2), glm::vec3(0,0,0), glm::vec3(0,1,0));
  glm::mat4 myTranslationMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f));
  glm::mat4 myScalingMatrix = glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f));
  glm::vec3 myRotationAxis(0.0f, 0.0f, 1.0f);
  glm::mat4 myRotationMatrix = glm::rotate(glm::radians(0.0f), myRotationAxis);
  glm::mat4 Model = myTranslationMatrix*myRotationMatrix*myScalingMatrix;
  glm::mat4 MVP = Projection*View*Model;

  GLfloat uv_texcoords_PP[] = {
    0.0, 0.0,
    1.0, 0.0,
    1.0, 1.0,
    0.0, 1.0,
  };

  glGenBuffers(1, &vbo_texcoords_PP);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords_PP);
  glBufferData(GL_ARRAY_BUFFER, sizeof(uv_texcoords_PP), uv_texcoords_PP, GL_STATIC_DRAW);

  glEnableVertexAttribArray(attribute_texcoord_PP);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords_PP);
  glVertexAttribPointer(attribute_texcoord_PP, 2, GL_FLOAT, GL_FALSE, 0, 0);

  glViewport(0, 0, WIDTH, HEIGHT);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  while(1){    
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
    glVertexAttribPointer(attribute_coord3d, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glUseProgram(program);
    glEnableVertexAttribArray(attribute_coord3d);
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(attribute_coord3d);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glGenBuffers(1, &vbo_PP);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_PP);
    glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW);
    glVertexAttribPointer(attribute_coord3d_PP, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(uniform_mytexture_PP, /*GL_TEXTURE*/0);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer_PP);
    glUseProgram(program_PP);
    glEnableVertexAttribArray(attribute_coord3d_PP);
    glEnableVertexAttribArray(attribute_texcoord_PP);
    glDrawArrays(GL_QUADS, 0, 4);
    glDisableVertexAttribArray(attribute_texcoord_PP);
    glDisableVertexAttribArray(attribute_coord3d_PP);
        
    SDL_GL_SwapWindow(window);
    if(SDL_PollEvent(&event) && event.type == SDL_QUIT) break;
  }

  glDeleteTextures(1, &texColorBuffer_PP);
  glDeleteBuffers(1, &vbo_PP);
  glDeleteBuffers(1, &vbo);
  glDeleteProgram(program_PP);
  glDeleteProgram(program);
  SDL_GL_DeleteContext(gl_context);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return EXIT_SUCCESS;
}
