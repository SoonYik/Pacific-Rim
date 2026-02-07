#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <cmath>
#include <cstdio> 

#include "ShapeFunction.h" 

#pragma comment (lib, "OpenGL32.lib")
#pragma comment (lib, "GLU32.lib")

#define WINDOW_TITLE "New OpenGL Project"

int questionToRender = 0; 

float cameraAngle = 0.0f;
float cameraRadius = 5.0f;
float cameraHeight = 2.0f;

float moveX = 0.0f;
float moveY = 0.0f;
float moveZ = 0.0f;
float rotationX = 0.0f;
float rotationY = 0.0f;
float rotationZ = 0.0f;
const float ROTATE_SPEED = 2.0f;
const float MOVE_SPEED = 0.01f;



void display();
void updateCamera();
bool initPixelFormat(HDC hdc);
LRESULT WINAPI WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


void updateCamera() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    float camX = cameraRadius * sin(cameraAngle);
    float camZ = cameraRadius * cos(cameraAngle);
    gluLookAt(camX, cameraHeight, camZ,
        0.0f, 0.0f, 0.0f, 
        0.0f, 1.0f, 0.0f); 
}

bool initPixelFormat(HDC hdc) {
    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
    pfd.cAlphaBits = 8;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
    pfd.iLayerType = PFD_MAIN_PLANE;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;

    int n = ChoosePixelFormat(hdc, &pfd);
    return SetPixelFormat(hdc, n, &pfd);
}


LRESULT WINAPI WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_KEYDOWN:
        switch (wParam) {
        case '1': questionToRender = 1; break; 
        case '2': questionToRender = 2; break; 
        case VK_ESCAPE: PostQuitMessage(0); break;

            // 摄像机控制
        case VK_UP: cameraHeight += 0.1f; break;
        case VK_DOWN: cameraHeight -= 0.1f; break;
        case VK_RIGHT: cameraAngle += 0.05f; break;
        case VK_LEFT: cameraAngle -= 0.05f; break;
        case VK_NEXT: cameraRadius += 0.1f; break; // Page Down
        case VK_PRIOR: cameraRadius -= 0.1f; if (cameraRadius < 0.1f) cameraRadius = 0.1f; break; // Page Up

            // object move (W, S, A, D) & rotate (T, G, F, H)
        case 'W': case 'w': moveY += MOVE_SPEED; break;
        case 'S': case 's': moveY -= MOVE_SPEED; break;
        case 'A': case 'a': moveX -= MOVE_SPEED; break;
        case 'D': case 'd': moveX += MOVE_SPEED; break;
        case 'T': case 't': rotationX -= ROTATE_SPEED; break;
        case 'G': case 'g': rotationX += ROTATE_SPEED; break;
        case 'F': case 'f': rotationY -= ROTATE_SPEED; break;
        case 'H': case 'h': rotationY += ROTATE_SPEED; break;


        default: break;
        }
        break;

    default: break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
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

    // 主握把 (细长)
    DrawCylinder(0.12f, 0.12f, 3.5f, 0.1f, 0.1f, 0.1f); 

    // 剑柄尾部的球 (Pommel)
    glPushMatrix();
    DrawSphere(0.25f, 0.4f, 0.4f, 0.4f); 
    glPopMatrix();

    // 握把中间的装饰环 (红圈)
    glPushMatrix();
    glTranslatef(0.0f, 1.5f, 0.0f);
    DrawCylinder(0.14f, 0.14f, 0.3f, 0.8f, 0.0f, 0.0f);
    glPopMatrix();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(0.0f, -1.0f, 0.0f); // 剑格位置

    DrawBox(1.2f, 1.0f, 0.6f, 0.2f, 0.2f, 0.2f); 

    // 左右两个机械翼
    DrawWingPart(1.0f);  // 右翼
    DrawWingPart(-1.0f); // 左翼

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.35f); 
    DrawBox(0.3f, 0.6f, 0.1f, 0.0f, 0.8f, 0.4f); 
    glPopMatrix();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(0.0f, 3.0f, 0.0f);

    // A. 剑脊 (Spine) - 黑色的骨架
    glPushMatrix();
    DrawBox(0.8f, 7.0f, 0.3f, 0.15f, 0.15f, 0.15f); 
    glPopMatrix();

    // B. 剑刃 (Edges) - 两侧锋利的银色部分
    // 左刃 (用三棱柱模拟锋利感)
    glPushMatrix();
    glTranslatef(-0.6f, 0.0f, 0.0f); 
    glScalef(1.0f, 1.0f, 0.5f); 

    // 旋转三棱柱让尖端朝外
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f); 
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f); 

    glPopMatrix();

    // 右刃 (镜像)
    glPushMatrix();
    glTranslatef(0.6f, 0.0f, 0.0f); 
    glScalef(1.0f, 1.0f, 0.5f);

    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // 反向，刃口朝右

    //DrawTriPrism(0.4f, 6.5f, 0.6f, 0.7f, 0.7f, 0.7f);
    glPopMatrix();

    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 6.5f, 0.0f); 

    // 巨大的三角形剑尖
    glScalef(1.2f, 2.5f, 0.2f);
    DrawPyramid(0.7f, 1.0f, 0.6f, 0.6f, 0.6f); 
    glPopMatrix();
}

void display() {
    glMatrixMode(GL_MODELVIEW);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    updateCamera();

    switch (questionToRender) {
    case 1: {
        glPushMatrix();
        glScalef(0.6f, 0.6f, 0.6f);

        glRotatef(rotationX, 1.0f, 0.0f, 0.0f);
        glRotatef(rotationY, 0.0f, 1.0f, 0.0f);
        glRotatef(rotationZ, 0.0f, 0.0f, 1.0f);

        DrawSword(); 

        glPopMatrix();
        break;
    }
    case 2: {

        break;
    }
    default:
        break;
    }
}



int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {
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

    HDC hdc = GetDC(hWnd);
    if (!initPixelFormat(hdc)) return false;

    HGLRC hglrc = wglCreateContext(hdc);
    if (!wglMakeCurrent(hdc, hglrc)) return false;

    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(60.0f, 1200.0f / 800.0f, 0.1f, 100.0f);


    ShowWindow(hWnd, nCmdShow);

    MSG msg;
    while (true) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        display();
        SwapBuffers(hdc);
    }

    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hglrc);
    ReleaseDC(hWnd, hdc);
    UnregisterClass(WINDOW_TITLE, wc.hInstance);
    return true;
}