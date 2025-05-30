#include <stdio.h>
#include <float.h>
#include "base.h"
#include "cg.h"
#include "GLFW/glfw3.h"

#include "base.c"
#include "cg.c"
#include "glad.c"

char *vertex_src =
"#version 330 core\n"
"layout (location = 0) in vec3 pos;\n"
"layout (location = 1) in vec3 norm;\n"
"uniform mat4 modelview;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"  gl_Position =  projection * modelview * vec4(pos, 1.0);\n"
"}\0";

char *frag_src =
"#version 330 core\n"
"out vec4 frag_color;\n"
"void main()\n"
"{\n"
"  frag_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
"}\0";

function void
framebuffer_resize(GLFWwindow *window, int width, int height)
{
  glViewport(0, 0, width, height);
}

function void
input_handler(GLFWwindow *window)
{
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, 1);
  }
}

int
main(int argc, char **argv)
{
  os_main_init(argc, argv);
  int result = 0;
  
  Arena *arena = arena_alloc();
  String8 obj = os_file_read(arena, str8_lit("model/bmw.obj"));
  ObjModel model = obj_parse(arena, obj);
  GLVertexArray vertices = gl_vertex_array_from_obj(arena, model);
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  GLFWwindow *window = glfwCreateWindow(1280, 720, "Computer Graphics", 0, 0);
  if(window)
  {
    glfwMakeContextCurrent(window);
    if(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
      glfwSetFramebufferSizeCallback(window, framebuffer_resize);
      
      glEnable(GL_DEPTH_TEST);
      
      unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
      glShaderSource(vertex_shader, 1, &vertex_src, 0);
      glCompileShader(vertex_shader);
      
      int success;
      char infoLog[512];
      glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
      if (!success)
      {
        glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n %s\n", infoLog);
      }
      
      unsigned int frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
      glShaderSource(frag_shader, 1, &frag_src, 0);
      glCompileShader(frag_shader);
      glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
      if (!success)
      {
        glGetShaderInfoLog(frag_shader, 512, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n %s\n", infoLog);
      }
      
      unsigned int shader_program = glCreateProgram();
      glAttachShader(shader_program, vertex_shader);
      glAttachShader(shader_program, frag_shader);
      glLinkProgram(shader_program);
      glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
      if (!success)
      {
        glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n %s\n", infoLog);
      }
      
      glDeleteShader(vertex_shader);
      glDeleteShader(frag_shader);
      
      unsigned int vao;
      unsigned int vbo;
      
      glGenVertexArrays(1, &vao);
      glGenBuffers(1, &vbo);
      
      glBindVertexArray(vao);
      
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferData(GL_ARRAY_BUFFER, vertices.count*sizeof(GLVertex), vertices.data, GL_STATIC_DRAW);
      
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLVertex), (void*)OffsetOfMember(GLVertex, pos));
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLVertex), (void*)OffsetOfMember(GLVertex, norm));
      glEnableVertexAttribArray(1);
      
      glBindVertexArray(0);
      
      // find model center and scale
      F32 center[3] = {0};
      F32 min[3] = {+FLT_MAX, +FLT_MAX, +FLT_MAX};
      F32 max[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
      F32 scale = FLT_MAX;
      for(S64 t = 0; t < vertices.count; t += 1)
      {
        for(S32 i = 0; i < 3; i += 1)
        {
          F32 r = vertices.data[t].pos.v[i];
          min[i] = min[i]<r ? min[i] : r;
          max[i] = max[i]>r ? max[i] : r;
        }
      }
      for(S32 i = 0; i < 3; i += 1)
      {
        F32 size = max[i] - min[i];
        center[i] = min[i] + size/2;
        scale = scale<size ? scale : size;
      }
      
      
      printf("%d triangles [%+.2g %+.2g %+.2g] x%.3g\n",
             (S32)vertices.count,
             (F64)center[0], (F64)center[1], (F64)center[2],
             (F64)scale);
      
      while(!glfwWindowShouldClose(window))
      {
        // input
        input_handler(window);
        
        // rendering
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(shader_program);
        
        Mat4x4F32 t1 = m4x4f32_make_translate(v3f32(0.f, 0.f, -3.0f));
        Mat4x4F32 s = m4x4f32_make_scale(v3f32(1.f/scale, 1.f/scale, 1.f/scale));
        Mat4x4F32 t2 = m4x4f32_make_translate(v3f32(-center[0], -center[1], -center[2]));
        
        Mat4x4F32 modelview = m4x4f32_mul(t1, m4x4f32_mul(t2, s));
        Mat4x4F32 projection = m4x4f32_make_perspective(35.f, 1280.f/720.f, 1.f, 6.f);
        unsigned int modelview_loc = glGetUniformLocation(shader_program, "modelview");
        unsigned int projection_loc = glGetUniformLocation(shader_program, "projection");
        
        glUniformMatrix4fv(modelview_loc, 1, GL_FALSE, modelview.v[0]);
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, projection.v[0]);
        
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, (S32)(vertices.count));
        
        glfwSwapBuffers(window);
        glfwPollEvents();
      }
    }
  }
  else
  {
    printf("ERROR: failed to create GLFW window\n");
    result = -1;
  }
  return result;
}
