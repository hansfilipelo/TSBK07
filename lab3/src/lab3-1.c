// Questions:
// 1) Why don't we need a buffer to upload translMatrix? Do we need buffers for everything that is not uniform (for example different colors)?


// Lab 1-2.
// This is the same as the first simple example in the course book,
// but with a few error checks.
// Remember to copy your file to a new on appropriate places during the lab so you keep old results.
// Note that the files "lab1-1.frag", "lab1-1.vert" are required.

// Should work as is on Linux and Mac. MS Windows needs GLEW or glee.
// See separate Visual Studio version of my demos.
#ifdef __APPLE__
  #include <OpenGL/gl3.h>
  #include "../common/mac/MicroGlut.h"
#elif defined __linux__
  #include "../common/Linux/MicroGlut.h"
#endif

#define MAX_FILE_SIZE 255
/* max file name length on windows using it since a file probably does not
 have a longer name than this */

#include <math.h>
#include <string.h>

#include "../common/GL_utilities.h"
#include "../common/loadobj.h"
#include "../common/LoadTGA.h"
#include "../common/VectorUtils3.h"

#define PI 3.14159265

#define near 1.0
#define far 100.0
#define right 0.5
#define left -0.5
#define top 0.5
#define bottom -0.5

// ---------------

void OnTimer(int value)
{
  glutPostRedisplay();
  glutTimerFunc(20, &OnTimer, value);
}

// Globals
// Data would normally be read from files

mat4 transformMatrix;

GLfloat projectionMatrix[] =
{
  2.0f*near/(right-left), 0.0f, (right+left)/(right-left), 0.0f,
  0.0f, 2.0f*near/(top-bottom), (top+bottom)/(top-bottom), 0.0f,
  0.0f, 0.0f, -(far + near)/(far - near), -2*far*near/(far - near),
  0.0f, 0.0f, -1.0f, 0.0f
};

int sizeOfGLfloat = sizeof(GLfloat)/2;
GLfloat phi = 0;
GLfloat cosPhi;
GLfloat sinPhi;
GLfloat minSin;

// Reference to shader program
GLuint program;
GLuint tex;

Model *mill;
Model *wing;

// ----------------------------------------

void init(void)
{
  // vertex buffer object, used for uploading the geometry

  dumpInfo(); // Dump driver info to stdout

  // Load Model
  mill = LoadModelPlus("models/windmill/windmill-walls.obj");
  wing = LoadModelPlus("models/windmill/blade.obj");

  // GL inits
  glClearColor(1,1,1,0);
  //glClearColor(0.2,0.2,0.5,0); // Color in buffer upon clear buffer
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glEnable(GL_DEPTH_TEST);
  printError("GL inits");

  // ------------------------------------
  // Load and compile shader
  char* vertex_shader = malloc(MAX_FILE_SIZE);
  char* fragment_shader = malloc(MAX_FILE_SIZE);

  // Append correct filename to shaders
  char* this_file = __FILE__;
  /* File ends with .c, 2 chars needs to be
  removed when appending to shaders which ends with .shader-stage */
  strncat(vertex_shader, this_file, strlen(this_file)-2);
  strncat(fragment_shader, this_file, strlen(this_file)-2);
  // Append name of shader-stage
  strcat(vertex_shader,".vert");
  strcat(fragment_shader,".frag");

  program = loadShaders(vertex_shader, fragment_shader); // These are the programs that run on GPU
  printError("init shader");



  // --------------------------------------

  glUniformMatrix4fv(glGetUniformLocation(program, "projectionMatrix"), 1, GL_TRUE, projectionMatrix);

  printError("init arrays");
}

// ----------------------------------------

void display(void)
{
  printError("pre display");

  // clear the screen (using chosen color earlier)
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(1,1,1,0);

  // Set rotation matrix
  phi = ( phi < 2*PI ) ? phi+PI/100 : phi-2*PI+PI/100;

  // Translations for windmill
  mat4 trans_mill = T(0,-5,-30);

  // Translations for wings/blades
  mat4 trans_wings = T(0,0,-26);
  mat4 rotation_wings = Ry(PI/2);
  mat4 trans_wings_up = T(0,1.3,0);

  // Translations for camera
  vec3 lookAtPoint = {0,0,-30};
  float radius = 30;
  vec3 cameraLocation = {radius*sin(phi),0.0f,-30-radius*cos(phi)};
  vec3 upVector = {0,1,0};

  mat4 lookAtMatrix = lookAtv(cameraLocation,lookAtPoint,upVector);
  transformMatrix = trans_mill;

  // Draw windmill
  glUniformMatrix4fv(glGetUniformLocation(program, "transformMatrix"), 1, GL_TRUE,  transformMatrix.m);
  // Send lookAt-vector to vertex shader
  glUniformMatrix4fv(glGetUniformLocation(program, "lookAtMatrix"), 1, GL_TRUE,  lookAtMatrix.m);
  DrawModel(mill, program, "in_Position", "in_Normal", "inTexCoord");

  // Model 2
  for (size_t i = 0; i < 4; i++) {
    transformMatrix = Mult(trans_wings_up, Mult(Rz(phi+i*(PI/2)), Mult(trans_wings, rotation_wings)));
    glUniformMatrix4fv(glGetUniformLocation(program, "transformMatrix"), 1, GL_TRUE,  transformMatrix.m);
    DrawModel(wing, program, "in_Position", "in_Normal", "inTexCoord");
  }

  printError("display");

  glutSwapBuffers(); // Swap buffer so that GPU can use the buffer we uploaded to it and we can write to another
}

// ----------------------------------------

int main(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitContextVersion(3, 2);
  glutCreateWindow("Windmill");
  glutDisplayFunc(display);
  init ();
  OnTimer(0);
  glutMainLoop();
}
