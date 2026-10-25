#pragma once

#include "utils.h"

typedef struct prism {
  pol2 top;
  dbl zt, zb;
  rgb ct, cb;
  wall *walls;
  u32 wallnum;
} prism;

typedef struct bspnode {
  vec3 del;
  wall *delim;
  u32 delnum;
  prism *prisms;
  u32 prismnum;
  item *items;
  u32 itemnum;
  void *left;
  void *right;
} bspnode;

void bspinit(bspnode *);
void bspfree(bspnode *);
