// Lab 4, terrain generation

#include<stdio.h>
#include<stdlib.h>

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include "../common/mac/MicroGlut.h"
#include <ApplicationServices/ApplicationServices.h>
#elif defined __linux__
#define GL_GLEXT_PROTOTYPES
#include "../common/Linux/MicroGlut.h"
#include<X11/X.h>
#include<X11/Xlib.h>
#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>
#endif

#include <math.h>
#include <string.h>

#include "../common/input_handler.h"
#include "../common/GL_utilities.h"
#include "../common/loadobj.h"
#include "../common/LoadTGA.h"
#include "../common/VectorUtils3.h"
#include "../common/input_handler.h"

mat4 projectionMatrix;

vec3 cam = {0, 5, 10};
vec3 lookAtPoint = {2, 0, 2};
static const vec3 upVector = {0,1,0};
static const float movement_speed = 0.3;
// Mouse
static const float mouse_speed = 0.01;

void handle_mouse_helper(int x, int y){
  handle_mouse(x, y, mouse_speed, &cam, &lookAtPoint, &upVector);
}

// ----------------------------------------
// Light

Point3D lightSourcesColorsArr[] = {
  {0.58f, 0.58f, 0.45f},
  {0.2f, 0.5f, 0.2f},
  {0.2f, 0.5f, 0.2},
  {0.2f, 0.5f, 0.2f}
};

Point3D lightSourcesDirectionsPositions[] = {
  {10.0f, 5.0f, 0.0f},
  {0.0f, 5.0f, 10.0f},
  {-1.0f, 0.0f, 0.0f},
  {0.0f, 0.0f, -1.0f}
};

GLfloat specularExponent[] = {10.0, 20.0, 60.0, 5.0};
GLint isDirectional[] = {0,0,1,1};

// ----------------------------------------

float getHeight(Model* model, float x, float z, int texWidth)
{
  int intX = (int)x;
  int intZ = (int)z;
  float deltax = x - intX;
  float deltaz = z - intZ;

  vec3 normal;
  vec3 a_vertex;

  if (deltax + deltaz < 1)
  {
    normal = (vec3){model->normalArray[(intX + intZ * texWidth)*3 + 0], model->normalArray[(intX + intZ * texWidth)*3 + 1], model->normalArray[(intX + intZ * texWidth)*3 + 2]};
    a_vertex = (vec3){model->vertexArray[(intX + intZ * texWidth)*3 + 0], model->vertexArray[(intX + intZ * texWidth)*3 + 1], model->vertexArray[(intX + intZ * texWidth)*3 + 2]};
  }
  else
  {
    normal = (vec3){model->normalArray[(intX+1 + intZ+1 * texWidth)*3 + 0], model->normalArray[(intX+1 + intZ+1 * texWidth)*3 + 1], model->normalArray[(intX+1 + intZ+1 * texWidth)*3 + 2]};
    a_vertex = (vec3){model->vertexArray[(intX+1 + intZ+1 * texWidth)*3 + 0], model->vertexArray[(intX+1 + intZ+1 * texWidth)*3 + 1], model->vertexArray[(intX+1 + intZ+1 * texWidth)*3 + 2]};
  }

  // Get constant for plane equation (Ax+By+Cz = D), any corner will do
  float D = DotProduct(normal, a_vertex);

  // Return height = y = (D-Ax-Cz)/B
  return (D - normal.x*x - normal.z*z)/normal.y;
}

// ----------------------------------------

