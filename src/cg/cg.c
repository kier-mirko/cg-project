function GLuint
ogl_make_shader(char *src, GLenum kind)
{
  GLuint result = 0;
  Temp scratch = scratch_begin(0, 0);
  
  GLuint shader = glCreateShader(kind);
  
  glShaderSource(shader, 1, (GLchar const*const*)&src, 0);
  
  glCompileShader(shader);
  
  GLint compile_status = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
  
  if(compile_status == 0)
  {
    {
      String8 log = {0};
      GLint info_log_lenth = 0;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_lenth);
      if(info_log_lenth > 0)
      {
        char *buffer = push_array(scratch.arena, char, info_log_lenth + 1);
        GLint length = 0;
        glGetShaderInfoLog(shader, info_log_lenth + 1, &length, buffer);
        log = str8((U8*)buffer, length);
      }
      
      const char *vendor       = glGetString(GL_VENDOR);
      const char *renderer     = glGetString(GL_RENDERER);
      const char *version      = glGetString(GL_VERSION);
      const char *glsl_version = glGetString(GL_SHADING_LANGUAGE_VERSION);
      
      printf("%s\n%s\n%s\n%s\n"
             "Shader Errors:\n%*.s", 
             vendor, renderer, version, glsl_version, str8_expand(log));
    }
    
    glDeleteShader(shader);
    shader = 0;
  }
  
  result = shader;
  scratch_end(scratch);
  return result;
}

function GLuint
ogl_make_program(GLuint *shaders, S32 count)
{
  GLuint result = 0;
  Temp scratch = scratch_begin(0, 0);
  
  GLuint program = glCreateProgram();
  for(S32 i = 0; i < count; i += 1)
  {
    glAttachShader(program, shaders[i]);
  }
  glLinkProgram(program);
  
  GLint link_status = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &link_status);
  
  if(link_status == 0)
  {
    {
      String8 log = {0};
      GLint info_log_length = 0;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);
      if(info_log_length > 0)
      {
        char *buffer = push_array(scratch.arena, char, info_log_length + 1);
        GLint lenght = 0;
        glGetProgramInfoLog(program, info_log_length + 1, &lenght, buffer);
        log = str8((U8*)buffer, lenght);
      }
      
      const char *vendor       = glGetString(GL_VENDOR);
      const char *renderer     = glGetString(GL_RENDERER);
      const char *version      = glGetString(GL_VERSION);
      const char *glsl_version = glGetString(GL_SHADING_LANGUAGE_VERSION);
      
      printf("%s\n%s\n%s\n%s\n"
             "Shader Errors:\n%*.s", 
             vendor, renderer, version, glsl_version, str8_expand(log));
    }
    
    glDeleteProgram(program);
    program = 0;
  }
  
  result = program;
  scratch_end(scratch);
  return result;
}

function void
ogl_shader_set_3fv(GLuint program, char *name, GLsizei count, GLfloat *value)
{
  GLint location = glGetUniformLocation(program, name);
  glUniform3fv(location, count, value);
}

