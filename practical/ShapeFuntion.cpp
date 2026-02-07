#include "ShapeFunction.h" 
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <cmath>

void DrawTeardrop(float cx, float cy, float r, float aspect, int num_segments) {
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(cx, cy - r);
	for (int i = 0; i <= num_segments / 2; i++) {
		float theta = (float)i / (float)(num_segments / 2) * M_PI;

		float angle_start = -M_PI;
		float theta_prime = angle_start + theta;

		float x = cx + r * cos(theta_prime);
		float y = cy + r * sin(theta_prime);

		glVertex2f(x, y);
	}
	glVertex2f(cx, cy + r * aspect);
	glVertex2f(cx + r * cos(-M_PI), cy + r * sin(-M_PI));
	glEnd();
}

void DrawEllipse(float cx, float cy, float rx, float ry, int num_segments) {
	glBegin(GL_TRIANGLE_FAN);

	glVertex2f(cx, cy);

	for (int i = 0; i <= num_segments; i++) {
		float theta = (float)i / (float)num_segments * 2.0f * M_PI;

		float x = cx + rx * cos(theta);
		float y = cy + ry * sin(theta);

		glVertex2f(x, y);
	}
	glEnd();
}

void drawStar(float outerRadius, float innerRadius, int numPoints) {
	if (numPoints < 3) numPoints = 3;
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(0.0f, 0.0f);
	for (int i = 0; i <= numPoints * 2; ++i) {
		float angle = (float)i / (float)(numPoints * 2) * (2.0f * M_PI) + (M_PI / 2.0f);
		float radius = (i % 2 == 0) ? outerRadius : innerRadius;
		float x = radius * cos(angle);
		float y = radius * sin(angle);
		glVertex2f(x, y);
	}
	glEnd();
}

void DrawRectangle(float left, float bottom, float right, float top) {
	glBegin(GL_QUADS);
	glVertex2f(left, top);
	glVertex2f(right, top);
	glVertex2f(right, bottom);
	glVertex2f(left, bottom);
	glEnd();
}

void DrawCircle(float cx, float cy, float r, int num_segments) {
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(cx, cy);

	for (int i = 0; i <= num_segments; i++) {  //int i = 0; i <= num_segments misising,1<=99, total 100, so missing 1
		float theta = (float)i / (float)num_segments * 2.0f * M_PI;

		float x = cx + r * cos(theta);
		float y = cy + r * sin(theta);

		glVertex2f(x, y);
	}
	glEnd();
}

void DrawHalfCircle(float cx, float cy, float r, int num_segments) {
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(cx, cy);

	for (int i = 0; i <= num_segments; i++) {  //int i = 0; i <= num_segments misising,1<=99, total 100, so missing 1
		float theta = (float)i / (float)num_segments * M_PI;

		float x = cx + r * cos(theta);
		float y = cy + r * -sin(theta);

		glVertex2f(x, y);
	}
	glEnd();
}

void DrawBox(float width, float height, float depth, float r, float g, float b) {
	glColor3f(r, g, b);

	glPushMatrix();
	glScalef(width, height, depth);

	glBegin(GL_QUADS);
	// front
	glVertex3f(-0.5f, -0.5f, 0.5f); glVertex3f(0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, 0.5f, 0.5f);   glVertex3f(-0.5f, 0.5f, 0.5f);

	// back
	glVertex3f(-0.5f, -0.5f, -0.5f); glVertex3f(-0.5f, 0.5f, -0.5f);
	glVertex3f(0.5f, 0.5f, -0.5f);   glVertex3f(0.5f, -0.5f, -0.5f);

	// left
	glVertex3f(-0.5f, -0.5f, -0.5f); glVertex3f(-0.5f, -0.5f, 0.5f);
	glVertex3f(-0.5f, 0.5f, 0.5f);   glVertex3f(-0.5f, 0.5f, -0.5f);

	// right
	glVertex3f(0.5f, -0.5f, -0.5f); glVertex3f(0.5f, 0.5f, -0.5f);
	glVertex3f(0.5f, 0.5f, 0.5f);   glVertex3f(0.5f, -0.5f, 0.5f);

	// up
	glVertex3f(-0.5f, 0.5f, -0.5f); glVertex3f(-0.5f, 0.5f, 0.5f);
	glVertex3f(0.5f, 0.5f, 0.5f);   glVertex3f(0.5f, 0.5f, -0.5f);

	// down
	glVertex3f(-0.5f, -0.5f, -0.5f); glVertex3f(0.5f, -0.5f, -0.5f);
	glVertex3f(0.5f, -0.5f, 0.5f);   glVertex3f(-0.5f, -0.5f, 0.5f);
	glEnd();

	glPopMatrix();
}


