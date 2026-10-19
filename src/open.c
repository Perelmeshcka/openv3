#include "config.h"
#include "draw.h"
#include "utils.h"

#include <GLUT/glut.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

frame fd;
u32 tick;
GLuint tex;
trg T;

wall *walls;
u32 wallnum;
pos campos;

bool keystate[256];

void sceneinit(void) {
  walls = (wall *)calloc(1, sizeof(wall));
  wallnum = 1;
  walls[0] = (wall){
      .a = {-20, 60},
      .b = {20, 60},
      .zb = 0,
      .zt = 30,
      .col = {0, 255, 0},
  };

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
    campos.d = angle((int)campos.d - 1);
  if (key == 'j')
    campos.d = angle((int)campos.d + 1);
  if (key == 'h')
    campos.a = angle((int)campos.a - 1);
  if (key == 'l')
    campos.a = angle((int)campos.a + 1);

  if (key == 'q')
    campos.z += 5;
  if (key == 'e')
    campos.z -= 5;
  
  if (key == 27)
    exit(0);
}

void texinit(void) {
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
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

  fillframe(&fd, (rgb){0, 0, 100});
  for (u32 i = 0; i < wallnum; ++i)
    drawwall(walls[i], campos, &fd, T);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glBindTexture(GL_TEXTURE_2D, tex);
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
