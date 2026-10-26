#include "config.h"
#include "draw.h"
#include "utils.h"
#include <assert.h>
#include <stdio.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

frame fd;
u32 tick;
GLuint texture;
trg T;

prism *prisms;
u32 prismnum;
bspnode *map;
pos campos;
tex testt;
item test;

bool keystate[256];

static prism prisminit(vec2 *v, rgb *c, u32 len, dbl zt, dbl zb, rgb ct,
                       rgb cb) {
  prism res = {
      .top =
          {
              .v = (vec2 *)calloc(len, sizeof(vec2)),
              .len = len,
          },
      .zt = zt,
      .zb = zb,
      .ct = ct,
      .cb = cb,
      .walls = (wall *)calloc(len, sizeof(wall)),
      .wallnum = len,
  };

  for (u32 i = 0; i < len; ++i)
    res.top.v[i] = v[i];
  vec2 cen = center(res.top.v, res.top.len);
  for (u32 i = 0; i < len; ++i) {
    res.walls[i] = (wall){
        .a = res.top.v[i],
        .b = res.top.v[(i + 1) % len],
        .zb = zb,
        .zt = zt,
        .col = c[i],
    };
    res.walls[i].leq = through(res.walls[i].a, res.walls[i].b);
    res.walls[i].psv = (sgn(lval(cen, res.walls[i].leq)) <= 0);
  }

  return res;
}

char *path;
void sceneinit(void) {
  testt = (tex){
      .w = 1,
      .h = 2,
      .pix = (rgb[2]){{0, 0, 0}, {0, 255, 0}},
  };
  test = (item){
      .w = 5,
      .h = 10,
      .pos = {0, 200, 10},
      .t = &testt,
  };
  fputs("1", stderr);

  map = malloc(sizeof(bspnode));
  FILE *mapfile = fopen(path, "rb");
  assert(mapfile);

  u32 prnum;
  fscanf(mapfile, "%d", &prnum);
  *map = (bspnode){
      .items = NULL,
      .prisms = (prism *)calloc(prnum, sizeof(prism)),
      .prismnum = prnum,
  };

  u32 n;
  vec2 *top;
  rgb *c;
  int zt, zb, curx, cury;
  rgb ct, cb;

  u32 i, j;
  for (i = 0; i < prnum; ++i) {
    fscanf(mapfile, "%d", &n);
    top = (vec2 *)calloc(n, sizeof(vec2));
    c = (rgb *)calloc(n, sizeof(rgb));

    for (j = 0; j < n; ++j) {
      fscanf(mapfile, "%d %d ", &curx, &cury);
      top[j].x = curx;
      top[j].y = cury;
    }

    for (j = 0; j < n; ++j)
      fscanf(mapfile, "%c%c%c", &c[j].r, &c[j].g, &c[j].b);

    fscanf(mapfile, " %d %d ", &zt, &zb);
    fscanf(mapfile, "%c%c%c", &ct.r, &ct.g, &ct.b);
    fscanf(mapfile, "%c%c%c", &cb.r, &cb.g, &cb.b);

    map->prisms[i] = prisminit(top, c, n, zt, zb, ct, cb);

    free(top);
    free(c);
  }

  fclose(mapfile);

  bspinit(map);

  campos = (pos){
      .x = 0,
      .y = 0,
      .z = 10,
      .a = 0,
      .d = 0,
  };
}

u32 angle(int x) {
  if (x < 0)
    return (u32)(x + 360);
  if (x >= 360)
    return (u32)(x - 360);
  return x;
}

void keyact(u8 key) {
  if (key == 'w') {
    campos.x += 5 * T.sin[campos.a];
    campos.y += 5 * T.cos[campos.a];
  }
  if (key == 's') {
    campos.x -= 5 * T.sin[campos.a];
    campos.y -= 5 * T.cos[campos.a];
  }
  if (key == 'a') {
    campos.x -= 5 * T.cos[campos.a];
    campos.y += 5 * T.sin[campos.a];
  }
  if (key == 'd') {
    campos.x += 5 * T.cos[campos.a];
    campos.y -= 5 * T.sin[campos.a];
  }

  if (key == 'k')
    campos.d = angle((int)campos.d + 1);
  if (key == 'j')
    campos.d = angle((int)campos.d - 1);
  if (key == 'h')
    campos.a = angle((int)campos.a - 1);
  if (key == 'l')
    campos.a = angle((int)campos.a + 1);

  if (key == 'q')
    campos.z -= 5;
  if (key == 'e')
    campos.z += 5;

  if (key == 27) {
    bspfree(map);
    exit(0);
  }
}

void texinit(void) {
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB,
               GL_UNSIGNED_BYTE, NULL);
}

void display() {
  for (u8 k = 'a'; k <= 'z'; ++k)
    if (keystate[k])
      keyact(k);

  frameupd(&fd);
  fillframe(&fd, (rgb){0, 0, 100});
  drawitem(test, campos, &fd, T);
  drawbsp(map, campos, &fd, T);
  // fputs("2", stderr);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WIDTH, HEIGHT, GL_RGB,
                  GL_UNSIGNED_BYTE, fd.pix);

  glColor3ub(255, 255, 255);
  glEnable(GL_TEXTURE_2D);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0);
  glVertex2i(0, 0);
  glTexCoord2f(1, 0);
  glVertex2i(WIDTH, 0);
  glTexCoord2f(1, 1);
  glVertex2i(WIDTH, HEIGHT);
  glTexCoord2f(0, 1);
  glVertex2i(0, HEIGHT);
  glEnd();

  glutSwapBuffers();
}

void timer(int) {
  ++tick;
  if (tick >= 1000)
    tick = 0;
  glutPostRedisplay();
  glutTimerFunc(1000 / FPS, timer, 0);
}

void keydown(u8 key, int, int) {
  if ('A' <= key && key <= 'Z')
    keystate[tolower(key)] = true;
  else
    keystate[key] = true;
}

void keyup(u8 key, int, int) {
  if ('A' <= key && key <= 'Z')
    keystate[tolower(key)] = false;
  else
    keystate[key] = false;
}

int main(int argc, char **argv) {
  if (argc == 1) {
    fputs("! provide map file", stderr);
    return 1;
  }
  path = argv[1];

  tick = 0;
  trginit(&T);

  sceneinit();

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(WIDTH * PIX, HEIGHT * PIX);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(TITLE);

  gluOrtho2D(0, WIDTH, 0, HEIGHT);
  texinit();

  glutDisplayFunc(display);
  glutKeyboardFunc(keydown);
  glutKeyboardUpFunc(keyup);
  glutIgnoreKeyRepeat(1);
  glutTimerFunc(1000 / FPS, timer, 0);

  glutMainLoop();

  return 0;
}
