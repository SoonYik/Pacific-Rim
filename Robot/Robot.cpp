#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <math.h>

#pragma comment (lib, "OpenGL32.lib")
#pragma comment (lib, "GLU32.lib")
#define WINDOW_TITLE "Pacific Rim"

//Cam Position
float camPosX = 0.0f, camPosY = 10.0f, camPosZ = 12.0f;
float turbineRotation = 0.0f;

//Color
const float CLR_NAVY[] = { 0.15f, 0.25f, 0.40f };
const float CLR_GREY[] = { 0.30f, 0.35f, 0.40f };
const float CLR_RED[] = { 0.70f, 0.10f, 0.10f };
const float CLR_WHITE[] = { 0.90f, 0.90f, 0.90f };
const float CLR_YELLOW[] = { 1.00f, 0.80f, 0.00f };
const float CLR_GLOW[] = { 1.00f, 0.60f, 0.20f };
const float CLR_SILVER[] = { 0.75f, 0.75f, 0.75f };

//Light
float noEmit[] = { 0,0,0,1 };
float highEmit[] = { 1,0.6,0.2,1 };
GLfloat ambientLight[] = { 0.2,0.2,0.2 };
GLfloat diffuseLight[] = { 0.7,0.7,0.7 };
GLfloat diffuseLightPosition[] = { 0,0.8,-2,1 };

//shapes initialize
void drawBox(float w, float h, float d) {
    glPushMatrix();
    glScalef(w, h, d);
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1); glVertex3f(-0.5f, -0.5f, 0.5f); glVertex3f(0.5f, -0.5f, 0.5f); glVertex3f(0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f, 0.5f);//top
    glNormal3f(0, 0, -1); glVertex3f(-0.5f, -0.5f, -0.5f); glVertex3f(-0.5f, 0.5f, -0.5f); glVertex3f(0.5f, 0.5f, -0.5f); glVertex3f(0.5f, -0.5f, -0.5f);//bot
    glNormal3f(0, 1, 0); glVertex3f(-0.5f, 0.5f, -0.5f); glVertex3f(-0.5f, 0.5f, -0.5f); glVertex3f(0.5f, 0.5f, 0.5f); glVertex3f(0.5f, 0.5f, 0.5f);//left
    glNormal3f(0, -1, 0); glVertex3f(-0.5f, -0.5f, -0.5f); glVertex3f(0.5f, -0.5f, -0.5f); glVertex3f(0.5f, -0.5f, 0.5f); glVertex3f(-0.5f, -0.5f, 0.5f);//right
    glNormal3f(1, 0, 0); glVertex3f(0.5f, -0.5f, -0.5f); glVertex3f(0.5f, 0.5f, -0.5f); glVertex3f(0.5f, 0.5f, 0.5f); glVertex3f(0.5f, -0.5f, 0.5f);//front
    glNormal3f(-1, 0, 0); glVertex3f(-0.5f, -0.5f, -0.5f); glVertex3f(-0.5f, -0.5f, 0.5f); glVertex3f(-0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f, -0.5f);//back
    glEnd();
    glPopMatrix();
}

void drawCylinder(float base, float top, float height, int slices) {
    GLUquadricObj* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluCylinder(q, base, top, height, slices, 4);
    gluDeleteQuadric(q);
}

void drawSphere(float radius, float slices, float stack) {
    GLUquadricObj* s = gluNewQuadric();
    gluQuadricNormals(s, GLU_SMOOTH);
    gluSphere(s, radius, slices, stack);
    gluDeleteQuadric(s);
}

