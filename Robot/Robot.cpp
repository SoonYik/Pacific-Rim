#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <math.h>

#include "ShapeFunction.h" 

#pragma comment (lib, "OpenGL32.lib")
#pragma comment (lib, "GLU32.lib")
#define WINDOW_TITLE "Pacific Rim"

struct Vec3 { float x, y, z; };
struct Rot { float left, right, limitPos, limitNeg; };

// Constants
float PI = 3.1415926535;
int windowWidth = 1200;
bool isOrtho = false;

//Cam Position
float
moveSpeed = 0.15,
rotateSpeed = 2.5,
posX = -3.0,
posY = -5.0,
posZ = -6.0,
pitch = 30,
yaw = -25,
tempDistance,
forwardX,
forwardY,
forwardZ;

bool toggleLight = true;
bool isSwordActive = false;
float turbineRotation = 0.0f;

// Rotations
float objRotateSpeed = 5.0f;
Rot upperArmY = { 0, 0, 30, -180 };
Rot upperArmX = { 0, 0, 90, -60 };
Rot lowerArm = { 0, 0, 0, -80 };
Rot wristZ = { 0, 0, 90, -90 };
Rot wristY = { 0, 0, 90, -90 };
Rot finger1 = { 0, 0, 0, -90 };
Rot fingerJoint1 = { 0, 0, 0, -90 };
Rot fingerTip1 = { 0, 0, 0, -90 };
Rot finger2 = { 0, 0, 0, -90 };
Rot fingerJoint2 = { 0, 0, 0, -90 };
Rot fingerTip2 = { 0, 0, 0, -90 };
Rot finger3 = { 0, 0, 0, -90 };
Rot fingerJoint3 = { 0, 0, 0, -90 };
Rot fingerTip3 = { 0, 0, 0, -90 };
Rot finger4 = { 0, 0, 0, -90 };
Rot fingerJoint4 = { 0, 0, 0, -90 };
Rot fingerTip4 = { 0, 0, 0, -90 };
Rot thumb = { 0, 0, 0, -90 };
Rot thumbTip = { 0, 0, 0, -90 };

// Animations
int animationIndex = 0;
int animationCount = 1;
bool startAnimation = false;
float animationTime = 0;

int rotationCount = 19;
int rotationIndex = 0;
bool rotateLeft = false;

Rot fingers[] = { finger1, fingerJoint1, fingerTip1,
finger2, fingerJoint2, fingerTip2, finger3, fingerJoint3, fingerTip3,
finger4, fingerJoint4, fingerTip4, thumb, thumbTip };

Rot rotations[] = { upperArmY, upperArmX, lowerArm, wristZ, wristY, finger1, fingerJoint1, fingerTip1,
finger2, fingerJoint2, fingerTip2, finger3, fingerJoint3, fingerTip3,
finger4, fingerJoint4, fingerTip4, thumb, thumbTip };

//Color
const float CLR_NAVY[] = { 0.15f, 0.25f, 0.40f };
const float CLR_LIGHT_NAVY[] = { 0.3f, 0.5f, 0.80f };
const float CLR_GREY[] = { 0.30f, 0.35f, 0.40f };
const float CLR_RED[] = { 0.70f, 0.10f, 0.10f };
const float CLR_WHITE[] = { 0.90f, 0.90f, 0.90f };
const float CLR_YELLOW[] = { 1.00f, 0.80f, 0.00f };
const float CLR_GLOW[] = { 1.00f, 0.60f, 0.20f };
const float CLR_SILVER[] = { 0.75f, 0.75f, 0.75f };
GLuint textures[2];
int currentTexture = 0;

//Light
float noEmit[] = { 0,0,0,1 };
float highEmit[] = { 1,0.6,0.2,1 };
GLfloat ambientLight[] = { 0.2,0.2,0.2 };
GLfloat diffuseLight[] = { 0.7,0.7,0.7 };
GLfloat diffuseLightPosition[] = { 0,1.5,3,1 };

// Functions
float DegToRad(float degree) {
    return degree / 180 * PI;
}

Vec3 normalizeNormal(Vec3 normal) {
    float distance = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));

    Vec3 result;
    result.x = normal.x / distance;
    result.y = normal.y / distance;
    result.z = normal.z / distance;

    return result;
}

Vec3 calculateNormal(Vec3 vertex1, Vec3 vertex2, Vec3 vertex3, bool normalize = true) {
    Vec3 edge1;
    edge1.x = vertex2.x - vertex1.x;
    edge1.y = vertex2.y - vertex1.y;
    edge1.z = vertex2.z - vertex1.z;

    Vec3 edge2;
    edge2.x = vertex3.x - vertex1.x;
    edge2.y = vertex3.y - vertex1.y;
    edge2.z = vertex3.z - vertex1.z;

    Vec3 normal;
    normal.x = edge1.y * edge2.z - edge1.z * edge2.y;
    normal.y = edge1.z * edge2.x - edge1.x * edge2.z;
    normal.z = edge1.x * edge2.y - edge1.y * edge2.x;

    if (normalize)
        normal = normalizeNormal(normal);

    return normal;
}

GLuint LoadBMP(const char* filename)
{
    BITMAP BMP;
    GLuint textureID;

    HBITMAP hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL),
        filename, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
    if (!hBMP) {
        MessageBoxA(NULL, ("Failed to load texture"), "Error", MB_OK);
        return 0;
    }
    GetObject(hBMP, sizeof(BMP), &BMP);

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BMP.bmWidth, BMP.bmHeight,
        0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);
    DeleteObject(hBMP);

    return textureID;
}