Model* GenerateTerrain(TextureData *tex)
{
  int vertexCount = tex->width * tex->height;
  int triangleCount = (tex->width-1) * (tex->height-1) * 2;
  int x, z;

  GLfloat *vertexArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
  GLfloat *normalArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
  GLfloat *texCoordArray = malloc(sizeof(GLfloat) * 2 * vertexCount);
  GLuint *indexArray = malloc(sizeof(GLuint) * triangleCount*3);

  printf("bpp %d\n", tex->bpp);
  for (x = 0; x < tex->width; x++)
  for (z = 0; z < tex->height; z++)
  {
    // Vertex array. You need to scale this properly
    vertexArray[(x + z * tex->width)*3 + 0] = x / 1.0;
    vertexArray[(x + z * tex->width)*3 + 1] = tex->imageData[(x + z * tex->width) * (tex->bpp/8)] / 20.0;
    vertexArray[(x + z * tex->width)*3 + 2] = z / 1.0;

    // Calculating normal vectors
    vec3 first_vertex = {vertexArray[(x + z * tex->width)*3 + 0], vertexArray[(x + z * tex->width)*3 + 1], vertexArray[(x + z * tex->width)*3 + 2]};
    vec3 second_vertex;
    vec3 third_vertex;
      // Will seg-fault due to out of range if we don't check boundaries
    second_vertex = (x+1 > tex->width) ? (vec3){vertexArray[((x+1) + z * tex->width)*3 + 0], vertexArray[((x+1) + z * tex->width)*3 + 1], vertexArray[((x+1) + z * tex->width)*3 + 2]} : (vec3){0,1,0};
    third_vertex = (z+1 > tex->height) ? (vec3){vertexArray[(x + (z+1) * tex->width)*3 + 0], vertexArray[(x + (z+1) * tex->width)*3 + 1], vertexArray[(x + (z+1) * tex->width)*3 + 2]} : (vec3){0,1,0};

    vec3 first_base = VectorSub(second_vertex, first_vertex);
    vec3 second_base = VectorSub(third_vertex, first_vertex);
    vec3 normal = Normalize(CrossProduct(first_base, second_base));
    // Make sure normal is always uppward facing
    if (normal.y < 0)
    {
      normal = ScalarMult(normal, -1);
    }

    // Normal vectors. You need to calculate these.
    normalArray[(x + z * tex->width)*3 + 0] = normal.x;
    normalArray[(x + z * tex->width)*3 + 1] = normal.y;
    normalArray[(x + z * tex->width)*3 + 2] = normal.z;

    // Texture coordinates. You may want to scale them.
    texCoordArray[(x + z * tex->width)*2 + 0] = x; // (float)x / tex->width;
    texCoordArray[(x + z * tex->width)*2 + 1] = z; // (float)z / tex->height;
  }
  for (x = 0; x < tex->width-1; x++)
  for (z = 0; z < tex->height-1; z++)
  {
    // Triangle 1
    indexArray[(x + z * (tex->width-1))*6 + 0] = x + z * tex->width;
    indexArray[(x + z * (tex->width-1))*6 + 1] = x + (z+1) * tex->width;
    indexArray[(x + z * (tex->width-1))*6 + 2] = x+1 + z * tex->width;
    // Triangle 2
    indexArray[(x + z * (tex->width-1))*6 + 3] = x+1 + z * tex->width;
    indexArray[(x + z * (tex->width-1))*6 + 4] = x + (z+1) * tex->width;
    indexArray[(x + z * (tex->width-1))*6 + 5] = x+1 + (z+1) * tex->width;
  }

  // End of terrain generation

  // Create Model and upload to GPU:

  Model* model = LoadDataToModel(
    vertexArray,
    normalArray,
    texCoordArray,
    NULL,
    indexArray,
    vertexCount,
    triangleCount*3);

    return model;
  }

  // vertex array object
  Model *m, *m2, *tm;
  // Reference to shader program
  GLuint program;
  GLuint tex1, tex2;
  TextureData ttex; // terrain

  void init(void)
  {
    // GL inits
    glClearColor(0.2,0.2,0.5,0);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    printError("GL inits");

    projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 50.0);

    // Load and compile shader
    program = loadShaders("src/terrain.vert", "src/terrain.frag");
    glUseProgram(program);
    printError("init shader");

    glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
    glUniform1i(glGetUniformLocation(program, "tex"), 0); // Texture unit 0
    LoadTGATextureSimple("models/grass.tga", &tex1);

    // Load terrain data

    LoadTGATextureData("models/fft-terrain.tga", &ttex);
    tm = GenerateTerrain(&ttex);
    printError("init terrain");

    // Upload light
    glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"), 4, &lightSourcesDirectionsPositions[0].x);
    glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"), 4, &lightSourcesColorsArr[0].x);
    glUniform1fv(glGetUniformLocation(program, "specularExponent"), 4, specularExponent);
    glUniform1iv(glGetUniformLocation(program, "isDirectional"), 4, isDirectional);
    printError("Init light");
  }


  void display(void)
  {
    handle_keyboard(&cam, &lookAtPoint, &upVector, &movement_speed);

    // clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4 total, modelView, camMatrix;

    printError("pre display");

    glUseProgram(program);

    // Build matrix
    camMatrix = lookAtv(cam,lookAtPoint,upVector);

    modelView = IdentityMatrix();
    total = Mult(camMatrix, modelView);
    glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
    // For light
    glUniform3f(glGetUniformLocation(program, "camera_position"), cam.x, cam.y, cam.z);

    glBindTexture(GL_TEXTURE_2D, tex1);		// Bind Our Texture tex1
    DrawModel(tm, program, "inPosition", "inNormal", "inTexCoord");

    printError("display 2");

    glutSwapBuffers();
  }

  void timer(int i)
  {
    glutTimerFunc(20, &timer, i);
    glutPostRedisplay();
  }

  void mouse(int x, int y)
  {
    printf("%d %d\n", x, y);
  }

  int main(int argc, char **argv)
  {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitContextVersion(3, 2);
    glutInitWindowSize (600, 600);
    glutCreateWindow ("TSBK07 Lab 4");
    glutDisplayFunc(display);
    init ();
    glutTimerFunc(20, &timer, 0);

    glutPassiveMotionFunc(&handle_mouse_helper); // set up mouse movement.
    glutHideCursor(); // Mouse does not work on OS X otherwise

    glutMainLoop();
    exit(0);
  }
