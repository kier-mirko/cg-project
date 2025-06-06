////////////////////////////////////////////////
////////////////////////////////////////////////
/////////     BASE IMPLEMENTATION     //////////
////////////////////////////////////////////////
////////////////////////////////////////////////


////////////////////////////////
// Functions: math

// 3d vector
function Vec3F32
v3f32(F32 x, F32 y, F32 z)
{
  Vec3F32 v = {x, y, z};
  return v;
}

function Vec3F32 
v3f32_add(Vec3F32 a, Vec3F32 b)
{
  Vec3F32 v = {a.x+b.x, a.y+b.y, a.z+b.z};
  return v;
}

function Vec3F32 
v3f32_sub(Vec3F32 a, Vec3F32 b)
{
  Vec3F32 v = {a.x-b.x, a.y-b.y, a.z-b.z};
  return v;
}

function Vec3F32 
v3f32_scale(Vec3F32 v, F32 s)
{
  Vec3F32 result = {v.x*s, v.y*s, v.z*s};
  return result;
}

function F32
v3f32_length(Vec3F32 v)
{
  F32 l = sqrt_f32(v.x*v.x + v.y*v.y + v.z*v.z);
  return l;
}

function F32     
v3f32_dot(Vec3F32 a, Vec3F32 b)
{
  F32 x = a.x*b.x + a.y*b.y + a.z*b.z;
  return x;
}

function Vec3F32
v3f32_norm(Vec3F32 v)
{
  return v3f32_scale(v, 1.f/v3f32_length(v));
}

function Vec3F32
v3f32_cross(Vec3F32 a, Vec3F32 b)
{
  Vec3F32 result = 
  {
    a.y*b.z - a.z*b.y,
    a.z*b.x - a.x*b.z,
    a.x*b.y - a.y*b.x,
  };
  return result;
}

// mat4x4

function Mat4x4F32
m4x4f32(F32 d)
{
  Mat4x4F32 m = {0};
  m.v[0][0] = d;
  m.v[1][1] = d;
  m.v[2][2] = d;
  m.v[3][3] = d;
  return m;
}

function Mat4x4F32
m4x4f32_make_translate(Vec3F32 delta)
{
  Mat4x4F32 m = m4x4f32(1.f);
  m.v[3][0] = delta.x;
  m.v[3][1] = delta.y;
  m.v[3][2] = delta.z;
  return m;
}

function Mat4x4F32
m4x4f32_make_scale(Vec3F32 scale)
{
  Mat4x4F32 m = m4x4f32(1.f);
  m.v[0][0] = scale.x;
  m.v[1][1] = scale.y;
  m.v[2][2] = scale.z;
  return m;
}

function Mat4x4F32 
m4x4f32_make_rotate(Vec3F32 axis, F32 turns)
{
  Mat4x4F32 m = m4x4f32(1.f);
  axis = v3f32_norm(axis);
  F32 sin_theta = sin_f32(turns);
  F32 cos_theta = cos_f32(turns);
  F32 cos_value = 1.f - cos_theta;
  m.v[0][0] = (axis.x * axis.x * cos_value) + cos_theta;
  m.v[0][1] = (axis.x * axis.y * cos_value) - (axis.z * sin_theta);
  m.v[0][2] = (axis.x * axis.z * cos_value) + (axis.y * sin_theta);
  m.v[1][0] = (axis.y * axis.x * cos_value) + (axis.z * sin_theta);
  m.v[1][1] = (axis.y * axis.y * cos_value) + cos_theta;
  m.v[1][2] = (axis.y * axis.z * cos_value) - (axis.x * sin_theta);
  m.v[2][0] = (axis.z * axis.x * cos_value) - (axis.y * sin_theta);
  m.v[2][1] = (axis.z * axis.y * cos_value) + (axis.x * sin_theta);
  m.v[2][2] = (axis.z * axis.z * cos_value) + cos_theta;
  return m;
}

function Mat4x4F32
m4x4f32_make_perspective(F32 fov, F32 aspect, F32 near_z, F32 far_z)
{
  Mat4x4F32 m = {0};
  
  F32 tan_theta_over_2 = tan_f32(fov / 2.0f * pi_F32 / 180.0f);
  m.v[0][0] = 1.0f / (aspect * tan_theta_over_2);
  m.v[1][1] = 1.0f / tan_theta_over_2;
  m.v[2][2] = -(far_z + near_z) / (far_z - near_z);
  m.v[2][3] = -1.0f;
  m.v[3][2] = -(2.0f * near_z * far_z) / (far_z - near_z);
  return m;
}