void loadTexture()
{
	textures[0] = LoadBMP("blueSteel.bmp");
	textures[1] = LoadBMP("redSteel.bmp");
}

//shapes initialize
void drawBox(float x, float y, float z) {
    glBegin(GL_QUADS);

    x /= 2;
    y /= 2;
    z /= 2;

    // Front
    glNormal3f(0, 0, -1);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-x, -y, -z);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x, -y, -z);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y, -z);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-x, y, -z);

    // Back
    glNormal3f(0, 0, 1);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x, -y, z);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-x, -y, z);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-x, y, z);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y, z);

    // Top
    glNormal3f(0, 1, 0);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-x, y, -z);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x, y, -z);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y, z);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-x, y, z);

    // Bottom
    glNormal3f(0, -1, 0);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x, -y, z);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-x, -y, z);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-x, -y, -z);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x, -y, -z);

    // Left
    glNormal3f(-1, 0, 0);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-x, -y, -z);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-x, y, -z);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-x, y, z);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-x, -y, z);

    // Right
    glNormal3f(1, 0, 0);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x, -y, -z);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x, -y, z);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y, z);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y, -z);

    glEnd();
}

void drawCustomCuboid(Vec3 vertices[8]) {
    // Vertex's location based on array position in the cuboid
    // 
    //     4------5
    //    /|     /|
    //   0-+----1 |
    //   | 6----+-7
    //   |/     |/
    //   2------3

    Vec3 normal;

    glBegin(GL_QUADS);

    // Front
    normal = calculateNormal(vertices[2], vertices[3], vertices[1]);
    glNormal3f(normal.x, normal.y, normal.z);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(vertices[2].x, vertices[2].y, vertices[2].z);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(vertices[3].x, vertices[3].y, vertices[3].z);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(vertices[1].x, vertices[1].y, vertices[1].z);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(vertices[0].x, vertices[0].y, vertices[0].z);

    // Back
    normal = calculateNormal(vertices[7], vertices[6], vertices[4]);
    glNormal3f(normal.x, normal.y, normal.z);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(vertices[7].x, vertices[7].y, vertices[7].z);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(vertices[6].x, vertices[6].y, vertices[6].z);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(vertices[4].x, vertices[4].y, vertices[4].z);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(vertices[5].x, vertices[5].y, vertices[5].z);

    // Left
    normal = calculateNormal(vertices[6], vertices[2], vertices[0]);
    glNormal3f(normal.x, normal.y, normal.z);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(vertices[6].x, vertices[6].y, vertices[6].z);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(vertices[2].x, vertices[2].y, vertices[2].z);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(vertices[0].x, vertices[0].y, vertices[0].z);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(vertices[4].x, vertices[4].y, vertices[4].z);

    // Right
    normal = calculateNormal(vertices[3], vertices[7], vertices[5]);
    glNormal3f(normal.x, normal.y, normal.z);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(vertices[3].x, vertices[3].y, vertices[3].z);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(vertices[7].x, vertices[7].y, vertices[7].z);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(vertices[5].x, vertices[5].y, vertices[5].z);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(vertices[1].x, vertices[1].y, vertices[1].z);

    // Top
    normal = calculateNormal(vertices[0], vertices[1], vertices[5]);
    glNormal3f(normal.x, normal.y, normal.z);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(vertices[0].x, vertices[0].y, vertices[0].z);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(vertices[1].x, vertices[1].y, vertices[1].z);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(vertices[5].x, vertices[5].y, vertices[5].z);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(vertices[4].x, vertices[4].y, vertices[4].z);

    // Bottom
    normal = calculateNormal(vertices[3], vertices[2], vertices[6]);
    glNormal3f(normal.x, normal.y, normal.z);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(vertices[3].x, vertices[3].y, vertices[3].z);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(vertices[2].x, vertices[2].y, vertices[2].z);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(vertices[6].x, vertices[6].y, vertices[6].z);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(vertices[7].x, vertices[7].y, vertices[7].z);

    glEnd();
}

void drawSphere(float radius, float slices, float stack) {
    GLUquadricObj* s = gluNewQuadric();
    gluQuadricNormals(s, GLU_SMOOTH);
    gluQuadricTexture(s, GL_TRUE); 
    gluSphere(s, radius, slices, stack);
    gluDeleteQuadric(s);
}

void drawDisk(float inner, float outer, int slices) {
    GLUquadricObj* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluDisk(q, inner, outer, slices, 4);
    gluDeleteQuadric(q);
}

void drawCylinder(float base, float top, float height, int slices, bool hasCaps = true) {
    glPushMatrix();
    glTranslatef(0, -height / 2, 0);
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    glRotatef(360.0f / slices / 2, 0, 0, 1);

    if (hasCaps)
        drawDisk(0, base, slices);
    
    GLUquadricObj* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluQuadricTexture(q, GL_TRUE); 
    gluCylinder(q, base, top, height, slices, 4);
    gluDeleteQuadric(q);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, height, 0);
    glRotatef(-90, 1, 0, 0);
    glRotatef(360.0f / slices / 2, 0, 0, 1);

    if (hasCaps)
        drawDisk(0, top, slices);
    
    glPopMatrix();
    glPopMatrix();
}

