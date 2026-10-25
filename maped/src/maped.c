#include "config.h"

#include <raylib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef double dbl;

typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;

typedef struct vec2 {
  int x, y;
} vec2;

typedef struct rgb {
  u8 r, g, b;
} rgb;

typedef struct prismvals {
  u32 len;
  vec2 *top;
  rgb *col;
  int zt, zb;
  rgb ct, cb;
} prismvals;

u32 nprisms;
prismvals *all;
prismvals cur;
int scale = 2;
vec2 shift = {0, 0};

void clean(void) {
  cur = (prismvals){
      .len = 0,
      .top = (vec2 *)calloc(MAXVERT, sizeof(vec2)),
      .col = (rgb *)calloc(MAXVERT, sizeof(rgb)),
      .zt = 100,
      .zb = 0,
      .ct = (rgb){0, 158, 47},
      .cb = (rgb){190, 33, 55},
  };
}

vec2 unpos(Vector2 v) {
  return (vec2){(v.x - shift.x) / scale, (HEIGHT - v.y + shift.y) / scale};
}

void addv(Vector2 v) {
  cur.top[cur.len] = unpos(v);
  cur.col[cur.len] = (rgb){253, 249, 0};
  ++cur.len;
}

void flush(void) {
  if (cur.len > 2) {
    all[nprisms++] = cur;
    clean();
  }
}

void keyboard(void) {
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    addv(GetMousePosition());
  if (IsKeyPressed(KEY_ENTER))
    flush();

  if (IsKeyPressed(KEY_V))
    ++scale;
  if (IsKeyPressed(KEY_C))
    --scale;

  if (IsKeyDown(KEY_UP))
    shift.y += 20;
  if (IsKeyDown(KEY_DOWN))
    shift.y -= 20;
  if (IsKeyDown(KEY_LEFT))
    shift.x += 20;
  if (IsKeyDown(KEY_RIGHT))
    shift.x -= 20;
}

Vector2 pos(vec2 v) {
  return (Vector2){v.x * scale + shift.x, HEIGHT - v.y * scale + shift.y};
}

Color col(rgb c) { return (Color){c.r, c.g, c.b, 255}; }

vec2 center(vec2 *p, u32 len) {
  vec2 res = {0, 0};
  for (u32 i = 0; i < len; ++i) {
    res.x += p[i].x;
    res.y += p[i].y;
  }
  res.x /= len;
  res.y /= len;

  return res;
}

void drawprism(prismvals p) {
  for (u32 i = 0; i < p.len; ++i)
    DrawLineV(pos(p.top[i]), pos(p.top[(i + 1) % p.len]), col(p.col[i]));
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fputs("Usage: maped <file>", stderr);
    return 1;
  }

  InitWindow(WIDTH, HEIGHT, TITLE);
  SetTargetFPS(FPS);

  all = (prismvals *)calloc(MAXPRISMS, sizeof(prismvals));
  clean();
  char sign[] = "1";
  Vector2 cen;

  for (; !WindowShouldClose();) {
    keyboard();

    BeginDrawing();

    ClearBackground(DARKBLUE);
    for (u32 i = 0; i < nprisms; ++i) {
      drawprism(all[i]);
      sign[0] = i + '0';
      cen = pos(center(all[i].top, all[i].len));
      DrawText(sign, cen.x, cen.y, 19, WHITE);
    }

    if (cur.len > 0) {
      addv(GetMousePosition());
      drawprism(cur);
      --cur.len;
    }

    EndDrawing();
  }

  FILE *f = fopen(argv[1], "wb");

  fprintf(f, "%d\n", nprisms);
  u32 i, j;
  for (i = 0; i < nprisms; ++i) {
    fprintf(f, "%d\n", all[i].len);
    for (j = 0; j < all[i].len; ++j)
      fprintf(f, "%d %d\n", all[i].top[j].x, all[i].top[j].y);
    for (j = 0; j < all[i].len; ++j)
      fprintf(f, "%c%c%c", all[i].col[j].r, all[i].col[j].g, all[i].col[j].b);
    fprintf(f, "\n%d %d\n%c%c%c%c%c%c\n", all[i].zt, all[i].zb, all[i].ct.r,
            all[i].ct.g, all[i].ct.b, all[i].cb.r, all[i].cb.g, all[i].cb.b);
  }

  fclose(f);
  CloseWindow();
  return 0;
}
