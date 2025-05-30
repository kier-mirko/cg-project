#ifndef CG_H
#define CG_H

typedef struct ObjFace ObjFace;
struct ObjFace
{
  S32 v[3];
  S32 n[3];
};

typedef struct ObjModel ObjModel;
struct ObjModel
{
  Vec3F32 *verts;
  S64 verts_count;
  Vec3F32 *norms;
  S64 norms_count;
  ObjFace *faces;
  S64 faces_count;
};

typedef struct GLVertex GLVertex;
struct GLVertex
{
  Vec3F32 pos;
  Vec3F32 norm;
};

typedef struct GLVertexArray GLVertexArray;
struct GLVertexArray
{
  GLVertex *data;
  S64 count;
};

function Vec3F32 obj_parse_v3f32(String8 str);
function ObjFace obj_parse_face(String8 str, S64 verts_count, S64 norms_count);
function ObjModel obj_parse(Arena *arena, String8 obj);
function GLVertexArray gl_vertex_array_from_obj(Arena *arena, ObjModel model);

#endif //CG_H
