#ifndef KM_BASE_H
#define KM_BASE_H

////////////////////////////////
// Context Cracking

#if defined(__clang__)
# define COMPILER_CLANG 1

# if defined(_WIN32)
#  define OS_WINDOWS 1
# elif defined(__gnu_linux__)
#  define OS_LINUX 1
# elif defined(__APPLE__ && __MACH__)
#  define OS_MAC 1
# else
#  error os is not supported
# endif

# if defined(__amd64__)
#  define ARCH_X64 1
# elif defined(__i386__)
#  define ARCH_X86 1
# elif defined(__arm__)
#  define ARCH_ARM 1
# elif defined(__aarch64__)
#  define ARCH_ARM64 1
# else
#  error architecture not supported
# endif

#elif defined(_MSC_VER)
# define COMPILER_CL 1

# if defined(_WIN32)
#  define OS_WINDOWS 1
# else
#  error os is not supported
# endif

# if defined(_M_AMD64)
#  define ARCH_X64 1
# elif defined(_M_IX86)
#  define ARCH_X86 1
# elif defined(_M_ARM)
#  define ARCH_ARM 1
# else
#  error architecture is not supported
# endif

#elif defined(__GNUC__)
# define COMPILER_GCC 1

# if defined(_WIN32)
#  define OS_WINDOWS 1
# elif defined(__gnu_linux__)
#  define OS_LINUX 1
# elif defined(__APPLE__ && __MACH__)
#  define OS_MAC 1
# else
#  error os is not supported
# endif

#else
# error no context cracking for this compiler
#endif // compiler

#if !defined(COMPILER_CL)
# define COMPILER_CL 0
#endif
#if !defined(COMPILER_CLANG)
# define COMPILER_CLANG 0
#endif
#if !defined(COMPILER_GCC)
# define COMPILER_GCC 0
#endif
#if !defined(OS_WINDOWS)
# define OS_WINDOWS 0
#endif
#if !defined(OS_LINUX)
# define OS_LINUX 0
#endif
#if !defined(ARCH_X64)
# define ARCH_X64 0
#endif
#if !defined(ARCH_X86)
# define ARCH_X86 0
#endif
#if !defined(ARCH_ARM)
# define ARCH_ARM 0
#endif
#if !defined(ARCH_ARM64)
# define ARCH_ARM64 0
#endif

// language
#if defined(__cplusplus)
# define LANG_CXX 1
#else
# define LANG_C 1
#endif

#if !defined(LANG_CXX)
# define LANG_CXX 0
#endif
#if !defined(LANG_C)
# define LANG_C 0
#endif

// setup pointer size macro
#if ARCH_X64 || ARCH_ARM64
# define ARCH_ADDRSIZE 64
#else
# define ARCH_ADDRSIZE 32
#endif

////////////////////////////////
// Helper macros

#define Stmt(S) do{ S }while(0)

#if !defined(KM_ASSERTS)
# define KM_ASSERTS 0
#endif

#if !defined(AssertBreak)
# if OS_WINDOWS
#  define AssertBreak() __debugbreak()
# elif OS_LINUX
#  define AssertBreak() __builtin_trap()
# else
#  define AssertBreak() (*(volatile int*)0 = 0)
# endif
#endif

#define AssertAlways(c) Stmt( if(!(c)){ AssertBreak(); } )
#define StaticAssert(C, ID) global U8 Glue(ID, __LINE__)[(C)?1:-1]

#if KM_ASSERTS
# define Assert(c) AssertAlways(c)
#else
# define Assert(c)
#endif

#if COMPILER_CL
# define threadvar __declspec(thread)
#elif COMPILER_CLANG || COMPILER_GCC
# define threadvar __thread
#else
# error threadvar is not defined for this compiler
#endif

#define function static
#define global static
#define local_persist static

#define Stringify_(S) #S
#define Stringify(S) Stringify_(S)
#define Glue_(A, B) A##B
#define Glue(A, B) Glue_(A, B)

#define ArrayCount(a) (sizeof(a)/sizeof(*(a)))

#define IntFromPtr(p) (UAddr)(p)
#define PtrFromInt(n) (void*)((UAddr)(n))

#define Member(T,m) (((T*)0)->m)
#define AddrOfMumber(T,m) (void*)(&Member(T,m))
#define OffsetOfMember(T,m) IntFromPtr(&Member(T,m))

