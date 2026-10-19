#include "draw.h"
#include "utils.h"

#include <stdlib.h>

void pixel(uv2 p, rgb col, frame *f) {
  if (p.x < WIDTH && p.y < HEIGHT)
    f->pix[p.y][p.x] = col;
}

typedef struct pol3 {
  vec3 *v;
  u32 len;
} pol3;

pol3 clip(pol3 p, u32 near) {
  bool ain, bin;
  vec3 a, b;
  dbl k;
  u32 i;

  pol3 res = {
      .v = (vec3 *)calloc(p.len * 2, sizeof(vec3)),
      .len = 0,
  };

  for (i = 0; i < p.len; ++i) {
    a = p.v[i];
    b = p.v[(i + 1) % p.len];
    ain = a.y >= near;
    bin = b.y >= near;

    if (ain)
      res.v[res.len++] = a;
    if (ain != bin) {
      k = ((dbl)near - a.y) / (b.y - a.y);
      res.v[res.len++] = (vec3){
          .x = a.x + (b.x - a.x) * k,
          .y = near,
          .z = a.z + (b.z - a.z) * k,
      };
    }
  }

  return res;
}

typedef struct pol2 {
  vec2 *v;
  u32 len;
} pol2;

/* true -> OX false -> OY */
static dbl coord(vec2 v, bool axis) { return axis ? v.x : v.y; }

static bool compare(dbl v, dbl u, bool lower) {
  return lower ? v <= u : v >= u;
}

static pol2 clipedge(pol2 p, bool axis, bool lower, dbl val) {
  bool ain, bin;
  vec2 a, b;
  dbl k, oc;
  u32 i;

  pol2 res = {
      .v = (vec2 *)calloc(p.len * 2, sizeof(vec2)),
      .len = 0,
  };

  for (i = 0; i < p.len; ++i) {
    a = p.v[i];
    b = p.v[(i + 1) % p.len];
    ain = compare(coord(a, axis), val, lower);
    bin = compare(coord(b, axis), val, lower);

    if (ain)
      res.v[res.len++] = a;
    if (ain != bin) {
      k = (val - coord(a, axis)) / (coord(b, axis) - coord(a, axis));
      oc = coord(a, !axis) + (coord(b, !axis) - coord(a, !axis)) * k;
      res.v[res.len++] =
          axis ? (vec2){.x = val, .y = oc} : (vec2){.x = oc, .y = val};
    }
  }

  return res;
}

static void clip4edges(pol2 *p) {
  if (p->v == NULL)
    return;

  pol2 c1 = clipedge(*p, false, false, 0);
  free(p->v);
  p->v = NULL;
  p->len = 0;

  pol2 c2 = clipedge(c1, true, false, 0);
  free(c1.v);

  c1 = clipedge(c2, false, true, HEIGHT - 1);
  free(c2.v);

  c2 = clipedge(c1, true, true, WIDTH - 1);
  free(c1.v);

  if (c2.len > 2) {
    p->len = c2.len;
    p->v = (vec2 *)calloc(p->len, sizeof(vec2));
    for (u32 i = 0; i < p->len; ++i)
      p->v[i] = c2.v[i];
  }

  free(c2.v);

  return;
}

typedef struct brez {
  uv2 b, e, p;
  int dx, dy, sx, sy;
  int err;
} brez;

static void brezinit(brez *d, uv2 b, uv2 e) {
  d->p = b;
  d->b = b;
  d->e = e;
  d->dx = abs((int)b.x - (int)e.x);
  d->dy = -abs((int)b.y - (int)e.y);
  d->sx = b.x < e.x ? 1 : -1;
  d->sy = b.y < e.y ? 1 : -1;
  d->err = d->dx + d->dy;
}

static bool brezstep(brez *d) {
  if (d->p.x == d->e.x && d->p.y == d->e.y)
    return false;
  int er2 = d->err << 1;
  if (er2 >= d->dy) {
    d->err += d->dy;
    d->p.x += d->sx;
  }
  if (er2 <= d->dx) {
    d->err += d->dx;
    d->p.y += d->sy;
  }
  return true;
}

static void drawpoly(pol3 p, rgb col, frame *f) {
  pol2 prod = {.v = NULL, .len = 0};
  pol3 pclip = clip(p, 1);
  if (pclip.len <= 2)
    goto end;

  u32 len = pclip.len;
  prod.v = (vec2 *)calloc(len, sizeof(vec2));
  prod.len = len;
  for (u32 i = 0; i < len; ++i)
    prod.v[i] = proj(pclip.v[i], 200, WIDTH, HEIGHT);
  clip4edges(&prod);
  len = prod.len;

  if (len <= 2)
    goto end;

  uv2 *pro = (uv2 *)calloc(len, sizeof(uv2));
  for (u32 i = 0; i < len; ++i)
    pro[i] = (uv2){
        (u32)max(0.0, min(prod.v[i].x, WIDTH - 1.0)),
        (u32)max(0.0, min(prod.v[i].y, HEIGHT - 1.0)),
    };

  brez d;
  u32 minx[HEIGHT];
  u32 maxx[HEIGHT];
  for (u32 y = 0; y < HEIGHT; ++y) {
    minx[y] = WIDTH;
    maxx[y] = WIDTH;
  }

  for (u32 i = 0; i < len; ++i) {
    brezinit(&d, pro[i], pro[(i + 1) % len]);
    do {
      minx[d.p.y] = min(minx[d.p.y], d.p.x);
      if (maxx[d.p.y] == WIDTH || maxx[d.p.y] < d.p.x)
        maxx[d.p.y] = d.p.x;
    } while (brezstep(&d));
  }

  u32 x, y;
  for (y = 0; y < HEIGHT; ++y)
    for (x = minx[y]; x <= maxx[y]; ++x)
      pixel((uv2){x, y}, col, f);

  free(pro);
end:
  free(prod.v);
  free(pclip.v);
}

void drawwall(wall w, pos c, frame *f, trg T) {
  vec3 v[4] = {
      cam((vec3){w.a.x, w.a.y, w.zb}, c, T),
      cam((vec3){w.b.x, w.b.y, w.zb}, c, T),
      cam((vec3){w.b.x, w.b.y, w.zt}, c, T),
      cam((vec3){w.a.x, w.a.y, w.zt}, c, T),
  };
  drawpoly((pol3){v, 4}, w.col, f);
}

bool rgbequal(rgb c1, rgb c2) {
  return (c1.r == c2.r && c1.g == c2.g && c1.b == c2.b);
}

void fillframe(frame *f, rgb col) {
  rgb *p = f->pix[0];
  for (size_t i = 0; i < WIDTH * HEIGHT; ++i)
    p[i] = col;
}
