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
GLuint ground_shaders;
GLuint ground_tex;

Model *mill;
Model *wing;
Model *ground;

// camera

vec3 cameraLocation = {0,0.0f,0};
vec3 lookAtPoint = {0,0,-30};
static const vec3 upVector = {0,1,0};
static const float movement_speed = 0.6;


// Mouse
static const float mouse_speed = 0.01;

// ---------------------------

void yaw(float deltax, float mouse_speed, vec3* cameraLocation, vec3* lookAtPoint, const vec3* upVector)
{
  // Does yaw
  vec3 look_at_vector = VectorSub(*lookAtPoint, *cameraLocation);
  mat4 translation_matrix = T(look_at_vector.x, look_at_vector.y, look_at_vector.z);
  *lookAtPoint = MultVec3(Mult(Ry(-deltax*mouse_speed), translation_matrix), *cameraLocation);
}

// ---

void pitch(float deltay, float mouse_speed, vec3* cameraLocation, vec3* lookAtPoint, const vec3* upVector)
{
  // Evaluate once
  static const float break_angle = 0.88; // cos(pi/4) = 0.71
  // End eval once

  // Do pitch if the angle between upVector and look_at_vector isn't too small
  vec3 look_at_vector = VectorSub(*lookAtPoint, *cameraLocation);
  float dot_product = DotProduct(look_at_vector, *upVector);

  mat4 translation_matrix = T(look_at_vector.x, look_at_vector.y, look_at_vector.z);
  vec3 rotation_axis = ScalarMult(CrossProduct(look_at_vector, *upVector), -1);
  *lookAtPoint = MultVec3(Mult(ArbRotate(rotation_axis, deltay*mouse_speed), translation_matrix), *cameraLocation);
}

// ---

void handleMouse(int x, int y)
{
  // Only initialized once
  static float last_x = 0.0;
  static float last_y = 0.0;
  static float deltax;
  static float deltay;
  // End of only initialized once

  int window_center_x = glutGet(GLUT_WINDOW_WIDTH)/2;
  int window_center_y = glutGet(GLUT_WINDOW_HEIGHT)/2;

  deltax = (float)x - last_x;
  deltay = (float)y - last_y;

  last_x = x;
  last_y = y;

  //if the mouse does large changes quickly (for example during a warp, ignore the change)
  if((abs((int)deltax)>25) || (abs((int)deltay)>25))
  {
    deltax = 0;
    deltay = 0;

    last_x = (float)x;
    last_y = (float)y;

  }

  // Do rotation
  yaw(deltax, mouse_speed, &cameraLocation, &lookAtPoint, &upVector);
  pitch(deltay, mouse_speed, &cameraLocation, &lookAtPoint, &upVector);

  /*Fix for quartz issue found at http://stackoverflow.com/questions/10196603/using-cgeventsourcesetlocaleventssuppressioninterval-instead-of-the-deprecated/17547015#17547015
  */
  // if mouse wander off too much, warp it back.
  int dist = (window_center_x < window_center_y) ? window_center_x/2: window_center_y/2;

  if(x > window_center_x+dist || x < window_center_x-dist || y > window_center_y+dist || y < window_center_y-dist){
    #ifdef __APPLE__ // Fix for OS X >= 10.10, which MicroGlut does not work with
    CGPoint warpPoint = CGPointMake(window_center_x, window_center_y);
    CGWarpMouseCursorPosition(warpPoint);
    CGAssociateMouseAndMouseCursorPosition(true);
    #endif
    #ifndef __APPLE__
    glutWarpPointer( window_center_x, window_center_y );
    #endif
  }
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
  ground = LoadModelPlus("models/ground.obj");

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
  ground_shaders = loadShaders("src/lab3-3-ground.vert", "src/lab3-3-ground.frag");
  printError("init shader");

  // --------------------------------------

  glUseProgram(program);
  glUniformMatrix4fv(glGetUniformLocation(program, "projectionMatrix"), 1, GL_TRUE, projectionMatrix);

  glUseProgram(ground_shaders);
  glUniformMatrix4fv(glGetUniformLocation(ground_shaders, "projectionMatrix"), 1, GL_TRUE, projectionMatrix);
  glUniform1i(glGetUniformLocation(ground_shaders, "tex"), 0); // Texture unit 0
	LoadTGATextureSimple("models/grass.tga", &ground_tex); // 5c

  printError("init arrays");

}


// ----------------------------------------

void handleKeyBoard(vec3* cameraLocation, vec3* lookAtPoint, const vec3* upVector, const float* movement_speed)
{
  // This is the direction the camera is looking
  vec3 translator;
  vec3 y_vector = {0, 1, 0};
  vec3 direction = Normalize(VectorSub(*cameraLocation, *lookAtPoint));
  vec3 temp = {direction.x, 0, direction.z};
  vec3 projected_direction = Normalize(temp);

  if ( glutKeyIsDown('w') ) {
    translator = ScalarMult(projected_direction,-*movement_speed);
    *cameraLocation = VectorAdd(*cameraLocation, ScalarMult(projected_direction, -*movement_speed));
    *lookAtPoint = VectorAdd(*lookAtPoint, translator);
  }
  if (glutKeyIsDown('d')) {
    translator = ScalarMult(Normalize(CrossProduct(projected_direction, y_vector)), -*movement_speed);
    *cameraLocation = VectorAdd(*cameraLocation, translator);
    *lookAtPoint = VectorAdd(*lookAtPoint, translator);
  }
  if ( glutKeyIsDown('a') ) {
    translator = ScalarMult(Normalize(CrossProduct(projected_direction, y_vector)), *movement_speed);
    *cameraLocation = VectorAdd(*cameraLocation, translator);
    *lookAtPoint = VectorAdd(*lookAtPoint, translator);
  }
  if ( glutKeyIsDown('s') ) {
    translator = ScalarMult(projected_direction,*movement_speed);
    *cameraLocation = VectorAdd(*cameraLocation, translator);
    *lookAtPoint = VectorAdd(*lookAtPoint, translator);
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

  // ---------------------------

  mat4 lookAtMatrix = lookAtv(cameraLocation,lookAtPoint,upVector);
  transformMatrix = trans_mill;

  glUseProgram(program);
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

  // Draw and texture ground
  glUseProgram(ground_shaders);
  transformMatrix = T(0,0,0);
  glUniformMatrix4fv(glGetUniformLocation(ground_shaders, "transformMatrix"), 1, GL_TRUE, transformMatrix.m);
  DrawModel(ground, ground_shaders, "in_Position", "in_Normal", "inTexCoord");

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
