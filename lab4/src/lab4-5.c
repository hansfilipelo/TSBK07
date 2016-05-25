// Lab 4, terrain generation
#define MAX_FILE_SIZE 255

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
#include <math.h>

mat4 projectionMatrix;

vec3 cam = {8, 5, 8};
vec3 lookAtPoint = {10, 5, 10};
mat4 transposeLocation;
float t = 0.0;
int t_switch = 0;

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

vec3 getVertex(Model* model, int x, int z, int texWidth)
{
    return (vec3){model->vertexArray[(x + z * texWidth)*3 + 0], model->vertexArray[(x + z * texWidth)*3 + 1], model->vertexArray[(x + z * texWidth)*3 + 2]};
}

vec3 getNormal(Model* model, int x, int z, int texWidth)
{
    return (vec3){model->normalArray[(x + z * texWidth)*3 + 0], model->normalArray[(x + z * texWidth)*3 + 1], model->normalArray[(x + z * texWidth)*3 + 2]};
}

float getHeight(Model* model, float x, float z, int texWidth)
{
    int intX = (int)x;
    int intZ = (int)z;
    float deltax = x - (float)intX;
    float deltaz = z - (float)intZ;

    //vec3 vertice[3];
    vec3 normal;
    vec3 vertice[3];
    float scalers[3];
    float distance_x;
    float distance_z;

    if ( deltax + deltaz < 1 ) {
       
        vertice[0] = getVertex(model, intX, intZ, texWidth);
        vertice[1] = getVertex(model, intX+1, intZ, texWidth);
        vertice[2] = getVertex(model, intX, intZ+1, texWidth);

        normal = CrossProduct(VectorSub(vertice[1],vertice[0]), VectorSub(vertice[2], vertice[0])); 
        //distance_x = vertice[1].x - vertice[0].x;
        //distance_z = vertice[2].z - vertice[0].z;

        //scalers[0] = sqrt(pow((distance_x-deltax)/distance_x,2) + pow((distance_z-deltaz)/distance_z,2));
        //scalers[1] = sqrt(pow(deltax/distance_x,2) + pow((distance_z-deltaz)/distance_z,2));
        //scalers[2] = sqrt(pow((distance_x-deltax)/distance_x,2) + pow(deltaz/distance_z,2));
    }
    else {
        deltax = (float)intX+1.f - x;
        deltaz = (float)intZ+1.f - z;
        vertice[0] = getVertex(model, intX+1, intZ+1, texWidth);
        vertice[1] = getVertex(model, intX+1, intZ, texWidth);
        vertice[2] = getVertex(model, intX, intZ+1, texWidth);
        
        normal = CrossProduct(VectorSub(vertice[1],vertice[0]), VectorSub(vertice[2], vertice[0])); 
        //distance_x = vertice[0].x - vertice[2].x;
        //distance_z = vertice[0].z - vertice[1].z;

        //scalers[0] = sqrt(pow((distance_x-deltax)/distance_x,2) + pow((distance_z-deltaz)/distance_z,2));
        //scalers[2] = sqrt(pow(deltax/distance_x,2) + pow((distance_z-deltaz)/distance_z,2));
        //scalers[1] = sqrt(pow((distance_x-deltax)/distance_x,2) + pow(deltaz/distance_z,2));
    }
    
    float D = normal.x*vertice[0].x + normal.y*vertice[0].y + normal.z*vertice[0].z;

    float height = (D-normal.x*x-normal.z*z)/normal.y;
    
    return height;
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

            // Calculating normal vectors
            // ---------
            vec3 first_vertex = (vec3){vertexArray[(x + z * tex->width)*3 + 0], vertexArray[(x + z * tex->width)*3 + 1], vertexArray[(x + z * tex->width)*3 + 2]};
            vec3 second_vertex;
            vec3 third_vertex;
            // Will seg-fault due to out of range if we don't check boundaries

            second_vertex = (x+1 < tex->width) ? (vec3){vertexArray[((x+1) + z * tex->width)*3 + 0], vertexArray[((x+1) + z * tex->width)*3 + 1], vertexArray[((x+1) + z * tex->width)*3 + 2]} : (vec3){vertexArray[(x + z * tex->width)*3 + 0], vertexArray[(x + z * tex->width)*3 + 1], vertexArray[(x + z * tex->width)*3 + 2]};

            third_vertex = (z+1 < tex->height) ? (vec3){vertexArray[(x + (z+1) * tex->width)*3 + 0], vertexArray[(x + (z+1) * tex->width)*3 + 1], vertexArray[(x + (z+1) * tex->width)*3 + 2]} : (vec3){vertexArray[(x + z * tex->width)*3 + 0], vertexArray[(x + z * tex->width)*3 + 1],vertexArray[(x + z * tex->width)*3 + 2]};

            vec3 first_base = VectorSub(second_vertex, first_vertex);
            vec3 second_base = VectorSub(third_vertex, first_vertex);
            vec3 normal_one = Normalize(CrossProduct(second_base, first_base));
            // Make sure normal is always upward facing
            if (normal_one.y < 0)
            {
                normal_one = ScalarMult(normal_one, -1.0);
            }
            // ---------
            second_vertex = (x+1 < tex->width && z-1 > 0) ? (vec3){vertexArray[((x+1) + (z-1) * tex->width)*3 + 0], vertexArray[((x+1) + (z-1) * tex->width)*3 + 1], vertexArray[((x+1) + (z-1) * tex->width)*3 + 2]} : (vec3){vertexArray[(x + z * tex->width)*3 + 0], vertexArray[(x + z * tex->width)*3 + 1], vertexArray[(x + z * tex->width)*3 + 2]};

            third_vertex = (z-1 > 0) ? (vec3){vertexArray[(x + (z-1) * tex->width)*3 + 0], vertexArray[(x + (z-1) * tex->width)*3 + 1], vertexArray[(x + (z-1) * tex->width)*3 + 2]} : (vec3){vertexArray[(x + z * tex->width)*3 + 0], vertexArray[(x + z * tex->width)*3 + 1],vertexArray[(x + z * tex->width)*3 + 2]};

            first_base = VectorSub(second_vertex, third_vertex);
            second_base = VectorSub(third_vertex, first_vertex);
            vec3 normal_two = Normalize(CrossProduct(second_base, first_base));
            // Make sure normal is always upward facing
            if (normal_two.y < 0)
            {
                normal_two = ScalarMult(normal_two, -1.0);
            }
            // ---------
            second_vertex = (x-1 > 0) ? (vec3){vertexArray[((x-1) + z * tex->width)*3 + 0], vertexArray[((x-1) + z * tex->width)*3 + 1], vertexArray[((x-1) + z * tex->width)*3 + 2]} : (vec3){vertexArray[(x + z * tex->width)*3 + 0], vertexArray[(x + z * tex->width)*3 + 1], vertexArray[(x + z * tex->width)*3 + 2]};

            third_vertex = (z+1 < tex->width && x-1 > 0 ) ? (vec3){vertexArray[((x-1) + (z+1) * tex->width)*3 + 0], vertexArray[((x-1) + (z+1) * tex->width)*3 + 1], vertexArray[((x-1) + (z+1) * tex->width)*3 + 2]} : (vec3){vertexArray[(x + z * tex->width)*3 + 0], vertexArray[(x + z * tex->width)*3 + 1],vertexArray[(x + z * tex->width)*3 + 2]};

            first_base = VectorSub(second_vertex, first_vertex);
            second_base = VectorSub(third_vertex, first_vertex);
            vec3 normal_three = Normalize(CrossProduct(second_base, first_base));
            // Make sure normal is always upward facing
            if (normal_three.y < 0)
            {
                normal_three = ScalarMult(normal_three, -1.0);
            }

            // --------

            vec3 normal = Normalize(VectorAdd(VectorAdd(normal_one, normal_two), normal_three));

            // Normal vectors. You need to calculate these.
            normalArray[(x + z * tex->width)*3 + 0] = normal.x;
            normalArray[(x + z * tex->width)*3 + 1] = normal.y;
            normalArray[(x + z * tex->width)*3 + 2] = normal.z;
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
Model* sphere;
// Reference to shader program
GLuint program;
GLuint model_shader;
GLuint tex1, tex2;
TextureData ttex; // terrain

// -------------------------------------------------

void init(void)
{
    // GL inits
    glClearColor(0.2,0.2,0.5,0);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    printError("GL inits");

    projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 100.0);

    // Load and compile shader
    program = loadShaders("src/terrain.vert", "src/terrain.frag");
    printError("init shader");

    // ------------------------------------
    // Load and compile shader
    char* vertex_shader = malloc(MAX_FILE_SIZE);
    char* fragment_shader = malloc(MAX_FILE_SIZE);
    // Initialize to empty string
    vertex_shader[0] = '\0';
    fragment_shader[0] = '\0';

    // Append correct filename to shaders
    char* this_file = __FILE__;
    /* File ends with .c, 2 chars needs to be
       removed when appending to shaders which ends with .shader-stage */
    strncat(vertex_shader, this_file, strlen(this_file)-2);
    strncat(fragment_shader, this_file, strlen(this_file)-2);
    // Append name of shader-stage
    strcat(vertex_shader,".vert");
    strcat(fragment_shader,".frag");

    model_shader = loadShaders(vertex_shader, fragment_shader); // These are the programs that run on GPU
    printError("init shader");

    // Load terrain data

    LoadTGATextureData("models/fft-terrain.tga", &ttex);
    tm = GenerateTerrain(&ttex);
    printError("init terrain");

    // Upload light
    glUseProgram(program);
    glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
    glUniform1i(glGetUniformLocation(program, "tex"), 0); // Texture unit 0
    LoadTGATextureSimple("models/grass.tga", &tex1);
    printError("Init grass");

    glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"), 4, &lightSourcesDirectionsPositions[0].x);
    glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"), 4, &lightSourcesColorsArr[0].x);
    glUniform1fv(glGetUniformLocation(program, "specularExponent"), 4, specularExponent);
    glUniform1iv(glGetUniformLocation(program, "isDirectional"), 4, isDirectional);
    printError("Init light program");

    glUseProgram(model_shader);
    glUniform3fv(glGetUniformLocation(model_shader, "lightSourcesDirPosArr"), 4, &lightSourcesDirectionsPositions[0].x);
    glUniform3fv(glGetUniformLocation(model_shader, "lightSourcesColorArr"), 4, &lightSourcesColorsArr[0].x);
    glUniform1fv(glGetUniformLocation(model_shader, "specularExponent"), 4, specularExponent);
    glUniform1iv(glGetUniformLocation(model_shader, "isDirectional"), 4, isDirectional);
    printError("Init light model_shader");

    sphere = LoadModelPlus("models/groundsphere.obj");
    glUniformMatrix4fv(glGetUniformLocation(model_shader, "transformMatrix"), 1, GL_TRUE, transposeLocation.m);
    glUniformMatrix4fv(glGetUniformLocation(model_shader, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
    printError("Init model shader last");
    // lookAtPoint = (vec3){10, getHeight(tm, 10, 10, ttex.width), 10};
    // cam = (vec3){12, getHeight(tm, 12, 12, ttex.width)+2, 12};
}

// -------------------------------------------------

void display(void)
{
    handle_keyboard(&cam, &lookAtPoint, &upVector, &movement_speed);

    // clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4 total, modelView, camMatrix;

    // Move sphere
    t = t+0.01;
    int start = 5;
    float y_height = getHeight(tm, start+t, start+t, ttex.width);
    transposeLocation = T(start+t, y_height, start+t);
    //printf("y_height: %f, x/z: %f\n", y_height, start+t);

    printError("pre display");

    glUseProgram(program);

    // Build matrix
    cam = (vec3){cam.x, getHeight(tm, cam.x, cam.z, ttex.width)+1, cam.z};
    camMatrix = lookAtv(cam,lookAtPoint,upVector);

    modelView = IdentityMatrix();
    total = Mult(camMatrix, modelView);
    glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
    // For light
    glUniform3f(glGetUniformLocation(program, "camera_position"), cam.x, cam.y, cam.z);
    glBindTexture(GL_TEXTURE_2D, tex1);     // Bind Our Texture tex1
    DrawModel(tm, program, "inPosition", "inNormal", "inTexCoord");

    // Draw sphere
    glUseProgram(model_shader);
    glUniformMatrix4fv(glGetUniformLocation(model_shader, "mdlMatrix"), 1, GL_TRUE, total.m);
    glUniform3f(glGetUniformLocation(model_shader, "camera_position"), cam.x, cam.y, cam.z);
    glUniformMatrix4fv(glGetUniformLocation(model_shader, "transformMatrix"), 1, GL_TRUE, transposeLocation.m);
    DrawModel(sphere, model_shader, "inPosition", "inNormal", "inTexCoord");

    printError("display 3");

    glutSwapBuffers();
}

// -------------------------------------------------

void timer(int i)
{
    glutTimerFunc(20, &timer, i);
    glutPostRedisplay();
}

void mouse(int x, int y)
{
    printf("%d %d\n", x, y);
}

// -------------------------------------------------

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
