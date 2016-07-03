#ifndef GLOBALS_H
#define GLOBALS_H

// Debugging
extern bool kPlotVectorField;

// Size constants
extern int kEyePercentTop;
extern int kEyePercentSide;
extern int kEyePercentHeight;
extern int kEyePercentWidth;

// Preprocessing
extern bool kSmoothFaceImage;
extern float kSmoothFaceFactor;

// Algorithm Parameters
extern int kFastEyeWidth;
extern int kWeightBlurSize;
extern bool kEnableWeight;
extern float kWeightDivisor;
extern double kGradientThreshold;

// Postprocessing
extern bool kEnablePostProcess;
extern float kPostProcessThreshold;

// Eye Corner
extern bool kEnableEyeCorner;

#endif