void drawDisk(float inner, float outer) {
    GLUquadricObj* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluDisk(q, inner, outer, 32, 4);
    gluDeleteQuadric(q);
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
    drawAbdomenLayer(0.4 ,0.25, 0.0);
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

//main part
void drawChestTurbine() {
    glPushMatrix();
    glColor3fv(CLR_GREY);
    drawCylinder(0.45f, 0.45f, 0.1f, 32);

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
    glRotatef(90, 1, 0, 0);
    drawCylinder(0.30f, 0.40f, 0.25f, 16);//neck
    glColor3fv(CLR_WHITE);
    glTranslatef(0, 0, -0.1);
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
    glColor3fv(CLR_NAVY);
    drawBox(1.2f, 0.8f, 1.0f);
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
    glColor3fv(CLR_NAVY);
    drawBox(1.0f, 0.9f, 1.0f);
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

void drawArm(bool isLeft) {
    float side = isLeft ? -1.0f : 1.0f;

    glPushMatrix();//joint
    glTranslatef(side * 1.4f, 2.3f, 0);
    glColor3fv(CLR_GREY);
    drawSphere(0.4f, 16, 16);

    glPushMatrix();//shoulder
    glRotatef(isLeft ? -20 : 20, 0, 0, 1);
    glTranslatef(0, 0.4f, 0);
    drawShoulder(isLeft);
    glPopMatrix();

    glColor3fv(CLR_NAVY);//upper arm
    glTranslatef(0, -0.6f, 0);
    drawBox(0.5f, 0.8f, 0.5f);

    glColor3fv(CLR_GREY);//joint
    glTranslatef(0, -0.55f, 0);
    glPushMatrix();
    drawSphere(0.28f, 16, 16);
    glPopMatrix();

    glColor3fv(CLR_NAVY);//lower arm
    glTranslatef(0, -0.6f, 0);
    glPushMatrix();
    glTranslatef(0, 0.2f, 0);
    drawBox(0.6f, 0.4f, 0.6f);
    glPopMatrix();
    drawBox(0.45f, 1.0f, 0.45f);
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
    glTranslatef(0, -0.6f, 0);
    glColor3fv(CLR_NAVY);
    drawBox(0.55f, 1.0f, 0.6f);

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
    glTranslatef(0, 0.3f, -0.3f);
    glRotatef(90, 1, 0, 0);
    drawCylinder(0.1f, 0.1f, 0.6f, 8);
    glPopMatrix();

    glTranslatef(0, -0.6f, 0);//mid joint
    glColor3fv(CLR_GREY);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glTranslatef(0, 0, -0.35f);
    drawCylinder(0.2f, 0.2f, 0.7f, 16);
    glPopMatrix();

    glColor3fv(CLR_NAVY);//joint cap
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
    glColor3fv(CLR_NAVY);
    glPushMatrix();
    glScalef(1.0f, 1.0f, 0.8f);
    drawBox(0.5f, 1.1f, 0.6f);
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
    glColor3fv(CLR_NAVY);
    glPushMatrix();
    glTranslatef(0, 0, -0.3f);
    drawBox(0.5f, 0.4f, 0.4f);
    glPopMatrix();

    glColor3fv(CLR_GREY);//mid
    glPushMatrix();
    glTranslatef(0, -0.1f, 0.0f);
    drawBox(0.4f, 0.2f, 0.5f);
    glPopMatrix();

    glColor3fv(CLR_NAVY);//top
    glPushMatrix();
    glTranslatef(0, -0.1f, 0.35f);
    drawBox(0.55f, 0.3f, 0.5f);

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
    gluSphere(lightQuad, 0.5f, 30, 30);
    gluDeleteQuadric(lightQuad);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(0, 0, -camPosZ);
    glRotatef(camPosY, 1, 0, 0);
    glRotatef(camPosX, 0, 1, 0);

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_POSITION, diffuseLightPosition);

    drawBody();
    drawHead();
    drawArm(true);
    drawArm(false);
    drawLeg(true);
    drawLeg(false);
    drawLightIndicator();
}

void update() {
    turbineRotation += 5.0f;
    if (turbineRotation > 360) turbineRotation -= 360;
}

LRESULT WINAPI WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_DESTROY: PostQuitMessage(0); break;
    case WM_KEYDOWN:
        switch (wParam) {
        case VK_ESCAPE: PostQuitMessage(0); break;
        case VK_LEFT: camPosX -= 5; break;
        case VK_RIGHT: camPosX += 5; break;
        case VK_UP: camPosY -= 5; break;
        case VK_DOWN: camPosY += 5; break;
        case VK_ADD: camPosZ -= 1; break; 
        case VK_SUBTRACT: camPosZ += 1; break;
        case VK_SPACE: 
            diffuseLightPosition[0] = 0;
            diffuseLightPosition[1] = 0.8;
            diffuseLightPosition[2] = -2;
            camPosX = 0;
            camPosY = 10;
            camPosZ = 12;

        case '1': diffuseLightPosition[0] -= 0.1f; break; //left
        case '2': diffuseLightPosition[0] += 0.1f; break; //right
        case '3': diffuseLightPosition[1] += 0.1f; break; //up
        case '4': diffuseLightPosition[1] -= 0.1f; break; //down
        case '5': diffuseLightPosition[2] += 0.1f; break; //forward
        case '6': diffuseLightPosition[2] -= 0.1f; break; //backward
        }
        break;
    default: return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, WindowProcedure, 0, 0, hInst, NULL, LoadCursor(NULL, IDC_ARROW), NULL, NULL, WINDOW_TITLE, NULL };
    RegisterClassEx(&wc);
    HWND hWnd = CreateWindow(WINDOW_TITLE, WINDOW_TITLE, WS_OVERLAPPEDWINDOW, 100, 100, 1200, 800, NULL, NULL, hInst, NULL);
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
    glMatrixMode(GL_PROJECTION);
    gluPerspective(50.6272f, 1.777f, 1.0f, 50.0f);
    glMatrixMode(GL_MODELVIEW);

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