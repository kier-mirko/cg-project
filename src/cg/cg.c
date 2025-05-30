function S32
s32_from_str8(String8 str)
{
  U32 r = 0;
  S32 sign = 1;
  for(S64 i = 0; i < str.size; i += 1)
  {
    switch(str.str[i])
    {
      case '+': break;
      case '-': sign = -1; break;
      default: r = 10*r + str.str[i] - '0';
    }
  }
  
  return r*sign;
}

function F32
f32_from_str8(String8 str)
{
  float r = 0.f;
  float sign = 1.f;
  float exp = 0.f;
  for(S64 i = 0; i < str.size; i += 1)
  {
    switch(str.str[i])
    {
      case '+': break;
      case '-': sign = -1; break;
      case '.': exp = 1; break;
      default: 
      {
        r = 10.f*r + (str.str[i] - '0');
        exp *= 0.1f;
      }
    }
  }
  
  return sign*r*(exp ? exp : 1.f);
}

function Vec3F32
obj_parse_v3f32(String8 str)
{
  Vec3F32 v = {0};
  String8Cut cut = str8_cut(str8_trim_left(str), ' ');
  v.x = f32_from_str8(cut.head);
  cut = str8_cut(str8_trim_left(cut.tail), ' ');
  v.y = f32_from_str8(cut.head);
  cut = str8_cut(str8_trim_left(cut.tail), ' ');
  v.z = f32_from_str8(cut.head);
  return v;
}

function ObjFace
obj_parse_face(String8 str, S64 verts_count, S64 norms_count)
{
  ObjFace result = {0};
  String8Cut fields = {0};
  fields.tail = str;
  for(S32 i = 0; i < 3; i += 1)
  {
    fields = str8_cut(str8_trim_left(fields.tail), ' ');
    String8Cut element = str8_cut(fields.head, '/');
    result.v[i] = s32_from_str8(element.head);
    element = str8_cut(element.tail, '/');
    element = str8_cut(element.tail, '/');
    result.n[i] = s32_from_str8(element.head);
    
    if(result.v[i] < 0)
    {
      result.v[i] = (S32)(result.v[i] + 1 + verts_count);
    }
    if(result.n[i] < 0)
    {
      result.n[i] = (S32)(result.n[i] + 1 + norms_count);
    }
  }
  return result;
}

function ObjModel 
obj_parse(Arena *arena, String8 obj)
{
  ObjModel m = {0};
  String8Cut lines = {0};
  
  lines.tail = obj;
  while(lines.tail.size)
  {
    lines = str8_cut(lines.tail, '\n');
    String8Cut fields = str8_cut(str8_trim_right(lines.head), ' ');
    String8 kind = fields.head;
    if(str8_match(kind, str8_lit("v"), 0))
    {
      m.verts_count += 1;
    }
    else if(str8_match(kind, str8_lit("vn"), 0))
    {
      m.norms_count += 1;
    }
    else if(str8_match(kind, str8_lit("f"), 0))
    {
      m.faces_count += 1;
    }
  }
  
  m.verts = push_array(arena, Vec3F32, m.verts_count);
  m.norms = push_array(arena, Vec3F32, m.norms_count);
  m.faces = push_array(arena, ObjFace, m.faces_count);
  m.verts_count = m.norms_count = m.faces_count = 0;
  
  lines.tail = obj;
  while(lines.tail.size)
  {
    lines = str8_cut(lines.tail, '\n');
    String8Cut fields = str8_cut(str8_trim_right(lines.head), ' ');
    String8 kind = fields.head;
    if(str8_match(kind, str8_lit("v"), 0))
    {
      m.verts[m.verts_count] = obj_parse_v3f32(fields.tail);
      m.verts_count += 1;
    }
    else if(str8_match(kind, str8_lit("vn"), 0))
    {
      m.norms[m.norms_count] = obj_parse_v3f32(fields.tail);
      m.norms_count += 1;
    }
    else if(str8_match(kind, str8_lit("f"), 0))
    {
      m.faces[m.faces_count] = obj_parse_face(fields.tail, m.verts_count, m.norms_count);
      m.faces_count += 1;
    }
  }
  
  return m;
}

function GLVertexArray
gl_vertex_array_from_obj(Arena *arena, ObjModel model)
{
  GLVertexArray result = {0};
  
  result.data = push_array(arena, GLVertex, model.faces_count*3);
  S64 count = 0;
  for(S64 f = 0; f < model.faces_count; f += 1)
  {
    B32 valid = 1;
    for(S32 i = 0; i < 3; i += 1)
    {
      valid &= (model.faces[f].v[i]>0 && model.faces[f].v[i]<=model.verts_count);
      valid &= (model.faces[f].n[i]>0 && model.faces[f].n[i]<=model.norms_count);
    }
    
    if(valid)
    {
      for(S64 i = 0; i < 3; i += 1)
      {
        result.data[count].norm = model.norms[model.faces[f].n[i]-1];
        result.data[count].pos = model.verts[model.faces[f].v[i]-1];
        count += 1;
      }
    }
  }
  
  result.count = count;
  return result;
}
