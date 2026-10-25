#include "bsp.h"
#include "utils.h"
#include <stdlib.h>

static pol2 pol2init(u32 cap) {
  return (pol2){
      .v = (vec2 *)calloc(cap, sizeof(vec2)),
      .len = 0,
  };
}

static void pol2split(pol2 p, vec3 l, pol2 *pos, pol2 *neg) {
  vec2 a, b, m;
  dbl av, bv, t;
  int sa, sb;
  u32 i;

  for (i = 0; i < p.len; ++i) {
    a = p.v[i];
    b = p.v[(i + 1) % p.len];
    av = lval(a, l);
    bv = lval(b, l);
    sa = sgn(av);
    sb = sgn(bv);

    if (sa >= 0)
      pos->v[pos->len++] = a;
    if (sa <= 0)
      neg->v[neg->len++] = a;

    if (sa * sb < 0) {
      t = av / (av - bv);
      m = (vec2){
          .x = a.x + (b.x - a.x) * t,
          .y = a.y + (b.y - a.y) * t,
      };
      pos->v[pos->len++] = m;
      neg->v[neg->len++] = m;
    }
  }
}

static prism prisminit(u32 cap, dbl zb, dbl zt, rgb ct, rgb cb) {
  return (prism){
      .top = pol2init(cap),
      .zb = zb,
      .zt = zt,
      .ct = ct,
      .cb = cb,
      .walls = (wall *)calloc(cap, sizeof(wall)),
      .wallnum = 0,
  };
}

void bspinit(bspnode *node) {
  if (!node)
    return;
  if (!node->prismnum)
    goto end;

  u32 wnum = 0;
  for (u32 pi = 0; pi < node->prismnum; ++pi)
    wnum += node->prisms[pi].wallnum;
  if (!wnum) {
    node->left = NULL;
    node->right = NULL;
    node->delim = NULL;
    node->delnum = 0;
    return;
  }

  wall *walls = (wall *)calloc(wnum, sizeof(wall));
  wnum = 0;
  for (u32 pi = 0; pi < node->prismnum; ++pi)
    for (u32 wi = 0; wi < node->prisms[pi].wallnum; ++wi)
      walls[wnum++] = node->prisms[pi].walls[wi];
  u32 dwi = rand() % wnum;

  wall dw = walls[dwi];
  vec3 del = through(dw.a, dw.b);
  node->del = del;
  node->delnum = 0;
  node->delim = (wall *)calloc(wnum, sizeof(wall));

  prism *left = (prism *)calloc(node->prismnum, sizeof(prism));
  prism *right = (prism *)calloc(node->prismnum, sizeof(prism));
  u32 lnum = 0, rnum = 0;
  prism cur, toaddl, toaddr;

  wall w;
  u32 i, j;
  dbl av, bv, t;
  int sa, sb;
  vec2 m;
  for (i = 0; i < node->prismnum; ++i) {
    cur = node->prisms[i];
    toaddl = prisminit(cur.top.len + 1, cur.zb, cur.zt, cur.ct, cur.cb);
    toaddr = prisminit(cur.top.len + 1, cur.zb, cur.zt, cur.ct, cur.cb);

    pol2split(cur.top, del, &toaddl.top, &toaddr.top);

    for (j = 0; j < cur.wallnum; ++j) {
      w = cur.walls[j];
      av = lval(w.a, del);
      bv = lval(w.b, del);
      sa = sgn(av);
      sb = sgn(bv);

      if (sa == 0 && sb == 0) {
        node->delim[node->delnum++] = w;
        continue;
      }

      if (sa >= 0 && sb >= 0)
        toaddl.walls[toaddl.wallnum++] = w;
      else if (sa <= 0 && sb <= 0)
        toaddr.walls[toaddr.wallnum++] = w;
      else {
        t = av / (av - bv);
        m = (vec2){
            .x = w.a.x + (w.b.x - w.a.x) * t,
            .y = w.a.y + (w.b.y - w.a.y) * t,
        };
        if (sa > 0) {
          toaddl.walls[toaddl.wallnum++] = (wall){
              w.a, m, w.leq, w.psv, w.zb, w.zt, w.col,
          };
          toaddr.walls[toaddr.wallnum++] = (wall){
              m, w.b, w.leq, w.psv, w.zb, w.zt, w.col,
          };
        } else {
          toaddl.walls[toaddl.wallnum++] = (wall){
              m, w.b, w.leq, w.psv, w.zb, w.zt, w.col,
          };
          toaddr.walls[toaddr.wallnum++] = (wall){
              w.a, m, w.leq, w.psv, w.zb, w.zt, w.col,
          };
        }
      }
    }

    if (toaddl.top.len > 2) {
      left[lnum++] = toaddl;
    } else {
      free(toaddl.walls);
      free(toaddl.top.v);
    }
    if (toaddr.top.len > 2) {
      right[rnum++] = toaddr;
    } else {
      free(toaddr.walls);
      free(toaddr.top.v);
    }
  }

  if (lnum) {
    node->left = malloc(sizeof(bspnode));
    bspnode *l = (bspnode *)node->left;
    l->prismnum = lnum;
    l->prisms = left;
    bspinit(node->left);
  } else {
    node->left = NULL;
    free(left);
  }

  if (rnum) {
    node->right = malloc(sizeof(bspnode));
    bspnode *r = (bspnode *)node->right;
    r->prismnum = rnum;
    r->prisms = right;
    bspinit(node->right);
  } else {
    node->right = NULL;
    free(right);
  }
  free(walls);

end:
  for (i = 0; i < node->prismnum; ++i) {
    free(node->prisms[i].walls);
    free(node->prisms[i].top.v);
  }
  free(node->prisms);
  node->prisms = NULL;
  node->prismnum = 0;
}

void bspfree(bspnode *node) {
  if (!node)
    return;
  free(node->delim);
  free(node->items);
  free(node->prisms);
  bspfree(node->left);
  bspfree(node->right);
}
