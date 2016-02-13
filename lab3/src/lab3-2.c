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
#include <ApplicationServices/ApplicationServices.h>
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

// camera

vec3 cameraLocation = {0,0.0f,0};
vec3 lookAtPoint = {0,0,-30};
vec3 upVector = {0,1,0};
static const float movement_speed = 0.2;
vec3 translator;

// Mouse

static const float mouse_speed = 0.05;
int deltax = 0;
int deltay = 0;

// ----------------------------------------

void handleMouse(int x, int y)
{
  static int last_x = 0.0;
  static int last_y = 0.0;

  float window_center_x = glutGet(GLUT_WINDOW_WIDTH)/2;
  float window_center_y = glutGet(GLUT_WINDOW_HEIGHT)/2;

  last_x = (float)x - last_x;
  last_y = (float)y - last_y;

  deltax = last_x;
  deltay = last_y;

  if((abs((int)x)>50) || (abs((int)y)>50))
  {
    deltax = 0;
    deltay = 0;

    last_x = (float)x;
    last_y = (float)y;

    return;

  }
  /*Fix for quartz issue found at http://stackoverflow.com/questions/10196603/using-cgeventsourcesetlocaleventssuppressioninterval-instead-of-the-deprecated/17547015#17547015
  */
  // if mouse wander off too much, warp it back.
  float dist = 100;

  if(x > window_center_x+dist || x < window_center_x-dist || y < window_center_y+dist || y > window_center_y-dist){
    #ifdef __APPLE__
    CGPoint warpPoint = CGPointMake(window_center_x, window_center_y);
    CGWarpMouseCursorPosition(warpPoint);
    CGAssociateMouseAndMouseCursorPosition(true);
    #endif
    #ifndef __APPLE__
    glutWarpPointer( window_center_x, window_center_y );
    #endif
  }
}

// --

void yaw(int deltax, float mouse_speed, vec3* cameraLocation, vec3* lookAtPoint)
{
  vec3 translation_values = VectorSub(*lookAtPoint, *cameraLocation);
  mat4 translation_matrix = T(translation_values.x, translation_values.y, translation_values.z);
  *lookAtPoint = MultVec3(Mult(Ry(((float)deltax)*mouse_speed), translation_matrix), *cameraLocation);
}


// ----------------------------------------

void init(void)
{
  // vertex buffer object, used for uploading the geometry

  dumpInfo(); // Dump driver info to stdout

  glutPassiveMotionFunc(&handleMouse); // set up mouse movement.
  #ifdef __APPLE__
  glutHideCursor();
  #endif

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

void handleKeyBoard(vec3* cameraLocation, vec3* lookAtPoint, vec3* upVector, const float* movement_speed)
{
  // This is the direction the camera is looking
  vec3 direction = Normalize(VectorSub(*cameraLocation, *lookAtPoint));

  if ( glutKeyIsDown('w') ) {
    *cameraLocation = VectorAdd(*cameraLocation, ScalarMult(direction, -*movement_speed));
  }
  if (glutKeyIsDown('d')) {
    translator = ScalarMult(Normalize(CrossProduct(direction, *upVector)), -*movement_speed);
    *cameraLocation = VectorAdd(*cameraLocation, translator);
    *lookAtPoint = VectorAdd(*lookAtPoint, translator);
  }
  if ( glutKeyIsDown('a') ) {
    translator = ScalarMult(Normalize(CrossProduct(direction, *upVector)), *movement_speed);
    *cameraLocation = VectorAdd(*cameraLocation, translator);
    *lookAtPoint = VectorAdd(*lookAtPoint, translator);
  }
  if ( glutKeyIsDown('s') ) {
    *cameraLocation = VectorAdd(*cameraLocation, ScalarMult(direction, *movement_speed));
  }
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

  // ---------------------------
  // Movement of camera with keyboard
  handleKeyBoard(&cameraLocation, &lookAtPoint, &upVector, &movement_speed);
  yaw(deltax, mouse_speed, &cameraLocation, &lookAtPoint);

  // ---------------------------

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
  init();
  OnTimer(0);
  glutMainLoop();
}
