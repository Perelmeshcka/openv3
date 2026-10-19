#include <GLUT/glut.h>   // macOS. На Linux/Windows: <GL/glut.h>
#include <stdlib.h>

#define SW 160
#define SH 120
#define PIXEL_SCALE 6

int tick = 0;

void pixel(int x, int y, int r, int g, int b) {
  glColor3ub(r, g, b);
  glVertex2i(x,     y);
  glVertex2i(x + 1, y);
  glVertex2i(x + 1, y + 1);
  glVertex2i(x,     y + 1);
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT);

  glBegin(GL_QUADS);

  for (int y = 0; y < SH; y++) {
    for (int x = 0; x < SW; x++) {
      pixel(x, y, x * 255 / SW, y * 255 / SH, 128);
    }
  }

  pixel(tick % SW, SH / 2, 255, 255, 255);

  glEnd();
  glutSwapBuffers();
}

void keyDown(unsigned char key, int x, int y) {
  if (key == 27) exit(0);
}

void timer(int value) {
  tick++;
  glutPostRedisplay();
  glutTimerFunc(50, timer, 0);  /* 20 fps */
}

int main(int argc, char *argv[]) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(SW * PIXEL_SCALE, SH * PIXEL_SCALE);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("GLUT");

  gluOrtho2D(0, SW, 0, SH);  // координаты в виртуальных пикселях, начало внизу слева

  glutDisplayFunc(display);
  glutKeyboardFunc(keyDown);
  glutTimerFunc(50, timer, 0);

  glutMainLoop();
  return 0;
}
