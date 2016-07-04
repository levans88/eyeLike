#ifndef GLOBALS_H
#define GLOBALS_H

// Debugging
extern bool g_plotVectorField;

// Size constants
extern int g_eyePercentTop;
extern int g_eyePercentSide;
extern int g_eyePercentHeight;
extern int g_eyePercentWidth;

// Preprocessing
extern bool g_smoothFaceImage;
extern float g_smoothFaceFactor;

// Algorithm Parameters
extern int g_fastEyeWidth;
extern int g_weightBlurSize;
extern bool g_enableWeight;
extern float g_weightDivisor;
extern double g_gradientThreshold;

// Postprocessing
extern bool g_enablePostProcess;
extern float g_postProcessThreshold;

// Eye Corner
extern bool g_enableEyeCorner;

#endif