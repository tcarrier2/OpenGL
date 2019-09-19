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
#include <math.h>

static const GLuint WIDTH = 640;
static const GLuint HEIGHT = 480;

static const GLchar* vertex_shader_source_PP =
  "#version 120\n"
  "attribute vec3 coord3d;\n"
  "attribute vec2 texcoord;\n"
  "varying vec2 f_texcoord;\n"
  "uniform mat4 MVP;\n"
  "void main(){\n"
  "  gl_Position = MVP*vec4(coord3d, 1.0);\n"
  "  f_texcoord = texcoord;\n"
  "}\n";

static const GLchar* fragment_shader_source_PP =
  "#version 120\n"
  "varying vec2 f_texcoord;\n"
  "uniform sampler2D mytexture;\n"
  "float temp;\n"
  "void main(){\n"
  "  vec2 flipped_texcoord = vec2(-f_texcoord.y, f_texcoord.x);\n"
  "  gl_FragColor = texture2D(mytexture, flipped_texcoord);\n"
  "  temp = gl_FragColor[0];\n"
  "  gl_FragColor[0] = gl_FragColor[2];\n"
  "  gl_FragColor[2] = temp;\n"
  "}\n";

static const GLchar* vertex_shader_source =
  "#version 120\n"
  "attribute vec3 coord3d;\n"
  "attribute vec3 v_color;\n"
  "varying vec3 f_color;\n"
  "uniform mat4 MVP;\n"
  "void main(){\n"
  "  gl_Position = MVP*vec4(coord3d, 1.0);\n"
  "  f_color = v_color;\n"
  "}\n";

static const GLchar* fragment_shader_source =
  "#version 120\n"
  "varying vec3 f_color;\n"
  "void main(){\n"
  "  gl_FragColor = vec4(f_color.r, f_color.g, f_color.b, 1.0);\n"
  "}\n";

static GLfloat square_PP[] = {
  0.8, 1.6, 0.0,
  0.8, 0.0, 0.0,
 -0.8, 0.0, 0.0,
 -0.8, 1.6, 0.0,
  0.8, 1.6,-1.6,
  0.8, 0.0,-1.6,
  0.8, 0.0, 0.0,
  0.8, 1.6, 0.0,
 -0.8, 1.6,-1.6,
 -0.8, 0.0,-1.6,
 -0.8, 0.0, 0.0,
 -0.8, 1.6, 0.0,
  0.8, 1.6,-1.6,
  0.8, 0.0,-1.6,
 -0.8, 0.0,-1.6,
 -0.8, 1.6,-1.6,
};

GLfloat uv_texcoords_PP[] = {
  0.0, 0.5,
  0.5, 0.5,
  0.5, 1.0,
  0.0, 1.0,
  0.0, 0.0,
  0.5, 0.0,
  0.5, 0.5,
  0.0, 0.5,
  0.0, 0.0,
  0.5, 0.0,
  0.5, 0.5,
  0.0, 0.5,
  0.5, 0.5,
  1.0, 0.5,
  1.0, 1.0,
  0.5, 1.0,
};

static GLfloat triangle[] = {
 -1000.0, 1000.0,  -20.0,
 -1000.0,-1000.0,  -20.0,
  1000.0,-1000.0,  -20.0,
 -1000.0, 1000.0,  -20.0,
  1000.0, 1000.0,  -20.0,
  1000.0,-1000.0,  -20.0,
 -1000.0,    0.0, 1000.0,
 -1000.0,    0.0,-1000.0,
  1000.0,    0.0,-1000.0,
 -1000.0,    0.0, 1000.0,
  1000.0,    0.0, 1000.0,
  1000.0,    0.0,-1000.0,
   -20.0,-1000.0, 1000.0,
   -20.0,-1000.0,-1000.0,
   -20.0, 1000.0,-1000.0,
   -20.0,-1000.0, 1000.0,
   -20.0, 1000.0, 1000.0,
   -20.0, 1000.0,-1000.0,
    20.0,-1000.0, 1000.0,
    20.0,-1000.0,-1000.0,
    20.0, 1000.0,-1000.0,
    20.0,-1000.0, 1000.0,
    20.0, 1000.0, 1000.0,
    20.0, 1000.0,-1000.0,
 -1000.0, 1000.0,   20.0,
 -1000.0,-1000.0,   20.0,
  1000.0,-1000.0,   20.0,
 -1000.0, 1000.0,   20.0,
  1000.0, 1000.0,   20.0,
  1000.0,-1000.0,   20.0,
};

