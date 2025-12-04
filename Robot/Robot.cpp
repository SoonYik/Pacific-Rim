#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <math.h>
#include <cmath>

#pragma comment (lib, "OpenGL32.lib")
#pragma comment (lib, "GLU32.lib")
#define WINDOW_TITLE "Pacific Rim"

float headYaw = 0, headPitch = 0;
float leftShoulder = 0, leftElbow = 0;
float rightShoulder = 0, rightElbow = 0;
float leftLeg = 0, leftKnee = 0;
float rightLeg = 0, rightKnee = 0;
float camYaw = 0, camPitch = 15, camDist = 10;
float robotColor[3] = { 0.2f, 0.6f, 1.0f };
float torsoScale = 1.0f;
bool attackAnimation = false;
float attackAngle = 0.0f;

LRESULT WINAPI WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    const float d = 5.0f;
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            PostQuitMessage(0);
            break;
        
        case '1': headYaw += 5; break;
        case '2': headYaw -= 5; break;

            // Arms
        case 'Q': leftShoulder += 5; break;
        case 'W': leftShoulder -= 5; break;
        case 'A': leftElbow += 5; break;
        case 'S': leftElbow -= 5; break;

        case 'E': rightShoulder += 5; break;
        case 'R': rightShoulder -= 5; break;
        case 'D': rightElbow += 5; break;
        case 'F': rightElbow -= 5; break;

            // Legs
        case 'Z': leftLeg += 5; break;
        case 'X': leftLeg -= 5; break;
        case 'C': leftKnee += 5; break;
        case 'V': leftKnee -= 5; break;

        case 'T': rightLeg += 5; break;
        case 'Y': rightLeg -= 5; break;
        case 'G': rightKnee += 5; break;
        case 'H': rightKnee -= 5; break;

            // Special attack animation
        case 'P': attackAnimation = !attackAnimation; break;

            // Customization
        case 'K': robotColor[0] = (float)rand() / RAND_MAX; break;
        case 'L': torsoScale += 0.1f; break;
        case 'O': torsoScale -= 0.1f; break;
        }break;
    default:
        break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool initPixelFormat(HDC hdc)
{
    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

    pfd.cAlphaBits = 8;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 0;

    pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;

    pfd.iLayerType = PFD_MAIN_PLANE;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;

    // choose pixel format returns the number most similar pixel format available
    int n = ChoosePixelFormat(hdc, &pfd);

    // set pixel format returns whether it sucessfully set the pixel format
    if (SetPixelFormat(hdc, n, &pfd))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void drawBox(float sx, float sy, float sz) {
    glPushMatrix();
    glScalef(sx, sy, sz);
    glBegin(GL_QUADS);
    // +Z
    glNormal3f(0, 0, 1);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    // -Z
    glNormal3f(0, 0, -1);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    // +Y
    glNormal3f(0, 1, 0);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    // -Y
    glNormal3f(0, -1, 0);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    // +X
    glNormal3f(1, 0, 0);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    // -X
    glNormal3f(-1, 0, 0);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glEnd();
    glPopMatrix();
}

void setCamera()
{
    glTranslatef(0, 0, -camDist);
    glRotatef(camPitch, 1, 0, 0);
    glRotatef(camYaw, 0, 1, 0);
}

void drawArm(float shoulderAngle, float elbowAngle, bool leftSide)
{
    float side = leftSide ? -1.0f : 1.0f;

    glPushMatrix();
    glTranslatef(side * 1.2f, 0.6f, 0);
    glRotatef(shoulderAngle, 1, 0, 0);

    // Upper arm
    drawBox(0.4f, 1.2f, 0.4f);

    glTranslatef(0, -0.8f, 0);
    glRotatef(elbowAngle, 1, 0, 0);

    // Lower arm
    drawBox(0.35f, 1.1f, 0.35f);

    // Weapon / forearm blade
    glTranslatef(0, -0.8f, 0.3f);
    glColor3f(1, 0, 0);
    drawBox(0.15f, 0.6f, 0.9f);

    glPopMatrix();
}

void drawLeg(float thighAngle, float kneeAngle, bool leftSide)
{
    float side = leftSide ? -0.6f : 0.6f;

    glPushMatrix();
    glTranslatef(side, -1.8f, 0);
    glRotatef(thighAngle, 1, 0, 0);

    drawBox(0.6f, 1.5f, 0.6f);

    glTranslatef(0, -1.0f, 0);
    glRotatef(kneeAngle, 1, 0, 0);

    drawBox(0.5f, 1.4f, 0.5f);

    glPopMatrix();
}

void drawRobot()
{
    glColor3fv(robotColor);

    // Torso
    glPushMatrix();
    glScalef(1.0f * torsoScale, 1.5f * torsoScale, 0.7f * torsoScale);
    drawBox(1, 1, 1);
    glPopMatrix();

    // Head
    glPushMatrix();
    glTranslatef(0, 1.6f, 0);
    glRotatef(headYaw, 0, 1, 0);
    glRotatef(headPitch, 1, 0, 0);
    drawBox(0.7f, 0.7f, 0.7f);
    glPopMatrix();

    // Arms
    drawArm(leftShoulder, leftElbow, true);
    drawArm(rightShoulder, rightElbow, false);

    // Legs
    drawLeg(leftLeg, leftKnee, true);
    drawLeg(rightLeg, rightKnee, false);
}

void updateAnimation()
{
    if (attackAnimation) {
        attackAngle += 2.0f;
        if (attackAngle > 60) attackAngle = -20;

        rightShoulder = attackAngle;
        rightElbow = -attackAngle * 0.7f;
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    setCamera();

    drawRobot();
}

void initGL() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.2f, 0.2f, 0.25f, 1.0f);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow)
{
    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpfnWndProc = WindowProcedure;
    wc.lpszClassName = WINDOW_TITLE;
    wc.style = CS_HREDRAW | CS_VREDRAW;

    if (!RegisterClassEx(&wc)) return false;

    HWND hWnd = CreateWindow(WINDOW_TITLE, WINDOW_TITLE, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1200, 800,
        NULL, NULL, wc.hInstance, NULL);

    //	Initialize window for OpenGL
    HDC hdc = GetDC(hWnd);

    //	initialize pixel format for the window
    initGL();
    initPixelFormat(hdc);

    //	get an openGL context
    HGLRC hglrc = wglCreateContext(hdc);

    //	make context current
    if (!wglMakeCurrent(hdc, hglrc)) return false;
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glOrtho(-2,2,-2,2,-2,2);//left, right, bottom, top, Near, Far
    gluPerspective(60.0f, 1200.0f / 800.0f, 0.1f, 100.0f);//fovy, aspect, Near, Far
    //glFrustum(-0.1, 0.1, -0.1, 0.1, 0.05, 5);//xmin, xmax, ymin, ymax, -zmin, -zmax
    glMatrixMode(GL_MODELVIEW);

    //	End initialization
    ShowWindow(hWnd, nCmdShow);

    MSG msg;
    ZeroMemory(&msg, sizeof(msg));

    while (true)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT) break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        display();
        SwapBuffers(hdc);
    }

    UnregisterClass(WINDOW_TITLE, wc.hInstance);
    return true;
}