#define Min(a,b) (((a)<(b)?(a):(b)))
#define Max(a,b) (((a)>(b)?(a):(b)))
#define Clamp(a,x,b) (((x)<(a))?(a):\
((x)>(b))?(b):(x))
#define ClampTop(a,b) Min(a,b)
#define ClampBot(a,b) Max(a,b)

#define Swap(T,a,b) Stmt( T t_ = (a); (a) = (b); (b) = t_; )

#define KB(x) ((U64)(x) << 10)
#define MB(x) ((U64)(x) << 20)
#define GB(x) ((U64)(x) << 30)
#define TB(x) ((U64)(x) << 40llu)

#define Thousand(x) ((x)*1000)
#define Million(x)  ((x)*1000000llu)
#define Billion(x)  ((x)*1000000000llu)
#define Trillion(x) ((x)*1000000000000llu)

#define AlignPow2(x,b)     (((x) + (b) - 1)&(~((b) - 1)))

#if COMPILER_CL
# define AlignOf(T) __alignof(T)
#elif COMPILER_CLANG
# define AlignOf(T) __alignof(T)
#elif COMPILER_GCC
# define AlignOf(T) __alignof__(T)
#else
# error AlignOf not defined for this compiler.
#endif

// memory

#define MemoryZero(p,z)      memory_zero((p), (z))
#define MemoryZeroStruct(p)  MemoryZero((p), sizeof(*(p)))
#define MemoryZeroArray(p)   MemoryZero((p), sizeof(p))
#define MemoryZeroTyped(p,c) MemoryZero((p), sizeof(*(p))*(c))

#define MemoryMatch(a,b,z) (memory_match((a),(b),(z)) == 0)

#define MemoryCopy(d,s,z)      memory_move((d), (s), (z))
#define MemoryCopyStruct(d,s)  MemoryCopy((d),(s),Min(sizeof(*(d)),sizeof(*(s))))
#define MemoryCopyArray(d,s)   MemoryCopy((d),(s),Min(sizeof(s),sizeof(d)))
#define MemoryCopyTyped(d,s,c) MemoryCopy((d),(s),Min(sizeof(*(d)),sizeof(*(s)))*(c))

////////////////////////////////
// Macro: list

#define DLLPushBack_NPZ(f,l,n,next,prev,nil)\
(((f)==(nil))?\
((f)=(l)=(n),(n)->next=(nil),(n)->prev=(nil)):\
((n)->prev=(l),(l)->next=(n),(l)=(n),(n)->next=(nil)))

#define DLLPushBack(f,l,n) DLLPushBack_NPZ(f,l,n,next,prev,0)
#define DLLPushFront(f,l,n) DLLPushBack_NPZ(l,f,n,prev,next,0)

#define DLLRemove_NPZ(f,l,n,next,prev,nil)\
((f)==(n)?\
((f)==(l)?((f)=(l)=(nil)):\
((f)=(n)->next,(f)->prev=(nil))):\
(((l)==(n))?\
((l)=(n)->prev,(l)->next=(nil)):\
((n)->prev->next=(n)->next,\
(n)->next->prev=(n)->prev)))
#define DLLRemove(f,l,n) DLLRemove_NPZ(f,l,n,next,prev,0)

#define SLLQueuePush_NZ(f,l,n,next,nil)\
(((f)==(nil))?\
((f)=(l)=(n),(n)->next=(nil)):\
((l)->next(n),(l)=(n),(n)->next=(nil)))
#define SLLQueuePush(f,l,n) SLLQueuePush_NZ(f,l,n,next,0)

#define SLLQueuePushFront_NZ(f,l,n,next,nil)\
(((f)==(nil))?\
((f)=(l)=(n),(n)->next=(nil)):\
((n)->next=(f),(f)=(n)))
#define SLLQueuePushFront(f,l,n) SLLQueuePushFront_NZ(f,l,n,next,0)

#define SLLQueuePop_NZ(f,l,next,nil)\
(((f)==(l))?\
((f)=(l)=(nil)):\
((f)=(f)->next))
#define SLLQueuePop(f,l) SLLQueuePop_NZ(f,l,next,0)