// ops
function Mat4x4F32 
m4x4f32_mul(Mat4x4F32 a, Mat4x4F32 b)
{
  Mat4x4F32 c = {0};
  for(S32 i = 0; i < 4; i += 1)
  {
    for(S32 j = 0; j < 4; j += 1)
    {
      c.v[i][j] = (a.v[i][0]*b.v[0][j] +
                   a.v[i][1]*b.v[1][j] +
                   a.v[i][2]*b.v[2][j] +
                   a.v[i][3]*b.v[3][j]);
    }
  }
  return c;
}

function Mat4x4F32
m4x4f32_scale(Mat4x4F32 m, F32 scale)
{
  for(S32 j = 0; j < 4; j += 1)
  {
    for(S32 i = 0; i < 4; i += 1)
    {
      m.v[i][j] *= scale;
    }
  }
  return m;
}

////////////////////////////////
// Functions: memory

function void
memory_zero(void *ptr, U64 size)
{
  U64 z64 = size/8;
  U64 z8 = size%8;
  
  U64 *p64 = (U64*)ptr;
  for(;z64 > 0;)
  {
    *p64 = 0;
    p64 += 1;
    z64 -= 1;
  }
  U8 *p8 = (U8*)p64;
  for(;z8 > 0;)
  {
    *p8 = 0;
    p8 += 1;
    z8 -= 1;
  }
}

function void
memory_fill(void *ptr, U64 size, U8 fillbyte)
{
  U64 fillqword = fillbyte;
  {
    fillqword |= fillqword << 8;
    fillqword |= fillqword << 16;
    fillqword |= fillqword << 32;
  }
  
  U64 z64 = size/8;
  U64 z8 = size%8;
  
  U64 *p64 = (U64*)ptr;
  for(;z64 > 0;)
  {
    *p64 = fillqword;
    p64 += 1;
    z64 -= 1;
  }
  U8 *p8 = (U8*)p64;
  for(; z8 > 0;)
  {
    *p8 = fillbyte;
    p8 += 1;
    z8 -= 1;
  }
}

function B32
memory_match(void *a, void *b, U64 size)
{
  U64 z64 = size/8;
  U64 z8 = size%8;
  
  U64 *a64 = (U64*)a;
  U64 *b64 = (U64*)b;
  for(;z64 > 0;)
  {
    if(*a64 != *b64)
    {
      return 0;
    }
    a64 += 1;
    b64 += 1;
    z64 -= 1;
  }
  U8 *a8 = (U8*)a64;
  U8 *b8 = (U8*)b64;
  for(;z8 > 0;)
  {
    if(*a8  != *b8)
    {
      return 0;
    }
    a8 += 1;
    b8 += 1;
    z8 -= 1;
  }
  return 1;
}

function void*
memory_move(void *dst, void *src, U64 size)
{
  U64 z64 = size/8;
  U64 z8 = size%8;
  
  // backwards
  if((U8*)src < (U8*)dst)
  {
    U8 *dst8 = (U8*)dst + size;
    U8 *src8 = (U8*)src + size;
    for(;z8 > 0;)
    {
      dst8 -= 1;
      src8 -= 1;
      *dst8 = *src8;
      z8 -= 1;
    }
    U64 *dst64 = (U64*)dst8;
    U64 *src64 = (U64*)src8;
    for(;z64 > 0;)
    {
      dst64 -= 1;
      src64 -= 1;
      *dst64 = *src64;
      z64 -= 1;
    }
  }
  
  // forwards
  else if((U8*)src > (U8*)dst)
  {
    U64 *dst64 = (U64*)dst;
    U64 *src64 = (U64*)src;
    for(;z64 > 0;)
    {
      *dst64 = *src64;
      dst64 += 1;
      src64 += 1;
      z64 -= 1;
    }
    U8 *dst8 = (U8*)dst64;
    U8 *src8 = (U8*)src64;
    for(;z8 > 0;)
    {
      *dst8 = *src8;
      dst8 += 1;
      src8 += 1;
      z8 -= 1;
    }
  }
  
  return dst;
}