//draw details
void drawAbdomenLayer(float xPos, float yPos, float zPos)
{
    for (int i = 0; i < 3; i++)
    {
        yPos -= 0.25f;

        glPushMatrix();
        glTranslatef(xPos, yPos, zPos);
        drawBox(0.3f, 0.2f, 0.3);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-xPos, yPos, zPos);
        drawBox(0.3f, 0.2f, 0.3f);
        glPopMatrix();
    }
}

void drawAbdomenDetail()
{
    glPushMatrix();//middle
    glColor3fv(CLR_GREY);
    drawBox(0.5f, 1.5f, 0.6f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0.3f, 0.25f);

    glColor3fv(CLR_NAVY);//layers
    drawAbdomenLayer(0.3, 0.3, 0.1);
    glColor3fv(CLR_SILVER);
    drawAbdomenLayer(0.4, 0.25, 0.0);
    glColor3fv(CLR_NAVY);
    drawAbdomenLayer(0.5, 0.2, -0.1);
    glColor3fv(CLR_NAVY);
    drawAbdomenLayer(0.5, 0.2, -0.3);
    glColor3fv(CLR_SILVER);
    drawAbdomenLayer(0.4, 0.25, -0.4);
    glColor3fv(CLR_NAVY);
    drawAbdomenLayer(0.3, 0.3, -0.5);

    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_EMISSION, highEmit);
    glColor3fv(CLR_RED);
    glTranslatef(0, 0, 0.02f);
    drawBox(0.08f, 0.3f, 0.1f);
    glMaterialfv(GL_FRONT, GL_EMISSION, noEmit);
    glPopMatrix();

    glPopMatrix();
}

void DrawWingPart(float side) { // side: 1.0 for right, -1.0 for left
    glPushMatrix();
    glTranslatef(0.8f * side, 0.0f, 0.0f);
    glRotatef(35.0f * side, 0.0f, 0.0f, 1.0f);

    DrawBox(0.5f, 1.8f, 0.3f, 0.25f, 0.25f, 0.25f);

    glPushMatrix();
    glTranslatef(0.0f, 0.2f, 0.16f);
    DrawBox(0.3f, 1.0f, 0.05f, 0.1f, 0.1f, 0.1f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 0.9f, 0.0f);
    DrawBox(0.5f, 0.2f, 0.32f, 0.8f, 0.1f, 0.1f);
    glPopMatrix();
    glPopMatrix();
}

void DrawSword() {
    glPushMatrix();
    glTranslatef(0.0f, -4.5f, 0.0f);

    // ??? (??)
    DrawCylinder(0.12f, 0.12f, 3.5f, 0.1f, 0.1f, 0.1f);

    // ?????? (Pommel)
    glPushMatrix();
    DrawSphere(0.25f, 0.4f, 0.4f, 0.4f);
    glPopMatrix();

    // ???????? (??)
    glPushMatrix();
    glTranslatef(0.0f, 1.5f, 0.0f);
    DrawCylinder(0.14f, 0.14f, 0.3f, 0.8f, 0.0f, 0.0f);
    glPopMatrix();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(0.0f, -1.0f, 0.0f); // ????

    DrawBox(1.2f, 1.0f, 0.6f, 0.2f, 0.2f, 0.2f);

    // ???????
    DrawWingPart(1.0f);  // ??
    DrawWingPart(-1.0f); // ??

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.35f);
    DrawBox(0.3f, 0.6f, 0.1f, 0.0f, 0.8f, 0.4f);
    glPopMatrix();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(0.0f, 3.0f, 0.0f);

    // A. ?? (Spine) - ?????
    glPushMatrix();
    DrawBox(0.8f, 7.0f, 0.3f, 0.15f, 0.15f, 0.15f);
    glPopMatrix();

    // B. ?? (Edges) - ?????????
    // ?? (?????????)
    glPushMatrix();
    glTranslatef(-0.6f, 0.0f, 0.0f);
    glScalef(1.0f, 1.0f, 0.5f);

    // ??????????
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

    glPopMatrix();

    // ?? (??)
    glPushMatrix();
    glTranslatef(0.6f, 0.0f, 0.0f);
    glScalef(1.0f, 1.0f, 0.5f);

    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // ???????

    //DrawTriPrism(0.4f, 6.5f, 0.6f, 0.7f, 0.7f, 0.7f);
    glPopMatrix();

    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 6.5f, 0.0f);

    // ????????
    glScalef(1.2f, 2.5f, 0.2f);
    DrawPyramid(0.7f, 1.0f, 0.6f, 0.6f, 0.6f);
    glPopMatrix();
}

//main part
void drawChestTurbine() {
    glPushMatrix();
    glColor3fv(CLR_GREY);
    glPushMatrix();
    glTranslatef(0, 0.05, 0);
    glRotatef(90, 1, 0, 0);
    drawCylinder(0.45f, 0.45f, 0.1f, 32, false);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0, 0.05f);
    glRotatef(turbineRotation, 0, 0, 1);
    glColor3f(0.2f, 0.2f, 0.2f);
    for (int i = 0; i < 12; i++) {//fan
        glPushMatrix();
        glRotatef(i * 30.0f, 0, 0, 1);
        glTranslatef(0.25f, 0, 0);
        glRotatef(30, 1, 0, 0);
        drawBox(0.3f, 0.08f, 0.02f);
        glPopMatrix();
    }
    glPopMatrix();

    glMaterialfv(GL_FRONT, GL_EMISSION, highEmit);
    glColor3fv(CLR_GLOW);
    gluSphere(gluNewQuadric(), 0.15f, 16, 16);//core
    glMaterialfv(GL_FRONT, GL_EMISSION, noEmit);
    glPopMatrix();
}

