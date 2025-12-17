#pragma once 

#ifndef M_PI
#define M_PI 3.1415926f
#endif

//2D
void DrawTeardrop(float cx, float cy, float r, float aspect, int num_segments);
void DrawEllipse(float cx, float cy, float rx, float ry, int num_segments);
void drawStar(float outerRadius, float innerRadius, int numPoints);
void DrawRectangle(float left, float bottom, float right, float top);
void DrawCircle(float cx, float cy, float r, int num_segments);
void DrawHalfCircle(float cx, float cy, float r, int num_segments);

//3D
void DrawBox(float width, float height, float depth, float r, float g, float b);
void DrawCylinder(float baseRadius, float topRadius, float height, float r, float g, float b);
void DrawPyramid(float baseSize, float height, float r, float g, float b);
void DrawPrism(float width, float height, float depth, float r, float g, float b);

void DrawSphere(float radius, float r, float g, float b);

void DrawTriPrism(float width, float height, float depth, float r, float g, float b); // 专用于剑刃