////////////////////////////////
// Functions: Arena

function Arena* 
arena_alloc_(ArenaParams *params)
{
  U64 reserve_size = params->reserve_size;
  U64 commit_size = params->commit_size;
  reserve_size = AlignPow2(reserve_size, KB(4));
  commit_size = AlignPow2(commit_size, KB(4));
  
  void *base = os_reserve(reserve_size);
  os_commit(base, commit_size);
  
  Arena *arena = (Arena*)base;
  arena->commit_size = params->commit_size;
  arena->reserve_size = params->reserve_size;
  arena->pos = ARENA_HEADER_SIZE;
  arena->commit_pos = commit_size;
  return arena;
}

function void
arena_release(Arena *arena)
{
  os_release(arena, arena->reserve_size);
}

function void*
arena_push_no_zero(Arena *arena, U64 size, U64 align)
{
  U64 pos_aligned = AlignPow2(arena->pos, align);
  U64 new_pos = pos_aligned + size;
  
  if(arena->commit_pos < new_pos)
  {
    U64 commit_pos_aligned = AlignPow2(new_pos, arena->commit_size);
    U64 commit_pos_clamped = ClampTop(commit_pos_aligned, arena->reserve_size);
    U64 commit_size = commit_pos_clamped - arena->commit_pos;
    U8 *commit_ptr = (U8*)arena + arena->commit_pos;
    os_commit(commit_ptr, commit_size);
    arena->commit_pos = commit_pos_clamped;
  }
  
  void *result = 0;
  if(arena->commit_pos >= new_pos)
  {
    result = (U8*)arena + pos_aligned;
    arena->pos = new_pos;
  }
  
  return result;
}

function void*
arena_push(Arena *arena, U64 size, U64 align)
{
  void *result = arena_push_no_zero(arena, size, align);
  MemoryZero(result, size);
  return result;
}

function U64   
arena_pos(Arena *arena)
{
  return arena->pos;
}

function void  
arena_pop_to(Arena *arena, U64 pos)
{
  U64 new_pos = ClampBot(ARENA_HEADER_SIZE, pos);
  AssertAlways(new_pos <= arena->pos);
  arena->pos = new_pos;
}

function void  
arena_clear(Arena *arena)
{
  arena_pop_to(arena, 0);
}

function void  
arena_pop(Arena *arena, U64 amt)
{
  U64 pos = arena_pos(arena);
  U64 new_pos = pos;
  if(amt < pos)
  {
    new_pos = pos - amt;
  }
  arena_pop_to(arena, new_pos);
}

function Temp 
temp_begin(Arena *arena)
{
  Temp temp = {arena, arena_pos(arena)};
  return temp;
}

function void
temp_end(Temp temp)
{
  arena_pop_to(temp.arena, temp.pos);
}

////////////////////////////////
// Functions: Thread Context

global threadvar TCTX* tctx_threadvar;

function void
tctx_init_and_equip(TCTX *tctx)
{
  MemoryZeroStruct(tctx);
  for(U64 i = 0; i < ArrayCount(tctx->arenas); i += 1)
  {
    tctx->arenas[i] = arena_alloc(.reserve_size = MB(64), .commit_size = KB(64));
  }
  tctx_threadvar = tctx;
}

function void
tctx_release(void)
{
  for(U64 i = 0; i < ArrayCount(tctx_threadvar->arenas); i += 1)
  {
    arena_release(tctx_threadvar->arenas[i]);
  }
}

function TCTX* 
tctx_get_equipped(void)
{
  return tctx_threadvar;
}

function Arena* 
tctx_get_scratch(Arena **conflicts, U64 count)
{
  TCTX *tctx = tctx_get_equipped();
  
  Arena *result = 0;
  for(U64 i = 0; i < ArrayCount(tctx->arenas); i += 1)
  {
    B32 has_conflict = 0;
    for(U64 j = 0; j < count; j += 1)
    {
      if(tctx->arenas[i] == conflicts[j])
      {
        has_conflict = 1;
        break;
      }
    }
    if(!has_conflict)
    {
      result = tctx->arenas[i];
      break;
    }
  }
  return result;
}


////////////////////////////////
// Functions: string

function String8
str8(U8 *str, S64 size)
{
  String8 result = {str, size};
  return result;
}