void drawHead() {
    glPushMatrix();
    glTranslatef(0, 3.0f, 0);
    glColor3fv(CLR_NAVY);

    glPushMatrix();
    glTranslatef(0, -0.2, 0);
    drawCylinder(0.30f, 0.40f, 0.25f, 16);//neck
    glColor3fv(CLR_WHITE);
    glTranslatef(0, 0.065, -0.1);
    drawCylinder(0.29f, 0.27f, 0.2f, 16);
    glPopMatrix();

    glColor3fv(CLR_NAVY);//head
    glTranslatef(0, 0.15f, 0);
    drawSphere(0.26f, 24, 24);

    glColor3fv(CLR_RED);//top
    glTranslatef(0, 0.3f, 0);
    drawBox(0.1f, 0.05f, 0.5f);

    glMaterialfv(GL_FRONT, GL_EMISSION, highEmit);//eye
    glColor3fv(CLR_YELLOW);
    glTranslatef(0, -0.25, 0.25f);
    glBegin(GL_POLYGON);
    glNormal3f(0, 0, 1);
    glVertex3f(-0.2f, 0.15f, -0.1f);
    glVertex3f(-0.1f, 0.0f, 0.01f);
    glVertex3f(0.0f, 0.05f, 0.01f);
    glVertex3f(0.0f, 0.1f, 0.01f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex3f(0.0f, 0.05f, 0.01f);
    glVertex3f(0.1f, 0.0f, 0.01f);
    glVertex3f(0.2f, 0.15f, -0.1f);
    glVertex3f(0.0f, 0.1f, 0.01f);
    glEnd();
    glMaterialfv(GL_FRONT, GL_EMISSION, noEmit);

    glPopMatrix();
}

void drawShoulder(bool isLeft) {
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures[currentTexture]);
    drawBox(1.2f, 0.8f, 1.0f);
    glDisable(GL_TEXTURE_2D);

    glTranslatef(0, 0.41f, 0);
    glColor3fv(CLR_WHITE);
    drawBox(0.4f, 0.02f, 1.02f);
    glTranslatef(0, 0.01f, 0);
    glColor3fv(CLR_RED);
    drawBox(0.2f, 0.02f, 1.02f);
    glPopMatrix();
}

void drawBody() {
    glPushMatrix();
    glTranslatef(0, 1.2f, 0);

    glPushMatrix();//abdomen
    glTranslatef(0, 0, 0);
    glScalef(0.9f, 1.0f, 0.7f);
    drawAbdomenDetail();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 1, 0);

    glPushMatrix();//chest
    glScalef(1.8f, 1.2f, 1.1f);
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D, textures[currentTexture]);
    drawBox(1.0f, 0.9f, 1.0f);
    glDisable(GL_TEXTURE_2D);
    glColor3fv(CLR_GREY);
    drawBox(0.8f, 1.0f, 0.8f);
    glPopMatrix();

    glPushMatrix();//left neck
    glColor3fv(CLR_WHITE);
    glTranslatef(0.6f, 0.6f, 0);
    glRotatef(20, 0, 0, 1);
    drawBox(0.5f, 0.4f, 0.6f);
    glPopMatrix();

    glPushMatrix();//right neck
    glTranslatef(-0.6f, 0.6f, 0);
    glRotatef(-20, 0, 0, 1);
    drawBox(0.5f, 0.4f, 0.6f);
    glPopMatrix();

    glTranslatef(0, 0.0f, 0.55f);
    drawChestTurbine();
    glPopMatrix();

    glPopMatrix();
}

void drawJoint(float diameter, float height) {
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    glRotatef(90, 0, 0, 1);
    drawCylinder(diameter, diameter, height, 8);
    drawCylinder(diameter / 4 * 3, diameter / 4 * 3, height + diameter * 0.2, 8);
    glPopMatrix();
}

void drawArmGuard(float side) {
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D, textures[currentTexture]);
    glRotatef(side * -3.5, 0, 0, 1);
    glTranslatef(side * 0.2f, 0, 0);
    drawBox(0.05f, 0.65f, 0.35f);
    glDisable(GL_TEXTURE_2D);

    glColor3fv(CLR_RED);
    glPushMatrix();
    glTranslatef(side * -0.0475f, 0, 0.195f);
    glRotatef(side * -60, 0, 1, 0);
    drawBox(0.05f, 0.6499f, 0.15f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(side * -0.0475f, 0, -0.195f);
    glRotatef(side * 60, 0, 1, 0);
    drawBox(0.05f, 0.6499f, 0.15f);
    glPopMatrix();

    glPopMatrix();
}