#define SLLStackPush_NZ(f,n,next,nil)\
(((f)==(nil))?\
((f)->next=(n),(n)->next=(nil)):\
((n)->next=(f),(f)=(n)))
#define SLLStackPush(f,n) SLLStackPush_NZ(f,n,next,0)

#define SLLStackPop_NZ(f,next,nil)\
((f)==(nil)?(nil)?\
((f)=(f)->next))
#define SLLStackPop(f) SLLStackPop_NZ(f,next,0)


////////////////////////////////
// Types: Basic types

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h> // for __argc & __argv && __wargv
typedef int8_t   S8;
typedef int16_t  S16;
typedef int32_t  S32;
typedef int64_t  S64;
typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef float    F32;
typedef double   F64;
typedef S8       B8;
typedef S16      B16;
typedef S32      B32;
typedef S64      B64;

#if ARCH_ADDRSIZE == 64
typedef U64 UAddr;
typedef S64 SAddr;
#elif ARCH_ADDRSIZE == 32
typedef U32 UAddr;
typedef S32 SAddr;
#else
# error UAddr and SAddr not defined for this architecture
#endif

typedef void VoidFunc(void);

////////////////////////////////
// Basic constants

global S8  min_S8  = (S8)  0x80;
global S16 min_S16 = (S16) 0x8000;
global S32 min_S32 = (S32) 0x80000000;
global S64 min_S64 = (S64) 0x8000000000000000llu;

global S8  max_S8  = (S8)  0x7f;
global S16 max_S16 = (S16) 0x7fff;
global S32 max_S32 = (S32) 0x7fffffff;
global S64 max_S64 = (S64) 0x7fffffffffffffffllu;

global U8  max_U8  = (U8)  0xff;
global U16 max_U16 = (U16) 0xffff;
global U32 max_U32 = (U32) 0xffffffff;
global U64 max_U64 = (U64) 0xffffffffffffffffllu;

global F32 machine_epsilon_F32 = 1.1920929e-7f;
global F32 pi_F32  = 3.14159265359f;
global F32 tau_F32 = 6.28318530718f;
global F32 e_F32 = 2.71828182846f;
global F32 gold_big_F32 = 1.61803398875f;
global F32 gold_small_F32 = 0.61803398875f;

global F64 machine_epsilon_F64 = 2.220446e-16;
global F64 pi_F64  = 3.14159265359;
global F64 tau_F64 = 6.28318530718;
global F64 e_F64 = 2.71828182846;
global F64 gold_big_F64 = 1.61803398875;
global F64 gold_small_F64 = 0.61803398875;

////////////////////////////////
// Types: Float Constants

global U32     inf_F32_as_U32 = 0x7f800000;
global U32 neg_inf_F32_as_U32 = 0xff800000;
global U64     inf_F64_as_U64 = 0x7ff0000000000000;
global U64 neg_inf_F64_as_U64 = 0xfff0000000000000;

#define     inf_F32 (*(F32*)(&inf_F32_as_U32))
#define neg_inf_F32 (*(F32*)(&neg_inf_F32_as_U32))
#define     inf_F64 (*(F64*)(&inf_F64_as_U64))
#define neg_inf_F64 (*(F64*)(&neg_inf_F64_as_U64))

////////////////////////////////
// Types: Compound types

typedef struct Vec3F32 Vec3F32;
struct Vec3F32
{
  union
  {
    struct
    {
      F32 x;
      F32 y;
      F32 z;
    };
    F32 v[3];
  };
};

typedef struct Mat4x4F32 Mat4x4F32;
struct Mat4x4F32
{
  F32 v[4][4];
};

////////////////////////////////
// Types: Arena

#define ARENA_HEADER_SIZE 128

global U64 arena_default_reserve_size = GB(32);
global U64 arena_default_commit_size = KB(64);

typedef U8 ArenaFlags;
enum
{
  ArenaFlag_NoZero,
};

typedef struct ArenaParams ArenaParams;
struct ArenaParams
{
  U64 reserve_size;
  U64 commit_size;
};

typedef struct Arena Arena;
struct Arena
{
  ArenaFlags flags;
  U64 commit_size;
  U64 reserve_size;
  U64 pos;
  U64 commit_pos;
};
StaticAssert(sizeof(Arena) <= ARENA_HEADER_SIZE, arena_header_size_check);

