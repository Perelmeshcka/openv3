#pragma once
#include "config.h"

#include <stdbool.h>
#include <stdint.h>

#ifndef min
#define min(x, y) ((x) < (y) ? (x) : (y))
#endif

#ifndef max
#define max(x, y) ((x) > (y) ? (x) : (y))
#endif

typedef double dbl;
typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;

typedef struct vec2 {
  dbl x, y;
} vec2;

typedef struct uv2 {
  u32 x, y;
} uv2;

typedef struct iv2 {
  int x, y;
} iv2;

typedef struct vec3 {
  dbl x, y, z;
} vec3;

typedef struct rgb {
  u8 r, g, b;
} rgb;

typedef struct frame {
  rgb pix[HEIGHT][WIDTH];
  // u32 ybuff[WIDTH][HEIGHT];
} frame;

typedef struct pos {
  dbl x, y, z;
  u32 a, d; /* rotation */
} pos;

typedef struct trg {
  float sin[360];
  float cos[360];
} trg;

void trginit(trg *);

vec3 cam(vec3 absolute, pos campos, trg);
vec2 proj(vec3 relative, u32 focus, u32 width, u32 height);

vec3 through(vec2, vec2);
dbl lval(vec2, vec3);
vec2 cross(vec3, vec3);

typedef struct wall {
  vec2 a, b;  /* bottom x, y */
  dbl zb, zt; /* bottom z, top z */
  rgb col;
} wall;

typedef struct prism {
  vec2 *b; /* bottom x, y */
  rgb *c;
  u32 len;
  vec2 cen; /* center x, y */
  dbl zb, zt;
  rgb tc, bc;
  bool bspused;
} prism;