void drawPalm() {
    drawBox(0.2f, 0.3f, 0.2f);
    glTranslatef(0, 0.0f, 0);

    Vec3 vertices3[8];
    vertices3[0].x = -0.1f; vertices3[0].y = -0.15f; vertices3[0].z = 0.1;
    vertices3[1].x = 0.1f; vertices3[1].y = -0.15f; vertices3[1].z = 0.1;
    vertices3[2].x = -0.05f; vertices3[2].y = -0.25f; vertices3[2].z = 0.1;
    vertices3[3].x = 0.05f; vertices3[3].y = -0.25f; vertices3[3].z = 0.1;
    vertices3[4].x = -0.1f; vertices3[4].y = -0.15f; vertices3[4].z = -0.1f;
    vertices3[5].x = 0.1f; vertices3[5].y = -0.15f; vertices3[5].z = -0.1f;
    vertices3[6].x = -0.05f; vertices3[6].y = -0.25f; vertices3[6].z = -0.1f;
    vertices3[7].x = 0.05f; vertices3[7].y = -0.25f; vertices3[7].z = -0.1f;

    drawCustomCuboid(vertices3);

    glPushMatrix();
    glTranslatef(0, 0, -0.1f);

    Vec3 vertices[8];
    vertices[0].x = -0.1f; vertices[0].y = 0.15f; vertices[0].z = 0;
    vertices[1].x = 0.1f; vertices[1].y = 0.15f; vertices[1].z = 0;
    vertices[2].x = -0.1f; vertices[2].y = -0.15f; vertices[2].z = 0;
    vertices[3].x = 0.1f; vertices[3].y = -0.15f; vertices[3].z = 0;
    vertices[4].x = -0.05f; vertices[4].y = 0.15f; vertices[4].z = -0.1f;
    vertices[5].x = 0.05f; vertices[5].y = 0.15f; vertices[5].z = -0.1f;
    vertices[6].x = -0.05f; vertices[6].y = -0.15f; vertices[6].z = -0.1f;
    vertices[7].x = 0.05f; vertices[7].y = -0.15f; vertices[7].z = -0.1f;

    Vec3 vertices2[8];
    vertices2[0].x = -0.1f; vertices2[0].y = -0.15f; vertices2[0].z = 0;
    vertices2[1].x = 0.1f; vertices2[1].y = -0.15f; vertices2[1].z = 0;
    vertices2[2].x = -0.05f; vertices2[2].y = -0.25f; vertices2[2].z = 0;
    vertices2[3].x = 0.05f; vertices2[3].y = -0.25f; vertices2[3].z = 0;
    vertices2[4].x = -0.05f; vertices2[4].y = -0.15f; vertices2[4].z = -0.1f;
    vertices2[5].x = 0.05f; vertices2[5].y = -0.15f; vertices2[5].z = -0.1f;
    vertices2[6].x = -0.05f; vertices2[6].y = -0.25f; vertices2[6].z = -0.05f;
    vertices2[7].x = 0.05f; vertices2[7].y = -0.25f; vertices2[7].z = -0.05f;

    drawCustomCuboid(vertices);
    drawCustomCuboid(vertices2);

    glTranslatef(0, 0, 0.2f);
    glRotatef(180, 0, 1, 0);
    drawCustomCuboid(vertices);
    drawCustomCuboid(vertices2);

    glPopMatrix();

}

void drawFinger(int fingerIndex, bool isLeft) {
    if (isLeft)
        glRotatef(fingers[0 + (fingerIndex * 3)].left, 0, 0, 1);

    else
        glRotatef(fingers[0 + (fingerIndex * 3)].right, 0, 0, 1);

    // Joint
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    drawJoint(0.02f, 0.05f);
    glPopMatrix();

    // Finger Base
    glTranslatef(0.0f, -0.07275f, 0.0f);
    drawBox(0.06f, 0.12f, 0.06f);

    // Joint
    glTranslatef(0.0f, -0.07275f, 0.0f);

    if (isLeft)
        glRotatef(fingers[1 + (fingerIndex * 3)].left, 0, 0, 1);

    else
        glRotatef(fingers[1 + (fingerIndex * 3)].right, 0, 0, 1);

    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    drawJoint(0.0175f, 0.04f);
    glPopMatrix();

    // Finger Middle
    glTranslatef(0.0f, -0.05275f, 0.0f);
    drawBox(0.04f, 0.09f, 0.04f);

    if (fingerIndex == 4)
        return;

    // Joint
    glTranslatef(0.0f, -0.05275f, 0.0f);

    if (isLeft)
        glRotatef(fingers[2 + (fingerIndex * 3)].left, 0, 0, 1);

    else
        glRotatef(fingers[2 + (fingerIndex * 3)].right, 0, 0, 1);

    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    drawJoint(0.015f, 0.03f);
    glPopMatrix();

    // Finger Tip
    glTranslatef(0.0f, -0.03775f, 0.0f);
    drawBox(0.03f, 0.06f, 0.03f);
}

