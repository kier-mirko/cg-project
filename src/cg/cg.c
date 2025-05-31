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

function ObjFace
obj_parse_face(String8 str, S64 verts_count, S64 norms_count, S64 textures_count)
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
    else if(str8_match(kind, str8_lit("vt"), 0))
    {
      m.textures_count += 1;
    }
    else if(str8_match(kind, str8_lit("f"), 0))
    {
      m.faces_count += 1;
    }
  }
  
  m.verts    = push_array(arena, Vec3F32, m.verts_count);
  m.norms    = push_array(arena, Vec3F32, m.norms_count);
  m.textures = push_array(arena, Vec3F32, m.textures_count);
  m.faces    = push_array(arena, ObjFace, m.faces_count);
  m.verts_count = m.norms_count = m.textures_count = m.faces_count = 0;
  
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
    else if(str8_match(kind, str8_lit("vt"), 0))
    {
      m.textures[m.textures_count] = obj_parse_v3f32(fields.tail);
      m.textures_count += 1;
    }
    else if(str8_match(kind, str8_lit("f"), 0))
    {
      m.faces[m.faces_count] = obj_parse_face(fields.tail, m.verts_count, m.norms_count, m.textures_count);
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
  B32 have_normals = (model.norms_count != 0);
  B32 have_textures = (model.textures_count != 0);
  for(S64 f = 0; f < model.faces_count; f += 1)
  {
    B32 valid = 1;
    for(S32 i = 0; i < 3; i += 1)
    {
      valid &= (model.faces[f].v[i]>0 && model.faces[f].v[i]<=model.verts_count);
      if(have_normals)
      {
        valid &= (model.faces[f].n[i]>0 && model.faces[f].n[i]<=model.norms_count);
      }
      if(have_textures)
      {
        valid &= (model.faces[f].t[i]>=0 && model.faces[f].t[i]<=model.textures_count);
      }
    }
    
    if(valid)
    {
      for(S64 i = 0; i < 3; i += 1)
      {
        result.data[count].norm = model.norms[model.faces[f].n[i]-1];
        result.data[count].pos = model.verts[model.faces[f].v[i]-1];
        result.data[count].text = model.textures[model.faces[f].t[i]-1];
        count += 1;
      }
    }
  }
  
  result.count = count;
  return result;
}