function String8
str8_span(U8 *first, U8 *opl)
{
  String8 str = {first, (S64)(opl -first)};
  return str;
}

function String8
str8_cstring(U8 *cstr)
{
  U8 *opl = cstr;
  for(;*opl; opl += 1);
  String8 result = str8_span(cstr, opl);
  return result;
}

function B32
str8_match(String8 a, String8 b, StringMatchFlags flags)
{
  B32 result = 0;
  if(a.size == b.size)
  {
    result = 1;
    B32 no_case = (flags & StringMatchFlag_NoCase);
    for(S64 i = 0; i < a.size; i += 1)
    {
      U8 x = a.str[i];
      U8 y = b.str[i];
      if(no_case)
      {
        x = char_to_lower(x);
        y = char_to_lower(y);
      }
      
      if(x != y)
      {
        result = 0;
        break;
      }
    }
  }
  return result;
}

function String8
str8_trim_left(String8 str)
{
  U8 *ptr = str.str;
  U8 *opl = str.str + str.size;
  for(;ptr < opl && char_is_space(*ptr); ptr += 1);
  String8 result = str8_span(ptr, opl);
  return result;
}

function String8
str8_trim_right(String8 str)
{
  U8 *first = str.str;
  U8 *opl = str.str + str.size;
  for(;opl > first && char_is_space(*(opl-1)); opl -= 1);
  String8 result = str8_span(first, opl);
  return result;
}

function String8Cut
str8_cut(String8 str, U8 sep)
{
  String8Cut result = {0};
  U8 *ptr = str.str;
  U8 *opl = str.str + str.size;
  U8 *cut = ptr;
  for(;cut < opl && *cut != sep; cut += 1);
  result.ok = (cut < opl);
  result.head = str8_span(ptr, cut);
  result.tail = str8_span(cut+result.ok, opl);
  return result;
}

function String8
str8_push_copy(Arena *arena, String8 string)
{
  String8 result = {0};
  result.str = push_array(arena, U8, string.size + 1);
  result.size = string.size;
  MemoryCopy(result.str, string.str, string.size);
  result.str[string.size] = 0;
  return result;
}

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

function void 
str8_list_push_node(String8List *list, String8Node *node)
{
  DLLPushBack(list->first, list->last, node);
  list->node_count += 1;
  list->total_size += node->string.size;
}

function void
str8_list_push(Arena *arena, String8List *list, String8 string)
{
  String8Node *node = push_array(arena, String8Node, 1);
  node->string = string;
  str8_list_push_node(list, node);
}

function Codepoint
utf8_decode(U8 *str, S32 cap)
{
  U8 lenghts[] =
  {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0,2,2,2,2,3,3,4,0
  };
  
  U8 mask[] = {0x00, 0x7f, 0x1f, 0x0f, 0x07};
  U8 shift[] = {0, 18, 12, 6, 0};
  
  Codepoint result = {0};
  if(cap > 0)
  {
    result.codepoint = '?';
    result.size = 1;
    U8 l = lenghts[str[0]>>3];
    U32 cp = (str[0] & mask[l]) << 18;
    switch(l)
    {
      case 4: cp |= ((str[3] & 0x3f) << 0);
      case 3: cp |= ((str[2] & 0x3f) << 6);
      case 2: cp |= ((str[1] & 0x3f) << 12);
      default: break;
    }
    
    cp >>= shift[l];
    
    result.codepoint = cp;
    result.size = l;
  }
  
  return result;
}

function U32
utf8_encode(U8 *dst, U32 codepoint)
{
  U32 size = 0;
  if(codepoint < 0x80)
  {
    dst[0] = codepoint;
    size = 1;
  }
  else if(codepoint < 0x800)
  {
    dst[0] = 0xC0 | (codepoint >> 6);
    dst[1] = 0x80 | (codepoint & 0x3F);
    size = 2;
  }
  else if(codepoint < 0x10000)
  {
    dst[0] = 0xE0 | (codepoint >> 12);
    dst[1] = 0x80 | ((codepoint >> 6) & 0x3F);
    dst[2] = 0x80 | (codepoint & 0x3F);
    size = 3;
  }
  else if(codepoint < 0x10FFFF)
  {
    dst[0] = 0xF0 | (codepoint >> 18);
    dst[1] = 0x80 | ((codepoint >> 12) & 0x3F);
    dst[2] = 0x80 | ((codepoint >> 6) & 0x3F);
    dst[3] = 0x80 | (codepoint & 0x3F);
    size = 4;
  }
  else
  {
    dst[0] = '?';
    size = 1;
  }
  return size;
}