static GLfloat triangle_colors[] = {
  1.0, 1.0, 1.0,
  0.0, 0.0, 1.0,
  0.0, 0.0, 1.0,
  1.0, 1.0, 1.0,
  1.0, 1.0, 1.0,
  0.0, 0.0, 1.0,
  0.0, 0.4, 0.0,
  0.0, 0.4, 0.0,
  0.0, 0.4, 0.0,
  0.0, 0.4, 0.0,
  0.0, 0.4, 0.0,
  0.0, 0.4, 0.0,
  1.0, 1.0, 1.0,
  0.0, 0.0, 1.0,
  0.0, 0.0, 1.0,
  1.0, 1.0, 1.0,
  1.0, 1.0, 1.0,
  0.0, 0.0, 1.0,
  1.0, 1.0, 1.0,
  0.0, 0.0, 1.0,
  0.0, 0.0, 1.0,
  1.0, 1.0, 1.0,
  1.0, 1.0, 1.0,
  0.0, 0.0, 1.0,
  1.0, 1.0, 1.0,
  1.0, 1.0, 1.0,
  0.0, 0.0, 1.0,
  1.0, 1.0, 1.0,
  1.0, 1.0, 1.0,
  0.0, 0.0, 1.0,
};

GLuint common_get_shader_program(const char* vertex_shader_source, const char* fragment_shader_source){
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
  GLint attribute_coord3d_PP, attribute_texcoord_PP, attribute_coord3d, attribute_v_color;
  GLuint program_PP, program, vbo_PP, vbo, vbo_texcoords_PP, vbo_colors;
  SDL_Event event;
  SDL_GLContext gl_context;
  SDL_Window* window;

  SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO);
  window = SDL_CreateWindow(__FILE__, 0, 0, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
  gl_context = SDL_GL_CreateContext(window);
  glewInit();

  GLuint texture_id_PP;
  GLint uniform_mytexture_PP;
  SDL_Surface* res_texture_PP = IMG_Load("texture.png");
  glGenTextures(1, &texture_id_PP);
  glBindTexture(GL_TEXTURE_2D, texture_id_PP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, res_texture_PP->w, res_texture_PP->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, res_texture_PP->pixels);
  SDL_FreeSurface(res_texture_PP);

  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 1);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  program_PP = common_get_shader_program(vertex_shader_source_PP, fragment_shader_source_PP);
  attribute_coord3d_PP = glGetAttribLocation(program_PP, "coord3d");
  attribute_texcoord_PP = glGetAttribLocation(program_PP, "texcoord");
  GLuint MatrixID_PP = glGetUniformLocation(program_PP, "MVP");
  
  program = common_get_shader_program(vertex_shader_source, fragment_shader_source);
  attribute_coord3d = glGetAttribLocation(program, "coord3d");
  attribute_v_color = glGetAttribLocation(program, "v_color");
  GLuint MatrixID = glGetUniformLocation(program, "MVP");

  glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f/3.0f, 0.1f, 100.0f);	
  glm::mat4 View = glm::lookAt(glm::vec3(0,0,2), glm::vec3(0,0,0), glm::vec3(0,1,0));
  glm::mat4 myScalingMatrix = glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f));
  
  glm::vec3 x_RotationAxis(1.0f, 0.0f, 0.0f);
  glm::mat4 x_RotationMatrix;
  
  glm::vec3 y_RotationAxis(0.0f, 1.0f, 0.0f);
  glm::mat4 y_RotationMatrix;
  
  glm::mat4 myTranslationMatrix;
  
  glm::mat4 Model;
  glm::mat4 MVP;

  glGenBuffers(1, &vbo_texcoords_PP);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords_PP);
  glBufferData(GL_ARRAY_BUFFER, sizeof(uv_texcoords_PP), uv_texcoords_PP, GL_STATIC_DRAW);

  glActiveTexture(GL_TEXTURE0);
  glUniform1i(uniform_mytexture_PP, 0);
  glBindTexture(GL_TEXTURE_2D, texture_id_PP);

  glViewport(0, 0, WIDTH, HEIGHT);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  SDL_SetRelativeMouseMode(SDL_TRUE);

  int* mouse_x = (int*)malloc(sizeof(int));
  int* mouse_y = (int*)malloc(sizeof(int));

  float x_position;
  float y_position;
  float z_position;
  x_position = 0.0f;
  y_position = 0.0f;
  z_position = 0.0f;

  float x_axis_angle;
  float y_axis_angle;
  x_axis_angle = 0.0f;
  y_axis_angle = 0.0f;

  float v_y = 0.0f;
  float t = 0.0f;
  float constant = 0.0f;

  bool arrow_up = 0;
  bool arrow_down = 0;
  bool arrow_right = 0;
  bool arrow_left = 0;
  bool spacebar = 0;

  while(1){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    x_RotationMatrix = glm::rotate(glm::radians(x_axis_angle), x_RotationAxis);
    y_RotationMatrix = glm::rotate(glm::radians(y_axis_angle), y_RotationAxis);

    myTranslationMatrix = glm::translate(glm::mat4(), glm::vec3(x_position, y_position, z_position));

    Model = myScalingMatrix * x_RotationMatrix * y_RotationMatrix * myTranslationMatrix;
    MVP = Projection*Model;

    glEnableVertexAttribArray(attribute_texcoord_PP);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords_PP);
    glVertexAttribPointer(attribute_texcoord_PP, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glGenBuffers(1, &vbo_PP);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_PP);
    glBufferData(GL_ARRAY_BUFFER, sizeof(square_PP), square_PP, GL_STATIC_DRAW);
    glVertexAttribPointer(attribute_coord3d_PP, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glUseProgram(program_PP);
    glEnableVertexAttribArray(attribute_coord3d_PP);
    glEnableVertexAttribArray(attribute_texcoord_PP);
    glUniformMatrix4fv(MatrixID_PP, 1, GL_FALSE, &MVP[0][0]);
    glDrawArrays(GL_QUADS, 0, 16);
    glDisableVertexAttribArray(attribute_texcoord_PP);
    glDisableVertexAttribArray(attribute_coord3d_PP);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
    glVertexAttribPointer(attribute_coord3d, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glGenBuffers(1, &vbo_colors);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_colors);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_colors), triangle_colors, GL_STATIC_DRAW);
    glVertexAttribPointer(attribute_v_color, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glUseProgram(program);
    glEnableVertexAttribArray(attribute_coord3d);
    glEnableVertexAttribArray(attribute_v_color);
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 30);
    glDisableVertexAttribArray(attribute_v_color);
    glDisableVertexAttribArray(attribute_coord3d);
    
    SDL_GetRelativeMouseState(mouse_x, mouse_y);
    x_axis_angle = x_axis_angle + ((float)*mouse_y)/100.0;
    y_axis_angle = y_axis_angle + ((float)*mouse_x)/100.0;

    if(event.type == SDL_KEYDOWN){
      if(event.key.keysym.sym == SDLK_z) arrow_up = 1;
      if(event.key.keysym.sym == SDLK_s) arrow_down = 1;
      if(event.key.keysym.sym == SDLK_d) arrow_right = 1;
      if(event.key.keysym.sym == SDLK_q) arrow_left = 1;
      if(event.key.keysym.sym == SDLK_SPACE) spacebar = 1;

      if(event.key.keysym.sym == SDLK_ESCAPE) break;
    }
    else if(event.type == SDL_KEYUP){
      if(event.key.keysym.sym == SDLK_z) arrow_up = 0;
      if(event.key.keysym.sym == SDLK_s) arrow_down = 0;
      if(event.key.keysym.sym == SDLK_d) arrow_right = 0;
      if(event.key.keysym.sym == SDLK_q) arrow_left = 0;
      if(event.key.keysym.sym == SDLK_SPACE) spacebar = 0;
    }

    if(arrow_up){
      z_position = z_position + 0.1*cos(y_axis_angle*M_PI/180.0)*cos(x_axis_angle*M_PI/180.0);
      x_position = x_position - 0.1*sin(y_axis_angle*M_PI/180.0)*cos(x_axis_angle*M_PI/180.0);
      y_position = y_position + 0.1*sin(x_axis_angle*M_PI/180.0);
    }
    if(arrow_down){
      z_position = z_position - 0.1*cos(y_axis_angle*M_PI/180.0)*cos(x_axis_angle*M_PI/180.0);
      x_position = x_position + 0.1*sin(y_axis_angle*M_PI/180.0)*cos(x_axis_angle*M_PI/180.0);
      y_position = y_position - 0.1*sin(x_axis_angle*M_PI/180.0);
    }
    if(arrow_right){
      x_position = x_position - 0.1*cos(y_axis_angle*M_PI/180.0);
      z_position = z_position - 0.1*sin(y_axis_angle*M_PI/180.0);
    }
    if(arrow_left){
      x_position = x_position + 0.1*cos(y_axis_angle*M_PI/180.0);
      z_position = z_position + 0.1*sin(y_axis_angle*M_PI/180.0);
    }
    if(spacebar){
      constant = -0.25;
    }

    v_y = 0.1*t + constant;
    y_position = y_position + 0.2*v_y;
    t = t + 0.1;
    
    if(y_position > -0.5){
      y_position = -0.5;
      constant = 0.0;
      v_y = 0.0;
      t = 0.0;
    }

    SDL_GL_SwapWindow(window);
    if(SDL_PollEvent(&event) && event.type == SDL_QUIT) break;
  }

  free(mouse_x);
  free(mouse_y);
  glDeleteTextures(1, &texture_id_PP);
  glDeleteBuffers(1, &vbo_PP);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &vbo_colors);
  glDeleteProgram(program_PP);
  glDeleteProgram(program);
  SDL_GL_DeleteContext(gl_context);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return EXIT_SUCCESS;
}