void DrawCylinder(float baseRadius, float topRadius, float height, float r, float g, float b) {
	glColor3f(r, g, b);
	GLUquadricObj* quad = gluNewQuadric();

	glPushMatrix();
	glRotatef(-90, 1.0f, 0.0f, 0.0f); 
	gluCylinder(quad, baseRadius, topRadius, height, 32, 1);

	glPushMatrix();
	glScalef(1.0f, 1.0f, -1.0f); 
	gluDisk(quad, 0.0f, baseRadius, 32, 1);
	glPopMatrix();

	// 画顶盖 (使用 topRadius)
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, height); // 移到顶部
	gluDisk(quad, 0.0f, topRadius, 32, 1);
	glPopMatrix();

	glPopMatrix();

	gluDeleteQuadric(quad);
}

void DrawPyramid(float baseSize, float height, float r, float g, float b) {
	glColor3f(r, g, b);
	float halfBase = baseSize / 2.0f;

	glBegin(GL_TRIANGLES);
	// front
	glVertex3f(0.0f, height, 0.0f); //top
	glVertex3f(-halfBase, 0.0f, halfBase);
	glVertex3f(halfBase, 0.0f, halfBase);

	// right
	glVertex3f(0.0f, height, 0.0f);
	glVertex3f(halfBase, 0.0f, halfBase);
	glVertex3f(halfBase, 0.0f, -halfBase);

	// back
	glVertex3f(0.0f, height, 0.0f);
	glVertex3f(halfBase, 0.0f, -halfBase);
	glVertex3f(-halfBase, 0.0f, -halfBase);

	// left
	glVertex3f(0.0f, height, 0.0f);
	glVertex3f(-halfBase, 0.0f, -halfBase);
	glVertex3f(-halfBase, 0.0f, halfBase);
	glEnd();

	// bottom (quad)
	glBegin(GL_QUADS);
	glVertex3f(-halfBase, 0.0f, halfBase);
	glVertex3f(halfBase, 0.0f, halfBase);
	glVertex3f(halfBase, 0.0f, -halfBase);
	glVertex3f(-halfBase, 0.0f, -halfBase);
	glEnd();
}

void DrawPrism(float width, float height, float depth, float r, float g, float b) {
	glColor3f(r, g, b);

	float halfW = width / 2.0f;
	float halfD = depth / 2.0f;

	glBegin(GL_TRIANGLES);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, height, halfD);    
	glVertex3f(-halfW, 0.0f, halfD);    
	glVertex3f(halfW, 0.0f, halfD);     

	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(0.0f, height, -halfD); 
	glVertex3f(halfW, 0.0f, -halfD);   
	glVertex3f(-halfW, 0.0f, -halfD);  
	glEnd();

	glBegin(GL_QUADS);

	glNormal3f(-1.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, height, halfD); 
	glVertex3f(0.0f, height, -halfD);
	glVertex3f(-halfW, 0.0f, -halfD);  
	glVertex3f(-halfW, 0.0f, halfD);    

	glNormal3f(1.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, height, halfD);   
	glVertex3f(halfW, 0.0f, halfD);    
	glVertex3f(halfW, 0.0f, -halfD);   
	glVertex3f(0.0f, height, -halfD);  

	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-halfW, 0.0f, halfD);
	glVertex3f(-halfW, 0.0f, -halfD);
	glVertex3f(halfW, 0.0f, -halfD);
	glVertex3f(halfW, 0.0f, halfD);
	glEnd();
}

void DrawSphere(float radius, float r, float g, float b) {
	glColor3f(r, g, b);

	GLUquadricObj* quad = gluNewQuadric();

	gluQuadricDrawStyle(quad, GLU_FILL);    // Render as solid geometry
	gluQuadricNormals(quad, GLU_SMOOTH);    // Smooth shading for lighting
	gluQuadricTexture(quad, GL_TRUE);       // Generate texture coordinates (Required for textures)

	gluSphere(quad, radius, 32, 32);

	gluDeleteQuadric(quad);
}

void DrawTriPrism(float width, float height, float depth, float r, float g, float b) {
	glColor3f(r, g, b);
	glPushMatrix();
	glScalef(width, height, depth);

	glBegin(GL_TRIANGLES);
	// Top Face
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-0.5f, 0.5f, -0.5f);
	glVertex3f(0.5f, 0.5f, -0.5f);
	glVertex3f(0.0f, 0.5f, 0.5f);

	// Bottom Face
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(0.0f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, -0.5f);
	glEnd();

	glBegin(GL_QUADS);
	// Back Face (The flat side)
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, 0.5f, -0.5f);
	glVertex3f(0.5f, 0.5f, -0.5f);
	glVertex3f(0.5f, -0.5f, -0.5f);

	// Left Slope
	// Calculate normal roughly (pointing up-left-forward)
	glNormal3f(-1.0f, 0.0f, 1.0f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(0.0f, -0.5f, 0.5f);
	glVertex3f(0.0f, 0.5f, 0.5f);
	glVertex3f(-0.5f, 0.5f, -0.5f);

	// Right Slope
	// Calculate normal roughly (pointing up-right-forward)
	glNormal3f(1.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, -0.5f);
	glVertex3f(0.5f, 0.5f, -0.5f);
	glVertex3f(0.0f, 0.5f, 0.5f);
	glEnd();

	glPopMatrix();
}