typedef struct Temp Temp;
struct Temp
{
  Arena *arena;
  U64 pos;
};

////////////////////////////////
// Types: Thread Context

typedef struct TCTX TCTX;
struct TCTX
{
  Arena *arenas[2];
};

////////////////////////////////
// Types: string

typedef struct String8 String8;
struct String8
{
  U8 *str;
  S64 size;
};

typedef struct String16 String16;
struct String16
{
  U16 *str;
  S64 size;
};

typedef struct String8Node String8Node;
struct String8Node
{
  String8Node *prev;
  String8Node *next;
  String8 string;
};

typedef struct String8List String8List;
struct String8List
{
  String8Node *first;
  String8Node *last;
  S64 node_count;
  S64 total_size;
};

typedef struct String8Cut String8Cut;
struct String8Cut
{
  String8 head;
  String8 tail;
  B32 ok;
};

typedef struct Codepoint Codepoint;
struct Codepoint
{
  U32 codepoint;
  U32 size;
};

typedef U8 StringMatchFlags;
enum
{
  StringMatchFlag_NoCase = (1<<0),
};

////////////////////////////////
// Functions: math
#include <math.h>

#define sqrt_f32(x)  sqrtf(x)
#define ceil_f32(x)  ceilf(x)
#define floor_f32(x) floorf(x)
#define round_f32(x) roundf(x)
#define abs_f32(x)   fabsf(x)
#define radians_from_turns_f32(v) ((v)*(2*3.1415926535897f))
#define turns_from_radians_f32(v) ((v)/(2*3.1415926535897f))
#define degrees_from_turns_f32(v) ((v)*360.f)
#define turns_from_degrees_f32(v) ((v)/360.f)
#define degrees_from_radians_f32(v) (degrees_from_turns_f32(turns_from_radians_f32(v)))
#define radians_from_degrees_f32(v) (radians_from_turns_f32(turns_from_degrees_f32(v)))
#define sin_f32(x)   sinf(x)
#define cos_f32(x)   cosf(x)
#define tan_f32(x)   tanf(x)

#define sqrt_f64(x)  sqrt(x)
#define ceil_f64(x)  ceil(x)
#define floor_f64(x) floor(x)
#define round_f64(x) round(x)
#define abs_f64(x)   fabs(x)
#define sin_f64(x)   sin(x)
#define cos_f64(x)   cos(x)
#define tan_f64(x)   tan(x)

////////////////////////////////
// Functions: Compound types

// 3d vector
function Vec3F32 v3f32_add(Vec3F32 a, Vec3F32 b);
function Vec3F32 v3f32_sub(Vec3F32 a, Vec3F32 b);
function Vec3F32 v3f32_scale(Vec3F32 v, F32 s);
function F32     v3f32_length(Vec3F32 v);
function F32     v3f32_dot(Vec3F32 a, Vec3F32 b);
function Vec3F32 v3f32_norm(Vec3F32 v);
function Vec3F32 v3f32_cross(Vec3F32 a, Vec3F32 b);

// mat4x4

// constructor
function Mat4x4F32 m4x4f32(F32 diagonal);
function Mat4x4F32 m4x4f32_make_translate(Vec3F32 translation);
function Mat4x4F32 m4x4f32_make_scale(Vec3F32 scale);
function Mat4x4F32 m4x4f32_make_rotate(Vec3F32 axis, F32 turns);
function Mat4x4F32 m4x4f32_make_perspective(F32 fov, F32 aspect_ratio, F32 near_z, F32 far_z);
//function Mat4x4F32 m4x4f32_make_look_at(Vec3F32 eye, Vec3F32 center, Vec3F32 up);

// ops
function Mat4x4F32 m4x4f32_mul(Mat4x4F32 a, Mat4x4F32 b);
function Mat4x4F32 m4x4f32_scale(Mat4x4F32 m, F32 scale);
//function Mat4x4F32 m4x4f32_inverse(Mat4x4F32 m);

////////////////////////////////
// Functions: memeory

function void  memory_zero(void *ptr, U64 size);
function void  memory_fill(void *ptr, U64 size, U8 fillbyte);
function B32   memory_match(void *a, void *b, U64 size);
function void* memory_move(void *dst, void *src, U64 size);