void drawArm(bool isLeft) {
    float side = isLeft ? -1.0f : 1.0f;

    glPushMatrix();//joint
    glTranslatef(side * 1.4f, 2.3f, 0);
    glColor3fv(CLR_GREY);
    drawSphere(0.5f, 16, 16);

    if (isLeft) {
        glRotatef(upperArmX.left, 0, 1, 0);
        glRotatef(upperArmY.left, 1, 0, 0);
    }

    else {
        glRotatef(upperArmX.right, 0, 1, 0);
        glRotatef(upperArmY.right, 1, 0, 0);
    }

    glPushMatrix();//shoulder
    glRotatef(isLeft ? -20 : 20, 0, 0, 1);
    glTranslatef(0, 0.4f, 0);
    drawShoulder(isLeft);
    glPopMatrix();

    glColor3f(1.0f, 1.0f, 1.0f);//upper arm
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures[currentTexture]);
    glTranslatef(0, -0.8f, 0);
    drawCylinder(0.25f, 0.3f, 0.8f, 8);
    glDisable(GL_TEXTURE_2D);

    glColor3fv(CLR_GREY);//joint
    glTranslatef(0, -0.4f, 0);

    if (isLeft)
        glRotatef(lowerArm.left, 1, 0, 0);

    else
        glRotatef(lowerArm.right, 1, 0, 0);

    drawJoint(0.32f, 0.5f);

    glColor3f(1.0f, 1.0f, 1.0f);//lower arm
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures[currentTexture]);
    glTranslatef(0, -0.6f, 0);
    drawCylinder(0.175f, 0.25f, 0.85f, 8);

    if (isLeft)
        glRotatef(wristY.left, 0, 1, 0);

    else
        glRotatef(wristY.right, 0, 1, 0);

    drawArmGuard(side);
    glDisable(GL_TEXTURE_2D);

    // Wrist
    glColor3fv(CLR_GREY);
    glTranslatef(0, -0.4f, 0);
    drawSphere(0.2f, 16, 16);

    if (isLeft)
        glRotatef(wristZ.left, 0, 0, 1);

    else
        glRotatef(wristZ.right, 0, 0, 1);


    // Palm
    glColor3f(1.0f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures[currentTexture]);
    glTranslatef(0, -0.25f, 0);
    drawPalm();
    glDisable(GL_TEXTURE_2D);

    glPushMatrix();//finger
    glTranslatef(0.0f, -0.2625f, 0.0f);
    glColor3fv(CLR_GREY);
    for (int i = 0; i < 4; i++) {
        float zPos = -0.12f + (i * 0.08f);
        glPushMatrix();
        glTranslatef(0.0f, 0.0f, zPos);
        drawFinger(i, isLeft);
        glPopMatrix();
    }

    glPushMatrix();
    glTranslatef(0.0f, 0.05f, 0.18f);
    glRotatef(-60, 1, 0, 0);

    drawFinger(4, isLeft);
    glPopMatrix();

    glPopMatrix();
    glPopMatrix();
}

void drawLeg(bool isLeft) {
    float side = isLeft ? -1.0f : 1.0f;

    glPushMatrix();//upper joint
    glTranslatef(side * 0.5f, 0.7f, 0.0f);
    glColor3fv(CLR_GREY);
    drawSphere(0.35f, 16, 16);
    glRotatef(isLeft ? -5 : 5, 0, 0, 1);

    glPushMatrix();//upper leg
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D, textures[currentTexture]);
    glTranslatef(0, -0.6f, 0);
    drawBox(0.55f, 1.0f, 0.6f);
    glDisable(GL_TEXTURE_2D);

    glPushMatrix();//decoration
    glTranslatef(side * 0.28f, 0, 0);
    glColor3fv(CLR_WHITE);
    drawBox(0.05f, 0.8f, 0.2f);
    glTranslatef(side * 0.01f, 0, 0);
    glColor3fv(CLR_RED);
    drawBox(0.05f, 0.6f, 0.08f);
    glPopMatrix();

    glColor3fv(CLR_GREY);//Hydraulic
    glPushMatrix();
    glTranslatef(0, 0, -0.3f);
    drawCylinder(0.1f, 0.1f, 0.6f, 8);
    glPopMatrix();

    glTranslatef(0, -0.6f, 0);//mid joint
    glColor3fv(CLR_GREY);
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    drawJoint(0.2f, 0.7f);
    glPopMatrix();

    glColor3f(1.0f, 1.0f, 1.0f);//joint cap
    glBindTexture(GL_TEXTURE_2D, textures[currentTexture]);
    glEnable(GL_TEXTURE_2D);
    glPushMatrix();
    glTranslatef(0, 0.05f, 0.35f);
    glRotatef(-10, 1, 0, 0);
    drawBox(0.35f, 0.4f, 0.15f);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0, 0.05f, -0.15f);
    drawBox(0.35f, 0.4f, 0.15f);
    glPopMatrix();

    glTranslatef(0, -0.7f, 0);//lower leg
    glPushMatrix();
    glScalef(1.0f, 1.0f, 0.8f);
    drawBox(0.5f, 1.1f, 0.6f);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    glColor3fv(CLR_GREY);//decoration
    glPushMatrix();
    glTranslatef(0, -0.2f, 0.31f);
    drawBox(0.2f, 0.4f, 0.05f);

    glColor3f(0.1f, 0.1f, 0.1f);
    for (float y = -0.1f; y < 0.2f; y += 0.1f) {
        glPushMatrix();
        glTranslatef(0, y, 0.03f);
        drawBox(0.18f, 0.02f, 0.02f);
        glPopMatrix();
    }
    glPopMatrix();

    glTranslatef(0, -0.6f, 0);//lower joint
    glColor3fv(CLR_GREY);
    drawSphere(0.25f, 16, 16);

    glTranslatef(0, -0.25f, 0.25f);//back
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D, textures[currentTexture]);
    glPushMatrix();
    glTranslatef(0, 0, -0.3f);
    drawBox(0.5f, 0.4f, 0.4f);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    glColor3fv(CLR_GREY);//mid
    glPushMatrix();
    glTranslatef(0, -0.1f, 0.0f);
    drawBox(0.4f, 0.2f, 0.5f);
    glPopMatrix();

    glColor3f(1.0f, 1.0f, 1.0f);//top
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures[currentTexture]);
    
    glPushMatrix();
    glTranslatef(0, -0.1f, 0.35f);
    drawBox(0.55f, 0.3f, 0.5f);
    glDisable(GL_TEXTURE_2D);

    glColor3fv(CLR_GREY);//toe
    glTranslatef(0, -0.15f, 0.26f);
    drawBox(0.4f, 0.1f, 0.1f);
    glPopMatrix();

    glPopMatrix();//upper leg
    glPopMatrix();//upper joint
    glPopMatrix();//main
}

