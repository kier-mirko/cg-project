#ifndef CG_H
#define CG_H

////////////////////////////////
// Types: OpenGL basic types

typedef U32 GLenum;
typedef F32 GLfloat;
typedef U32 GLbitfield;
typedef U32 GLuint;
typedef S32 GLint;
typedef S32 GLsizei;
typedef char GLchar;
typedef unsigned char GLboolean;


typedef struct ObjFace ObjFace;
struct ObjFace
{
  S32 v[3];
  S32 n[3];
  S32 t[3];
};

typedef struct ObjModel ObjModel;
struct ObjModel
{
  Vec3F32 *verts;
  S64 verts_count;
  Vec3F32 *norms;
  S64 norms_count;
  Vec3F32 *textures;
  S64 textures_count;
  ObjFace *faces;
  S64 faces_count;
};

typedef struct GLVertex GLVertex;
struct GLVertex
{
  Vec3F32 pos;
  Vec3F32 norm;
  Vec3F32 text;
};

typedef struct GLVertexArray GLVertexArray;
struct GLVertexArray
{
  GLVertex *data;
  S64 count;
};

typedef struct Mesh Mesh;
struct Mesh
{
  GLVertexArray vertices;
  GLuint vao;
  GLuint vbo;
};

////////////////////////////////
// Functions: Shader

function GLuint ogl_make_shader(char *src, GLenum kind);
function GLuint ogl_make_program(GLuint *shaders, S32 count);
function void   ogl_shader_set_3fv(GLuint program, char *name, GLsizei count, GLfloat *value);
function void   ogl_shader_set_matrix4fv(GLuint program, char *name, GLfloat *value);

////////////////////////////////
// Functions: OBJ Parser

function Vec3F32 obj_parse_v3f32(String8 str);
function ObjFace obj_parse_face(String8 str, S64 verts_count, S64 norms_count, S64 textures_count);
function ObjModel obj_parse(Arena *arena, String8 obj);
function GLVertexArray gl_vertex_array_from_obj(Arena *arena, ObjModel model);

////////////////////////////////
// Functions: Model

function Mesh mesh_from_obj_model(Arena *arena, ObjModel model);
function void mesh_draw(Mesh mesh);

#endif //CG_H
