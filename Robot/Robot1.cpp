#include <GL/glut.h>
#include <cmath>

// ------------------------------
// Robot joint angles
// ------------------------------
float headYaw = 0, headPitch = 0;

float leftShoulder = 0, leftElbow = 0;
float rightShoulder = 0, rightElbow = 0;

float leftLeg = 0, leftKnee = 0;
float rightLeg = 0, rightKnee = 0;

// Camera control
float camYaw = 0, camPitch = 15, camDist = 10;

// Customization
float robotColor[3] = { 0.2f, 0.6f, 1.0f };
float torsoScale = 1.0f;

// Animation flag
bool attackAnimation = false;
float attackAngle = 0.0f;

// ------------------------------------------------
// Helper: Render cube scaled
// ------------------------------------------------
void drawBox(float sx, float sy, float sz)
{
    glPushMatrix();
    glScalef(sx, sy, sz);
    glutSolidCube(1);
    glPopMatrix();
}

// ------------------------------------------------
// Camera
// ------------------------------------------------
void setCamera()
{
    glTranslatef(0, 0, -camDist);
    glRotatef(camPitch, 1, 0, 0);
    glRotatef(camYaw, 0, 1, 0);
}

// ------------------------------------------------
// Robot drawing (HIERARCHICAL MODEL)
// ------------------------------------------------
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

// ------------------------------------------------
// Animation
// ------------------------------------------------
void updateAnimation()
{
    if (attackAnimation) {
        attackAngle += 2.0f;
        if (attackAngle > 60) attackAngle = -20;

        rightShoulder = attackAngle;
        rightElbow = -attackAngle * 0.7f;
    }

    glutPostRedisplay();
}

// ------------------------------------------------
// Display
// ------------------------------------------------
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    setCamera();

    drawRobot();

    glutSwapBuffers();
}

// ------------------------------------------------
// Keyboard controls
// ------------------------------------------------
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        // Head
    case '1': headYaw += 5; break;
    case '2': headYaw -= 5; break;

        // Arms
    case 'q': leftShoulder += 5; break;
    case 'w': leftShoulder -= 5; break;
    case 'a': leftElbow += 5; break;
    case 's': leftElbow -= 5; break;

    case 'e': rightShoulder += 5; break;
    case 'r': rightShoulder -= 5; break;
    case 'd': rightElbow += 5; break;
    case 'f': rightElbow -= 5; break;

        // Legs
    case 'z': leftLeg += 5; break;
    case 'x': leftLeg -= 5; break;
    case 'c': leftKnee += 5; break;
    case 'v': leftKnee -= 5; break;

    case 't': rightLeg += 5; break;
    case 'y': rightLeg -= 5; break;
    case 'g': rightKnee += 5; break;
    case 'h': rightKnee -= 5; break;

        // Special attack animation
    case 'p': attackAnimation = !attackAnimation; break;

        // Customization
    case 'k': robotColor[0] = (float)rand() / RAND_MAX; break;
    case 'l': torsoScale += 0.1f; break;
    case 'o': torsoScale -= 0.1f; break;

        // Exit
    case 27: exit(0);
    }

    glutPostRedisplay();
}

// Mouse for camera
int lastX, lastY;
bool mouseDown = false;

void mouse(int btn, int state, int x, int y)
{
    if (btn == GLUT_LEFT_BUTTON)
        mouseDown = (state == GLUT_DOWN);

    lastX = x;
    lastY = y;
}

void motion(int x, int y)
{
    if (mouseDown) {
        camYaw += (x - lastX) * 0.4f;
        camPitch += (y - lastY) * 0.4f;
    }
    lastX = x;
    lastY = y;

    glutPostRedisplay();
}

void mouseWheel(int wheel, int dir, int x, int y)
{
    camDist -= dir;
    if (camDist < 4) camDist = 4;
    if (camDist > 30) camDist = 30;
    glutPostRedisplay();
}

// ------------------------------------------------
// Main
// ------------------------------------------------
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(900, 600);
    glutCreateWindow("Pacific Rim Style Robot (GLUT)");

    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    //glutMouseWheelFunc(mouseWheel);
    glutIdleFunc(updateAnimation);

    glClearColor(0.1, 0.1, 0.15, 1);

    glutMainLoop();
    return 0;
}
