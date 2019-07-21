#define GLEW_STATIC

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <OpenGL/gl.h>
#include <OpenGl/glu.h>
#include <GLUT/glut.h>
#include <SDL2/SDL.h>

static const GLuint WIDTH = 512;
static const GLuint HEIGHT = 512;

static const GLchar* vertex_shader_source =
  "#version 120\n"
  "attribute vec2 coord2d;\n"
  "attribute vec3 v_color;\n"
  "varying vec3 f_color;\n"
  "uniform float time;\n"
  "float position;\n"
  "void main(){\n"
  "  position = coord2d[1] + 0.1*cos(20.0*time*coord2d[0]);\n" 
  "  gl_Position = vec4(coord2d[0], position, 0.0, 1.0);\n"
  "  f_color = v_color;\n"
  "}\n";

static const GLchar* fragment_shader_source =
  "#version 120\n"
  "varying vec3 f_color;\n"
  "void main(){\n"
  "  if(gl_FragCoord.x > 160){\n"
  "    gl_FragColor = vec4(f_color.r, f_color.g, f_color.b, 1.0);\n"
  "  }\n"
  "  else{\n"
  "    gl_FragColor = vec4(0.0, f_color.g, f_color.b, 1.0);\n"
  "  }\n"
  "}\n";

static GLfloat vertices[] = {
 -0.4, 0.8,
 -0.8,-0.8,
  0.8,-0.8,
};

GLfloat triangle_colors[] = {
  1.0, 1.0, 0.0,
  0.0, 0.0, 1.0,
  1.0, 0.0, 0.0,
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
  GLint attribute_coord2d, attribute_v_color, loc_time;
  GLuint program, vbo, vbo_colors;
  SDL_Event event;
  SDL_GLContext gl_context;
  SDL_Window* window;

  SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO);
  window = SDL_CreateWindow(__FILE__, 0, 0, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
  gl_context = SDL_GL_CreateContext(window);
  glewInit();

  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 1);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  program = common_get_shader_program(vertex_shader_source, fragment_shader_source);
  attribute_coord2d = glGetAttribLocation(program, "coord2d");
  attribute_v_color = glGetAttribLocation(program, "v_color");
  loc_time = glGetUniformLocationARB(program, "time");

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenBuffers(1, &vbo_colors);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_colors);
  glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_colors), triangle_colors, GL_STATIC_DRAW);
  glVertexAttribPointer(attribute_v_color, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glUseProgram(program);
  glViewport(0, 0, WIDTH, HEIGHT);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  float time;
  time = 0.0;

  while(1){
    glClear(GL_COLOR_BUFFER_BIT);
    glEnableVertexAttribArray(attribute_coord2d);
    glEnableVertexAttribArray(attribute_v_color);
    glUniform1fARB(loc_time, time);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(attribute_coord2d);
    glDisableVertexAttribArray(attribute_v_color);
    SDL_GL_SwapWindow(window);
    if(SDL_PollEvent(&event) && event.type == SDL_QUIT) break;
    time = time + 0.01;
  }

  glDeleteBuffers(1, &vbo);
  glDeleteProgram(program);
  SDL_GL_DeleteContext(gl_context);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return EXIT_SUCCESS;
}