//render
void drawLightIndicator()
{
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 0.0f);
    glTranslatef(diffuseLightPosition[0], diffuseLightPosition[1], diffuseLightPosition[2]);

    GLUquadric* lightQuad = gluNewQuadric();
    gluSphere(lightQuad, 0.25f, 30, 30);
    gluDeleteQuadric(lightQuad);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (isOrtho)
        glOrtho(-4, 4, -2.25f, 2.25f, -25, 25);

    else
        gluPerspective(90.0f / (16.0f / 9), 16.0f / 9, 0.1, 25);

    forwardX = -cos(DegToRad(pitch)) * sin(DegToRad(yaw));
    forwardY = sin(DegToRad(pitch));
    forwardZ = cos(DegToRad(pitch)) * cos(DegToRad(yaw));

    glRotatef(pitch, 1, 0, 0);
    glRotatef(yaw, 0, 1, 0);
    glTranslatef(posX, posY, posZ);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_POSITION, diffuseLightPosition);

    if (toggleLight) {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
    }
    else
        glDisable(GL_LIGHTING);

    drawBody();
    drawHead();
    drawArm(true);
    drawArm(false);
    drawLeg(true);
    drawLeg(false);
    glDisable(GL_TEXTURE_2D);

    if (isSwordActive) {
        glPushMatrix();
        glTranslatef(1.2f, 0.0f, 0.5f);
        glRotatef(45, 1, 0, 0);
        glRotatef(5, 0, 0, 1);
        glRotatef(90, 0, 1, 0);
        glScalef(0.3f, 0.3f, 0.3f);
        DrawSword();
        glPopMatrix();
    }

    drawLightIndicator();
}

void resetPosition() {
    for (int i = 0; i < rotationCount; i++) {
        rotations[i].right = 0;
        rotations[i].left = 0;
    }
}

void playBlockAnimation() {
    if (animationTime < 1) {
        animationTime += 0.025f;

        rotations[0] = { -80 * animationTime, -80 * animationTime, 30, -180 };
        rotations[1] = { 50 * animationTime, -60 * animationTime, 90, -60 };
        rotations[2] = { -60 * animationTime, -60 * animationTime, 0, -80 };
        rotations[3] = { 0, 0, 90, -90 };
        rotations[4] = { 0, 0, 90, -90 };
        rotations[5] = { 90 * animationTime, -90 * animationTime, 0, -90 };
        rotations[6] = { 90 * animationTime, -90 * animationTime, 0, -90 };
        rotations[7] = { 90 * animationTime, -90 * animationTime, 0, -90 };
        rotations[8] = { 90 * animationTime, -90 * animationTime, 0, -90 };
        rotations[9] = { 90 * animationTime, -90 * animationTime, 0, -90 };
        rotations[10] = { 90 * animationTime, -90 * animationTime, 0, -90 };
        rotations[11] = { 90 * animationTime, -90 * animationTime, 0, -90 };
        rotations[12] = { 90 * animationTime, -90 * animationTime, 0, -90 };
        rotations[13] = { 90 * animationTime, -90 * animationTime, 0, -90 };
        rotations[14] = { 90 * animationTime, -90 * animationTime, 0, -90 };
        rotations[15] = { 90 * animationTime, -90 * animationTime, 0, -90 };
        rotations[16] = { 90 * animationTime, -90 * animationTime, 0, -90 };
        rotations[17] = { 90 * animationTime, -90 * animationTime, 0, -90 };
        rotations[18] = { 90 * animationTime, -90 * animationTime, 0, -90 };
    }
}

void update() {
    turbineRotation += 5.0f;
    if (turbineRotation > 360) turbineRotation -= 360;

    if (animationIndex == 0 && startAnimation)
        playBlockAnimation();

    upperArmY = rotations[0];
    upperArmX = rotations[1];
    lowerArm = rotations[2];
    wristZ = rotations[3];
    wristY = rotations[4];
    fingers[0] = rotations[5];
    fingers[1] = rotations[6];
    fingers[3] = rotations[7];
    fingers[3] = rotations[8];
    fingers[4] = rotations[9];
    fingers[5] = rotations[10];
    fingers[6] = rotations[11];
    fingers[7] = rotations[12];
    fingers[8] = rotations[13];
    fingers[9] = rotations[14];
    fingers[10] = rotations[15];
    fingers[11] = rotations[16];
    fingers[12] = rotations[17];
    fingers[13] = rotations[18];

    finger1 = fingers[0];
    fingerJoint1 = fingers[1];
    fingerTip1 = fingers[2];
    finger2 = fingers[3];
    fingerJoint2 = fingers[4];
    fingerTip2 = fingers[5];
    finger3 = fingers[6];
    fingerJoint3 = fingers[7];
    fingerTip3 = fingers[8];
    finger4 = fingers[9];
    fingerJoint4 = fingers[10];
    fingerTip4 = fingers[11];
    thumb = fingers[12];
    thumbTip = fingers[13];
}