function void
ogl_shader_set_matrix4fv(GLuint program, char *name, GLfloat *value)
{
  GLint location = glGetUniformLocation(program, name);
  glUniformMatrix4fv(location, 1, GL_FALSE, value);
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

function OBJFace
obj_parse_face(String8 str, S64 verts_count, S64 norms_count, S64 textures_count)
{
  OBJFace result = {0};
  String8Cut fields = {0};
  fields.tail = str;
  for(S32 i = 0; i < 3; i += 1)
  {
    fields = str8_cut(str8_trim_left(fields.tail), ' ');
    String8Cut element = str8_cut(fields.head, '/');
    result.v[i] = s32_from_str8(element.head);
    element = str8_cut(element.tail, '/');
    result.t[i] = s32_from_str8(element.head);
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
    if(result.t[i] < 0)
    {
      result.t[i] = (S32)(result.t[i] + 1 + textures_count);
    }
  }
  
  return result;
}

function OBJGroup
obj_parse_object(String8 str)
{
  OBJGroup result = {0};
  result.name = str8_trim_left(str);
  return result;
}

function OBJGroup
obj_parse_group(String8 str)
{
  OBJGroup result = {0};
  result.name = str8_trim_left(str);
  return result;
}

function S64
obj_parse_mtllib(Arena *arena, String8 str, OBJMaterial *materials_out)
{
  Temp scratch = scratch_begin(0, 0);
  String8 file_name = str8_trim_left(str);
  String8 content = os_file_read(scratch.arena, str8_push_cat(scratch.arena, str8_lit("model/"), file_name));
  
  S64 materials_count = 0;
  String8Cut lines = {0};
  lines.tail = content;
  while(lines.tail.size)
  {
    lines = str8_cut(lines.tail, '\n');
    String8Cut fields = str8_cut(str8_trim_left(str8_trim_right(lines.head)), ' ');
    String8 kind = fields.head;
    if(str8_match(kind, str8_lit("newmtl"), 0))
    {
      materials_out[materials_count].name = str8_trim_left(fields.tail);
      materials_count += 1;
    }
    else if(str8_match(kind, str8_lit("Ka"), 0))
    {
      materials_out[materials_count-1].Ka = obj_parse_v3f32(fields.tail);
    }
    else if(str8_match(kind, str8_lit("Kd"), 0))
    {
      materials_out[materials_count-1].Kd = obj_parse_v3f32(fields.tail);
    }
    else if(str8_match(kind, str8_lit("Ks"), 0))
    {
      materials_out[materials_count-1].Ks = obj_parse_v3f32(fields.tail);
    }
    else if(str8_match(kind, str8_lit("Ke"), 0))
    {
      materials_out[materials_count-1].Ke = obj_parse_v3f32(fields.tail);
    }
    else if(str8_match(kind, str8_lit("Kt"), 0))
    {
      materials_out[materials_count-1].Kt = obj_parse_v3f32(fields.tail);
    }
    else if(str8_match(kind, str8_lit("Ns"), 0))
    {
      materials_out[materials_count-1].Ns = f32_from_str8(str8_trim_left(fields.tail));
    }
    else if(str8_match(kind, str8_lit("Ni"), 0))
    {
      materials_out[materials_count-1].Ni = f32_from_str8(str8_trim_left(fields.tail));
    }
    else if(str8_match(kind, str8_lit("Tf"), 0))
    {
      materials_out[materials_count-1].Tf = obj_parse_v3f32(fields.tail);
    }
    else if(str8_match(kind, str8_lit("d"), 0))
    {
      materials_out[materials_count].d = f32_from_str8(str8_trim_left(fields.tail));
    }
    else if(str8_match(kind, str8_lit("illum"), 0))
    {
      materials_out[materials_count].illum = f32_from_str8(str8_trim_left(fields.tail));
    }
  }
  
  scratch_end(scratch);
  return materials_count;
}

function OBJ 
obj_parse(Arena *arena, String8 obj)
{
  OBJ m = {0};
  String8Cut lines = {0};
  
  lines.tail = obj;
  while(lines.tail.size)
  {
    lines = str8_cut(lines.tail, '\n');
    String8Cut fields = str8_cut(str8_trim_right(lines.head), ' ');
    String8 kind = fields.head;
    if(str8_match(kind, str8_lit("v"), 0))
    {
      m.mesh.verts_count += 1;
    }
    else if(str8_match(kind, str8_lit("vn"), 0))
    {
      m.mesh.norms_count += 1;
    }
    else if(str8_match(kind, str8_lit("vt"), 0))
    {
      m.mesh.textures_count += 1;
    }
    else if(str8_match(kind, str8_lit("f"), 0))
    {
      m.mesh.faces_count += 1;
    }
    else if(str8_match(kind, str8_lit("o"), 0))
    {
      m.mesh.objects_count += 1;
    }
    else if(str8_match(kind, str8_lit("g"), 0))
    {
      m.mesh.groups_count += 1;
    }
  }
  
  m.mesh.verts     = push_array(arena, Vec3F32, m.mesh.verts_count);
  m.mesh.norms     = push_array(arena, Vec3F32, m.mesh.norms_count);
  m.mesh.textures  = push_array(arena, Vec3F32, m.mesh.textures_count);
  m.mesh.faces     = push_array(arena, OBJFace, m.mesh.faces_count);
  m.mesh.objects   = push_array(arena, OBJGroup, m.mesh.objects_count);
  m.mesh.groups    = push_array(arena, OBJGroup, m.mesh.groups_count);
  
  m.mesh.verts_count = m.mesh.norms_count = m.mesh.textures_count = 0;
  m.mesh.faces_count = m.mesh.objects_count = m.mesh.groups_count = 0;
  
  lines.tail = obj;
  while(lines.tail.size)
  {
    lines = str8_cut(lines.tail, '\n');
    String8Cut fields = str8_cut(str8_trim_right(lines.head), ' ');
    String8 kind = fields.head;
    if(str8_match(kind, str8_lit("v"), 0))
    {
      m.mesh.verts[m.mesh.verts_count] = obj_parse_v3f32(fields.tail);
      m.mesh.verts_count += 1;
    }
    else if(str8_match(kind, str8_lit("vn"), 0))
    {
      m.mesh.norms[m.mesh.norms_count] = obj_parse_v3f32(fields.tail);
      m.mesh.norms_count += 1;
    }
    else if(str8_match(kind, str8_lit("vt"), 0))
    {
      m.mesh.textures[m.mesh.textures_count] = obj_parse_v3f32(fields.tail);
      m.mesh.textures_count += 1;
    }
    else if(str8_match(kind, str8_lit("f"), 0))
    {
      m.mesh.faces[m.mesh.faces_count] = obj_parse_face(fields.tail, m.mesh.verts_count, 
                                                        m.mesh.norms_count, m.mesh.textures_count);
      m.mesh.faces_count += 1;
      m.object.faces_count += 1;
      m.group.faces_count += 1;
    }
    else if(str8_match(kind, str8_lit("o"), 0))
    {
      if(m.object.faces_count > 0)
      {
        m.mesh.objects[m.mesh.objects_count] = m.object;
        m.mesh.objects_count += 1;
      }
      m.object = obj_parse_object(fields.tail);
      m.object.face_offset = m.mesh.faces_count;
    }
    else if(str8_match(kind, str8_lit("g"), 0))
    {
      if(m.group.faces_count > 0)
      {
        m.mesh.groups[m.mesh.groups_count] = m.group;
        m.mesh.groups_count += 1;
      }
      m.group = obj_parse_group(fields.tail);
    }
    else if(str8_match(kind, str8_lit("mtllib"), 0))
    {
      m.mesh.materials_count += obj_parse_mtllib(arena, fields.tail, m.mesh.materials);
    }
  }
  
  return m;
}

function GLVertexArray
gl_vertex_array_from_obj(Arena *arena, OBJ model)
{
  GLVertexArray result = {0};
  
  result.data = push_array(arena, GLVertex, model.mesh.faces_count*3);
  S64 count = 0;
  B32 have_normals = (model.mesh.norms_count != 0);
  B32 have_textures = (model.mesh.textures_count != 0);
  for(S64 f = 0; f < model.mesh.faces_count; f += 1)
  {
    B32 valid = 1;
    for(S32 i = 0; i < 3; i += 1)
    {
      valid &= (model.mesh.faces[f].v[i]>0 && model.mesh.faces[f].v[i]<=model.mesh.verts_count);
      if(have_normals)
      {
        valid &= (model.mesh.faces[f].n[i]>0 && model.mesh.faces[f].n[i]<=model.mesh.norms_count);
      }
      if(have_textures)
      {
        valid &= (model.mesh.faces[f].t[i]>=0 && model.mesh.faces[f].t[i]<=model.mesh.textures_count);
      }
    }
    
    if(valid)
    {
      for(S64 i = 0; i < 3; i += 1)
      {
        result.data[count].norm = model.mesh.norms[model.mesh.faces[f].n[i]-1];
        result.data[count].pos = model.mesh.verts[model.mesh.faces[f].v[i]-1];
        result.data[count].text = model.mesh.textures[model.mesh.faces[f].t[i]-1];
        count += 1;
      }
    }
  }
  
  result.count = count;
  return result;
}

function Mesh
mesh_from_obj_model(Arena *arena, OBJ model)
{
  GLVertexArray vertices = gl_vertex_array_from_obj(arena, model);
  GLuint vao = 0;
  GLuint vbo = 0;
  
  glEnable(GL_DEPTH_TEST);
  
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  
  glBindVertexArray(vao);
  
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.count*sizeof(GLVertex), vertices.data, GL_STATIC_DRAW);
  
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLVertex), (void*)OffsetOfMember(GLVertex, pos));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLVertex), (void*)OffsetOfMember(GLVertex, norm));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GLVertex), (void*)OffsetOfMember(GLVertex, text));
  glEnableVertexAttribArray(2);
  
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  
  Mesh result = {0};
  result.vertices = vertices;
  result.vao = vao;
  result.vbo = vbo;
  return result;
}

function void 
mesh_draw(Mesh mesh)
{
  glBindVertexArray(mesh.vao);
  glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(mesh.vertices.count));
  glBindVertexArray(0);
}