function Codepoint
utf16_decode(U16 *str, S32 cap)
{
  Codepoint result = {'?', 1};
  if(str[0] < 0xD800 || str[0] > 0xDFFF)
  {
    result.codepoint = str[0];
    result.size = 1;
  }
  else if(cap >= 2)
  {
    if(0xD800 <= str[0] && str[0] <= 0xDBFF
       && 0xDC00 <= str[1] && str[1] <= 0xDFFF)
    {
      result.codepoint = ((str[0] - 0xD800) << 10) + (str[1] - 0xDC00) + 0x10000;
      result.size = 2;
    }
  }
  return result;
}

function U32
utf16_encode(U16 *str, U32 codepoint)
{
  U32 size = 0;
  if(codepoint < 0x10000)
  {
    str[0] = codepoint;
    size = 1;
  }
  else if(0x10000 <= codepoint && codepoint <= 0x10FFFF)
  {
    codepoint -= 0x10000;
    str[0] = 0xD800 + (codepoint >> 10);
    str[1] = 0xDC00 + (codepoint & 0x3FF);
    size = 2;
  }
  return size;
}


function String8  
str8_from_16(Arena *arena, String16 string)
{
  U8 *memory = push_array(arena, U8, string.size*2 + 1);
  
  U8 *dptr = memory;
  U16 *ptr = string.str;
  U16 *opl = string.str + string.size;
  for(;ptr < opl;)
  {
    Codepoint cp = utf16_decode(ptr, (S32)(opl - ptr));
    U32 size = utf8_encode(dptr, cp.codepoint);
    ptr += cp.size;
    dptr += size;
  }
  *dptr = 0;
  S64 alloc_count = string.size*2 + 1;
  S64 string_count = (S64)(dptr - memory);
  S64 unused_count = alloc_count - string_count;
  arena_pop(arena, unused_count*sizeof(U8));
  
  String8 result = {memory, string_count};
  return result;
}

function String16
str16_from_8(Arena *arena, String8 string)
{
  U16*memory = push_array(arena, U16, string.size*2 + 1);
  
  U16 *dptr = memory;
  U8 *ptr = string.str;
  U8 *opl = string.str + string.size;
  for(;ptr < opl;)
  {
    Codepoint cp = utf8_decode(ptr, (S32)(opl - ptr));
    U32 size = utf16_encode(dptr, cp.codepoint);
    ptr += cp.size;
    dptr += size;
  }
  *dptr = 0;
  S64 alloc_count = string.size*2 + 1;
  S64 string_count = (S64)(dptr - memory);
  S64 unused_count = alloc_count - string_count;
  arena_pop(arena, unused_count*sizeof(U16));
  
  String16 result = {memory, string_count};
  return result;
}

function U8
char_to_lower(U8 c)
{
  U8 result = c;
  if('A' <= c && c <= 'Z')
  {
    result = c - 'A' + 'a';
  }
  return result;
}

