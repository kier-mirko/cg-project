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


typedef struct OBJFace OBJFace;
struct OBJFace
{
  S32 v[3];
  S32 n[3];
  S32 t[3];
};

typedef struct OBJGroup OBJGroup;
struct OBJGroup
{
  String8 name;
  S64 faces_count;
  S64 face_offset;
};

typedef struct OBJMaterial OBJMaterial;
struct OBJMaterial
{
  String8 name;
  Vec3F32 Ka;      // Ambient
  Vec3F32 Kd;      // Diffuse
  Vec3F32 Ks;      // Specular
  Vec3F32 Ke;      // Emission
  Vec3F32 Kt;      // Transmittance
  F32 Ns;         // Shininess
  F32 Ni;         // Index of refraction
  Vec3F32 Tf;      // Transmission filter
  F32 d;          // Disolve (alpha)
  F32 illum;      // Illumination model
  B32 fallback;
};

typedef struct OBJMesh OBJMesh;
struct OBJMesh
{
  Vec3F32 *verts;
  S64 verts_count;
  Vec3F32 *norms;
  S64 norms_count;
  Vec3F32 *textures;
  S64 textures_count;
  OBJFace *faces;
  S64 faces_count;
  OBJGroup *objects;
  S64 objects_count;
  OBJGroup *groups;
  S64 groups_count;
  OBJMaterial *materials;
  S64 materials_count;
};

typedef struct OBJ OBJ;
struct OBJ
{
  OBJMesh mesh;
  OBJGroup object; // current object and group
  OBJGroup group;
  S64 mat_index;   // current material
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
function OBJFace obj_parse_face(String8 str, S64 verts_count, S64 norms_count, S64 textures_count);
function OBJ obj_parse(Arena *arena, String8 obj);
function GLVertexArray gl_vertex_array_from_obj(Arena *arena, OBJ model);

////////////////////////////////
// Functions: Model

function Mesh mesh_from_obj_model(Arena *arena, OBJ model);
function void mesh_draw(Mesh mesh);

#endif //CG_H