LRESULT WINAPI WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_DESTROY: PostQuitMessage(0); break;
    case WM_KEYDOWN:
        switch (wParam) {
        case VK_ESCAPE:
            PostQuitMessage(0);
            break;

        case 'D':
            // Rotate 90 deg along y-axis
            tempDistance = sqrt((forwardX * forwardX) + (forwardZ * forwardZ));
            posX -= moveSpeed * forwardZ / tempDistance;
            posZ += moveSpeed * forwardX / tempDistance;
            break;

        case 'A':
            // Rotate 90 deg along y-axis
            tempDistance = sqrt((forwardX * forwardX) + (forwardZ * forwardZ));
            posX += moveSpeed * forwardZ / tempDistance;
            posZ -= moveSpeed * forwardX / tempDistance;
            break;

        case 'W':
            posX += moveSpeed * forwardX;
            posY += moveSpeed * forwardY;
            posZ += moveSpeed * forwardZ;
            break;

        case 'S':
            posX -= moveSpeed * forwardX;
            posY -= moveSpeed * forwardY;
            posZ -= moveSpeed * forwardZ;
            break;

        case 'K':
            pitch += rotateSpeed;
            if (pitch > 90) pitch = 90;
            else if (pitch < -90) pitch = -90;

            break;

        case 'I':
            pitch -= rotateSpeed;
            if (pitch > 90) pitch = 90;
            else if (pitch < -90) pitch = -90;

            break;

        case 'L':
            yaw += rotateSpeed;
            if (yaw >= 180) yaw -= 360;
            else if (yaw < -180) yaw += 360;

            break;

        case 'J':
            yaw -= rotateSpeed;
            if (yaw >= 180) yaw -= 360;
            else if (yaw < -180) yaw += 360;

            break;

        case 'F': diffuseLightPosition[0] -= moveSpeed; break;
        case 'H': diffuseLightPosition[0] += moveSpeed; break;
        case 'Y': diffuseLightPosition[1] += moveSpeed; break;
        case 'R': diffuseLightPosition[1] -= moveSpeed; break;
        case 'T': diffuseLightPosition[2] -= moveSpeed; break;
        case 'G': diffuseLightPosition[2] += moveSpeed; break;

        case 'P':
            if (toggleLight) toggleLight = false;
            else toggleLight = true;
            break;

        case 'Q':
            isSwordActive = !isSwordActive;
            break;
        
        case 'E':
            currentTexture++;
			if (currentTexture > 1) currentTexture = 0;
            break;

        case 'M':
            rotateLeft = !rotateLeft;
            break;

        case 'O':
            isOrtho = !isOrtho;

        case VK_UP:
            if (!rotateLeft) {
                rotations[rotationIndex].right += objRotateSpeed;

                if (rotations[rotationIndex].right > rotations[rotationIndex].limitPos)
                    rotations[rotationIndex].right = rotations[rotationIndex].limitPos;
            }

            else {
                rotations[rotationIndex].left += objRotateSpeed;

                if (rotations[rotationIndex].left > rotations[rotationIndex].limitPos)
                    rotations[rotationIndex].left = rotations[rotationIndex].limitPos;
            }
            break;

        case VK_DOWN:
            if (!rotateLeft) {
                rotations[rotationIndex].right -= objRotateSpeed;

                if (rotations[rotationIndex].right < rotations[rotationIndex].limitNeg)
                    rotations[rotationIndex].right = rotations[rotationIndex].limitNeg;
            }

            else {
                    rotations[rotationIndex].left -= objRotateSpeed;

                if (rotations[rotationIndex].left < rotations[rotationIndex].limitNeg)
                    rotations[rotationIndex].left = rotations[rotationIndex].limitNeg;
            }
            break;

        case VK_LEFT:
            rotationIndex--;
            if (rotationIndex < 0)
                rotationIndex = rotationCount - 1;
            break;

        case VK_RIGHT:
            rotationIndex++;
            if (rotationIndex >= rotationCount)
                rotationIndex = 0;
            break;

        case 'Z':
            animationTime = 0;
            animationIndex--;
            if (animationIndex < 0)
                animationIndex = animationCount - 1;

            startAnimation = false;
            break;
            
        case 'X':
            startAnimation = !startAnimation;
            animationTime = 0;

            if (!startAnimation)
                resetPosition();

            break;

        case 'C':
            animationTime = 0;
            animationIndex++;
            if (animationIndex >= animationCount)
                animationIndex = 0;

            startAnimation = false;
            break;

        }break;
    default: return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, WindowProcedure, 0, 0, hInst, NULL, LoadCursor(NULL, IDC_ARROW), NULL, NULL, WINDOW_TITLE, NULL };
    RegisterClassEx(&wc);
    HWND hWnd = CreateWindow(WINDOW_TITLE, WINDOW_TITLE, WS_OVERLAPPEDWINDOW, 100, 100, windowWidth, windowWidth * 9.0f / 16.0f, NULL, NULL, hInst, NULL);
    HDC hdc = GetDC(hWnd);

    // Pixel Format
    PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0 };
    SetPixelFormat(hdc, ChoosePixelFormat(hdc, &pfd), &pfd);
    wglMakeCurrent(hdc, wglCreateContext(hdc));

    // OpenGL Init
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    loadTexture();

    ShowWindow(hWnd, nCmdShow);
    MSG msg = { 0 };
    while (true) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        update();
        display();
        SwapBuffers(hdc);
        Sleep(16);
    }
    //UnregisterClass(WINDOW_TITLE, wc.hInstance);
    return 0;
}