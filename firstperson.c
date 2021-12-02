#include <GL/glut.h>
#include <math.h>
#include <stdbool.h>

static int win_id;
static int win_y, win_x;
static int omx, omy, mx, my;
static int mouse_down[3];

static int g_viewport_width = 0;
static int g_viewport_height = 0;
const float g_rotation_speed = M_PI / 180 * 0.02;
static float c_yaw = 0.0;
static float c_pitch = 0.0;
static float c_x, c_y, c_z;
static float c_lx, c_ly, c_lz;

static void c_Refresh(void) {
  c_lx = cos(c_yaw) * cos(c_pitch);
  c_ly = sin(c_pitch);
  c_lz = sin(c_yaw) * cos(c_pitch);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(c_x, c_y, c_z, c_x + c_lx, c_y + c_ly, c_z + c_lz, 0.0, 1.0, 0.0);
}

static void c_RotatePitch(float angle) {
  const float limit = 89.0 * M_PI / 180.0;

  c_pitch -= angle;

  if (c_pitch < -limit)
    c_pitch = -limit;
  if (c_pitch > limit)
    c_pitch = limit;

  c_Refresh();
}

static void c_SetPos(float x, float y, float z) {
  c_x = x;
  c_y = y;
  c_z = z;

  c_Refresh();
}

static void post_display(void) {
  glFlush();
  glutSwapBuffers();
}

static void pre_display(void) {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static void Grid(void) {
  glPushMatrix();
  glColor3f(1, 1, 1);

  for (int i = -50; i < 50; i++) {
    glBegin(GL_LINES);
    glVertex3f(i, 0, -50);
    glVertex3f(i, 0, 50);
    glEnd();
  }

  for (int i = -50; i < 50; i++) {
    glBegin(GL_LINES);
    glVertex3f(-50, 0, i);
    glVertex3f(50, 0, i);
    glEnd();
  }

  glPopMatrix();
}

static void mouse_func(int button, int state, int x, int y) {
  omx = mx = x;
  omy = my = y;

  mouse_down[button] = state == GLUT_DOWN;
}

static void reshape_func(int w, int h) {
  glutSetWindow(win_id);
  glutReshapeWindow(w, h);
  win_x = w;
  win_y = h;
  g_viewport_width = w;
  g_viewport_height = h;
  glViewport(
      0, 0, (GLsizei)w,
      (GLsizei)h); // set the viewport to the current window specifications
  glMatrixMode(GL_PROJECTION); // set the matrix to projection

  glLoadIdentity();
  gluPerspective(
      60, (GLfloat)w / (GLfloat)h, 0.1,
      100.0); // set the perspective (angle of sight, width, height, ,depth)
  glMatrixMode(GL_MODELVIEW); // set the matrix back to model
}

static void idle_func(void) {
  // update objects

  glutSetWindow(win_id);
  glutPostRedisplay();
}

static void motion_func(int x, int y) {
  mx = x;
  my = y;
  static bool just_warped = false;

  if (just_warped) {
    just_warped = false;
    return;
  }

  int dx = x - g_viewport_width / 2;
  int dy = y - g_viewport_height / 2;

  if (dx) {
    c_yaw += g_rotation_speed * dx;
  }

  if (dy) {
    c_RotatePitch(g_rotation_speed * dy);
  }

  glutWarpPointer(g_viewport_width / 2, g_viewport_height / 2);

  just_warped = true;
}

static void display_func(void) {
  pre_display();

  glLoadIdentity();

  c_Refresh();
  Grid();

  post_display();
}

static void open_glut_window(void) {
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

  glutInitWindowPosition(0, 0);
  glutInitWindowSize(win_x, win_y);
  win_id = glutCreateWindow("Alias | wavefront");

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glutSwapBuffers();
  glClear(GL_COLOR_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glutSwapBuffers();

  pre_display();

  // glutKeyboardFunc(key_func);
  glutMouseFunc(mouse_func);
  glutMotionFunc(motion_func);
  glutPassiveMotionFunc(motion_func);
  glutReshapeFunc(reshape_func);
  glutIdleFunc(idle_func);
  glutDisplayFunc(display_func);
  glutIgnoreKeyRepeat(1);
}

int main(int argc, char *argv[]) {
  glutInit(&argc, argv);

  win_x = 640;
  win_y = 480;
  c_SetPos(1, 1, 1);

  open_glut_window();
  glutMainLoop();

  return 0;
}
