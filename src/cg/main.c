#include <stdio.h>
#include <float.h>
#include "base.h"
#include "cg.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "base.c"
#include "cg.c"
#include "glad.c"

char *vertex_src =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aNorm;\n"
"layout (location = 2) in vec3 aTexCoord;\n"
"out vec3 vNorm;\n"
"out vec3 vFragPos;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"  vFragPos = vec3(model * vec4(aPos, 1.0));\n"
"  vNorm = mat3(transpose(inverse(model))) * aNorm;\n"
"  gl_Position =  projection * view * model * vec4(aPos, 1.0);\n"
"}\0";

char *frag_src =
"#version 330 core\n"
"in vec3 vNorm;\n"
"in vec3 vFragPos;\n"
"uniform vec3 lightPos;\n"
"uniform vec3 viewPos;\n"
"uniform vec3 lightColor;\n"
"uniform vec3 objectColor;\n"
"out vec4 fColor;\n"
"void main()\n"
"{\n"
"  float ambientStrength = 0.1;\n"
"  vec3 ambient = ambientStrength * lightColor;\n"
"  vec3 norm = normalize(vNorm);\n"
"  vec3 lightDirection = normalize(lightPos - vFragPos);\n"
"  float diff = max(dot(norm, lightDirection), 0.0);\n"
"  vec3 diffuse = diff * lightColor;\n"
"  float specularStrength = 0.5;\n"
"  vec3 viewDirection = normalize(viewPos - vFragPos);\n"
"  vec3 reflectDirection = reflect(-lightDirection, norm);\n"
"  float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 32);\n"
"  vec3 specular = specularStrength * spec * lightColor;\n"
"  vec3 result = (ambient + diffuse + specular) * objectColor;\n"
"  fColor = vec4(result, 1.0f);\n"
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
  
  S32 width = 1280;
  S32 height = 720;
  
  Arena *arena = arena_alloc();
  String8 obj = os_file_read(arena, str8_lit("model/bmw.obj"));
  ObjModel model = obj_parse(arena, obj);
  
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  GLFWwindow *window = glfwCreateWindow(width, height, "Computer Graphics", 0, 0);
  if(window)
  {
    glfwMakeContextCurrent(window);
    if(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
      glfwSetFramebufferSizeCallback(window, framebuffer_resize);
      
      GLuint vertex_shader = ogl_make_shader(vertex_src, GL_VERTEX_SHADER);
      GLuint frag_shader = ogl_make_shader(frag_src, GL_FRAGMENT_SHADER);
      GLuint program = ogl_make_program((GLuint[]){vertex_shader, frag_shader}, 2);
      
      Mesh mesh = mesh_from_obj_model(arena, model);
      
      // find model center and scale
      F32 center[3] = {0};
      F32 min[3] = {+FLT_MAX, +FLT_MAX, +FLT_MAX};
      F32 max[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
      F32 scale = FLT_MAX;
      for(S64 t = 0; t < mesh.vertices.count; t += 1)
      {
        for(S32 i = 0; i < 3; i += 1)
        {
          F32 r = mesh.vertices.data[t].pos.v[i];
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
             (S32)mesh.vertices.count,
             (F64)center[0], (F64)center[1], (F64)center[2],
             (F64)scale);
      
      Vec3F32 light_pos = v3f32(3.f, 3.f, 2.f);
      Vec3F32 view_pos = v3f32(0.f, 0.f, 3.f);
      Vec3F32 light_color = v3f32(1.f, 1.f, 1.f);
      Vec3F32 object_color = v3f32(0.8f, 0.8f, 0.8f);
      
      Mat4x4F32 projection = m4x4f32_make_perspective(45.0f, 1280.f/720.f, 0.1f, 100.f);
      Mat4x4F32 view = m4x4f32_make_translate(v3f32(0.f, 0.f, -3.0f));
      
      while(!glfwWindowShouldClose(window))
      {
        // input
        input_handler(window);
        
        // rendering
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(program);
        
        Mat4x4F32 t = m4x4f32_make_translate(v3f32(-center[0], -center[1], -center[2]));
        Mat4x4F32 s = m4x4f32_make_scale(v3f32(1.0f/scale, 1.0f/scale, 1.0f/scale));
        F32 rotation = (F32)((U64)(glfwGetTime() * 1000.0) % (30 * 360)) / 30;
        Mat4x4F32 r = m4x4f32_make_rotate(v3f32(0.f, 1.f, 0.f), radians_from_degrees_f32(rotation));
        Mat4x4F32 model = m4x4f32_mul(m4x4f32_mul(t, s), r);
        
        ogl_shader_set_matrix4fv(program, "model", &model.v[0][0]);
        ogl_shader_set_matrix4fv(program, "view", &view.v[0][0]);
        ogl_shader_set_matrix4fv(program, "projection", &projection.v[0][0]);
        
        ogl_shader_set_3fv(program, "lightPos", 1, light_pos.v);
        ogl_shader_set_3fv(program, "viewPos", 1, view_pos.v);
        ogl_shader_set_3fv(program, "lightColor", 1, light_color.v);
        ogl_shader_set_3fv(program, "objectColor", 1, object_color.v);
        
        mesh_draw(mesh);
        
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