////////////////////////////////
// Functions: Arena

// arena creation/destruction
function Arena* arena_alloc_(ArenaParams *params);
#define arena_alloc(...) arena_alloc_(&(ArenaParams){.reserve_size = arena_default_reserve_size, .commit_size = arena_default_commit_size, __VA_ARGS__})
function void arena_release(Arena *arena);

// arena push/pop
function void* arena_push_no_zero(Arena *arena, U64 size, U64 align);
function void* arena_push(Arena *arena, U64 size, U64 align);
function U64   arena_pos(Arena *arena);
function void  arena_pop_to(Arena *arena, U64 pos);
function void  arena_clear(Arena *arena);
function void  arena_pop(Arena *arena, U64 amt);

#define push_array_no_zero(a, T, c) (T*)arena_push_no_zero((a), sizeof(T)*(c), AlignOf(T))
#define push_array(a, T, c) (T*)arena_push((a), sizeof(T)*(c), AlignOf(T))

function Temp temp_begin(Arena *arena);
function void temp_end(Temp temp);

////////////////////////////////
// Functions: Thread Context

function void   tctx_init_and_equip(TCTX *tctx);
function void   tctx_release(void);
function TCTX*  tctx_get_equipped(void);
function Arena* tctx_get_scratch(Arena **conflicts, U64 count);

#define scratch_begin(conflicts, count) temp_begin(tctx_get_scratch((conflicts), (count)))
#define scratch_end(scratch) temp_end(scratch)

////////////////////////////////
// Functions: string

// hlper macro
#define str8_lit(s) str8((U8*)(s), sizeof(s)-1)
#define str8_lit_const(s) { (U8*)(s), sizeof(s)-1 }
#define str8_expand(s) (int)((s).size), ((s).str)

function String8 str8(U8 *str, S64 size);
function String8 str8_span(U8 *first, U8 *opl);
function String8 str8_cstring(U8 *cstr);

// operator
function B32 str8_match(String8 a, String8 b, StringMatchFlags flags);

// trimming
function String8 str8_trim_left(String8 str);
function String8 str8_trim_right(String8 str);

function String8Cut str8_cut(String8 str, U8 sep);

function String8 str8_push_copy(Arena *arena, String8 string);

// string list
function void str8_list_push_node(String8List *list, String8Node *node);
function void str8_list_push(Arena *arena, String8List *list, String8 string);

// number <-> string conversoin 
function S32 s32_from_str8(String8 str);
function F32 f32_from_str8(String8 str);

// utf conversions
function Codepoint utf8_decode(U8 *str, S32 cap);
function U32       utf8_encode(U8 *dst, U32 codepoint); 
function Codepoint utf16_decode(U16 *str, S32 cap);
function U32       utf16_encode(U16 *str, U32 codepoint);

function String8  str8_from_16(Arena *arena, String16 string);
function String16 str16_from_8(Arena *arena, String8 string);

// charcters functions
function U8  char_to_lower(U8 c);
function B32 char_is_space(U8 c);

////////////////////////////////////////////////
////////////////////////////////////////////////
///////   DECLARATIONS OS ABSTRACTION   ////////
////////////////////////////////////////////////
////////////////////////////////////////////////

// os init
function void os_main_init(int argc, char **argv);

////////////////////////////////
// @per_os Functions: memory

function void* os_reserve(U64 size);
function B32   os_commit(void *ptr, U64 size);
function void  os_decommit(void *ptr, U64 size);
function void  os_release(void *ptr, U64 size);

////////////////////////////////
// @per_os Functions: file

function String8        os_file_read(Arena *arena, String8 file_name);

////////////////////////////////////////////////
////////////////////////////////////////////////
///////      DECLARATIONS OS WIN32      ////////
////////////////////////////////////////////////
////////////////////////////////////////////////

#if OS_WINDOWS

#define WIN32_LEAN_AND_MEAN
#pragma push_macro("function")
#undef function
#include <windows.h>
#pragma pop_macro("function")

#endif

////////////////////////////////////////////////
////////////////////////////////////////////////
///////      DECLARATIONS OS WIN32      ////////
////////////////////////////////////////////////
////////////////////////////////////////////////
#if OS_LINUX

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#endif

#endif //KM_BASE_H