function B32
char_is_space(U8 c)
{
  return(c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\f' || c == '\v');
}


////////////////////////////////////////////////
////////////////////////////////////////////////
/////////   OS IMPLEMENTATION WIN32   //////////
////////////////////////////////////////////////
////////////////////////////////////////////////

#if OS_WINDOWS

////////////////////////////////
// Functions: OS init

global Arena *w32_perm_arena    = 0;
global String8List w32_cmd_line = {0};

function void 
os_main_init(int argc, char **argv)
{
  
  w32_perm_arena = arena_alloc(.reserve_size = GB(24));
  
  for(int i = 0; i < argc; i += 1)
  {
    str8_list_push(w32_perm_arena, &w32_cmd_line, str8_cstring(argv[i]));
  }
  
  local_persist TCTX tctx;
  tctx_init_and_equip(&tctx);
}

////////////////////////////////
// Functions: memory

function void*
os_reserve(U64 size)
{
  void *ptr = VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
  return ptr;
}

function B32
os_commit(void *ptr, U64 size)
{
  B32 result = (VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != 0);
  return result;
}

function void
os_decommit(void *ptr, U64 size)
{
  VirtualFree(ptr, size, MEM_DECOMMIT);
}

function void
os_release(void *ptr, U64 size)
{
  VirtualFree(ptr, 0, MEM_RELEASE);
}

////////////////////////////////
// Functions: file

function String8
os_file_read(Arena *arena, String8 filename)
{
  String8 result = {0};
  Temp scratch = scratch_begin(&arena, 1);
  String16 filename16 = str16_from_8(scratch.arena, filename);
  HANDLE handle = CreateFileW((WCHAR*)filename16.str, GENERIC_READ, FILE_SHARE_READ, 0, 
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
  if(handle != INVALID_HANDLE_VALUE)
  {
    LARGE_INTEGER file_size;
    BOOL file_size_result = GetFileSizeEx(handle, &file_size);
    U64 total_size = file_size.QuadPart;
    
    Temp temp = temp_begin(arena);
    U8 *buffer = push_array(arena, U8, total_size);
    
    U8 *ptr = buffer;
    U8 *opl = ptr + total_size;
    B32 success = 1;
    for(;ptr < opl;)
    {
      U64 total_to_read = (U64)(opl - ptr);
      U32 to_read = total_to_read > max_U32 ? max_U32 : (U32)total_to_read;
      U32 bytes_read = 0;
      if(!ReadFile(handle, ptr, to_read, &bytes_read, 0))
      {
        success= 0;
        break;
      }
      ptr += bytes_read;
    }
    
    if(success)
    {
      *ptr = 0;
      result.str = buffer;
      result.size = total_size;
    }
    else
    {
      temp_end(temp);
    }
  }
  CloseHandle(handle);
  scratch_end(scratch);
  return result;
}

#endif


////////////////////////////////////////////////
////////////////////////////////////////////////
/////////   OS IMPLEMENTATION LINUX   //////////
////////////////////////////////////////////////
////////////////////////////////////////////////

#if OS_LINUX

global Arena *lnx_perm_arena = {0};
global String8List lnx_cmd_line = {0};

function void
os_main_init(int argc, char **argv)
{
  lnx_perm_arena = arena_alloc(.reserve_size = GB(24));
  
  for(int i = 0; i < argc; i += 1)
  {
    str8_list_push(lnx_perm_arena, &lnx_cmd_line, str8_cstring(argv[i]));
  }
  
  local_persist TCTX tctx;
  tctx_init_and_equip(&tctx);
}

////////////////////////////////
// Functions: memory

function void*
os_reserve(U64 size)
{
  void *ptr = mmap(0, size, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
  return ptr;
}

function B32
os_commit(void *ptr, U64 size)
{
  B32 result = (mprotect(ptr, size, PROT_READ|PROT_WRITE) == 0);
  return result;
}

function void
os_decommit(void *ptr, U64 size)
{
  madvise(ptr, size, MADV_DONTNEED);
  mprotect(ptr, size, PROT_NONE);
}

function void
os_release(void *ptr, U64 size)
{
  munmap(ptr, size);
}

////////////////////////////////
// functions: file

function String8
os_file_read(Arena *arena, String8 file_name)
{
  String8 result = {0};
  Temp scratch = scratch_begin(&arena, 1);
  file_name = str8_push_copy(scratch.arena, file_name);
  
  int fd = open(file_name.str, O_RDONLY, 0);
  if(fd != -1)
  {
    struct stat statbuf = {0};
    fstat(fd, &statbuf);
    U64 total_size = statbuf.st_size;
    
    Temp temp = temp_begin(arena);
    U8 *buffer = push_array(arena, U8, total_size);
    
    U8 *ptr = buffer;
    U8 *opl = buffer + total_size;
    B32 success = 1;
    for(;ptr < opl;)
    {
      U64 total_to_read = (U64)(opl - ptr);
      U32 to_read = total_to_read > max_U32 ? max_U32 : (U32)total_to_read;
      U32 bytes_read = read(fd, ptr, to_read);
      if(bytes_read == -1)
      {
        success = 0;
        break;
      }
      ptr += bytes_read;
    }
    
    if(success)
    {
      result.str = buffer;
      result.size = total_size;
    }
    else
    {
      temp_end(temp);
    }
  }
  scratch_end(scratch);
  return result;
}

#endif
