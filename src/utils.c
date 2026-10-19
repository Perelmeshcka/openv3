#include "utils.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define EPS 1e-9

void trginit(trg *t) {
  for (u32 deg = 0; deg < 360; ++deg) {
    t->sin[deg] = sin(M_PI * deg / 180);
    t->cos[deg] = cos(M_PI * deg / 180);
  }
}

vec3 cam(vec3 a, pos c, trg T) {
  a.x -= c.x;
  a.y -= c.y;
  a.z -= c.z;

  dbl x = a.x, y = a.y, z = a.z;

  x = a.x * T.cos[c.a] - a.y * T.sin[c.a];
  y = a.x * T.sin[c.a] + a.y * T.cos[c.a];

  a.x = x;
  a.y = y;

  y = a.y * T.cos[c.d] + a.z * T.sin[c.d];
  z = -a.y * T.sin[c.d] + a.z * T.cos[c.d];

  a.y = y;
  a.z = z;

  return a;
}

vec2 proj(vec3 r, u32 f, u32 w, u32 h) {
  dbl x = (dbl)(w >> 1) + f * r.x / r.y;
  dbl y = (dbl)(h >> 1) - f * r.z / r.y;

  return (vec2){x, y};
}

vec2 center(vec2 *p, u32 len) {
  vec2 res = {.x = 0, .y = 0};

  for (u32 i = 0; i < len; ++i) {
    res.x += p[i].x;
    res.y += p[i].y;
  }
  res.x /= len;
  res.y /= len;

  return res;
}

vec3 through(vec2 v, vec2 u) {
  return (vec3){
    .x = u.y - v.y,
    .y = v.x - u.x,
    .z = v.y * u.x - v.x * u.y,
  };
}

dbl lval(vec2 v, vec3 l) {
  return v.x * l.x + v.y * l.y + l.z;
}

vec2 cross(vec3 l, vec3 k) {
  dbl det = l.x * k.y - l.y * k.x;

  if (fabs(det) < EPS) {
    fputs("0 DIV", stderr);
    return (vec2){0, 0};
  }

  return (vec2){
    .x = (l.y * k.z - l.z * k.y) / det,
    .y = (l.z * k.x - l.x * k.z) / det,
  };
}

