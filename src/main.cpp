#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <queue>
#include <stdio.h>
#include <math.h>

#include "constants.h"
#include "findEyeCenter.h"
#include "findEyeCorner.h"


/** Constants **/


/** Function Headers */
void detectAndDisplay( cv::Mat frame );
//void callbackButton(int state, void* userdata);

/** Global variables */
//-- Note, either copy these two files from opencv/data/haarscascades to your current folder, or change these locations
cv::String face_cascade_name = "../res/haarcascade_frontalface_alt.xml";
cv::CascadeClassifier face_cascade;
std::string main_window_name = "Capture - Face detection";
std::string face_window_name = "Capture - Face";
cv::RNG rng(12345);
cv::Mat debugImage;
cv::Mat skinCrCbHist = cv::Mat::zeros(cv::Size(256, 256), CV_8UC1);

//-- Note, these used to be constants but are now global variables so they can be modified after runtime
// Debugging
bool g_plotVectorField = false;

// Size constants
int g_eyePercentTop = 25;
int g_eyePercentSide = 13;
int g_eyePercentHeight = 30;
int g_eyePercentWidth = 35;

// Preprocessing
bool g_smoothFaceImage = true;
int g_smoothFaceFactorInt = 5;
//float g_smoothFaceFactor = 0.005;

// Algorithm Parameters
int g_fastEyeWidth = 50; //Trade accuracy for speed, smaller = faster, eye frame is rescaled to this width.
int g_weightBlurSize = 5; //No observable difference
bool g_enableWeight = true;
int g_weightDivisorInt = 10; //Unsure of meaningful values or scale
//float g_weightDivisor = 1.0;
int g_gradientThresholdInt = 500;
//double g_gradientThreshold = 50.0;

// Postprocessing
bool g_enablePostProcess = true;
int g_postProcessThresholdInt = 97;
//float g_postProcessThreshold = 0.97;

// Eye Corner
bool g_enableEyeCorner = false;

/**
 * @function main
 */
int main( int argc, const char** argv ) {
  cv::Mat frame;

  // Load the cascades
  if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading face cascade, please change face_cascade_name in source code. Press enter to continue...\n"); getchar(); return -1; };

  cv::namedWindow(main_window_name, CV_WINDOW_NORMAL);
  cv::moveWindow(main_window_name, 400, 100);
  cv::namedWindow(face_window_name, CV_WINDOW_NORMAL);
  cv::moveWindow(face_window_name, 10, 100);
  cv::namedWindow("Right Eye", CV_WINDOW_NORMAL);
  cv::moveWindow("Right Eye", 10, 600);
  cv::namedWindow("Left Eye", CV_WINDOW_NORMAL);
  cv::moveWindow("Left Eye", 10, 800);
  //cv::namedWindow("aa",CV_WINDOW_NORMAL);
  //cv::moveWindow("aa", 10, 800);
  //cv::namedWindow("aaa",CV_WINDOW_NORMAL);
  //cv::moveWindow("aaa", 10, 800);

  cv::namedWindow("Menu", CV_WINDOW_AUTOSIZE | CV_GUI_EXPANDED);
  cv::moveWindow("Menu", 0, 0);

  //g_plotVectorField
  //cv::createButton("button1", callbackButton, &g_plotVectorField, CV_CHECKBOX, 0);
  //get state from var
  cv::createTrackbar("Eye % Top", "Menu", &g_eyePercentTop, 70);
  cv::createTrackbar("Eye % Side", "Menu", &g_eyePercentSide, 65);
  cv::createTrackbar("Eye % H", "Menu", &g_eyePercentHeight, 75);
  cv::createTrackbar("Eye % W", "Menu", &g_eyePercentWidth, 87);
  cv::createTrackbar("Smooth", "Menu", &g_smoothFaceFactorInt, 20);
  cv::createTrackbar("Fast Eye W", "Menu", &g_fastEyeWidth, 70);
  cv::createTrackbar("Wt. Blur", "Menu", &g_weightBlurSize, 30);
  cv::createTrackbar("Wt. Div.", "Menu", &g_weightDivisorInt, 100);
  cv::createTrackbar("Grd. TH", "Menu", &g_gradientThresholdInt, 1000);
  cv::createTrackbar("PostProc TH", "Menu", &g_postProcessThresholdInt, 100);

  //g_smoothFaceImage = false;
  //g_enableWeight = true;
  //g_enablePostProcess = true;
  //g_enableEyeCorner = false;

  createCornerKernels();
  ellipse(skinCrCbHist, cv::Point(113, 155.6), cv::Size(23.4, 15.2),
          43.0, 0.0, 360.0, cv::Scalar(255, 255, 255), -1);

  // I make an attempt at supporting both 2.x and 3.x OpenCV
#if CV_MAJOR_VERSION < 3
  CvCapture* capture = cvCaptureFromCAM( -1 );
  if( capture ) {
    while( true ) {
      frame = cvQueryFrame( capture );
#else
  cv::VideoCapture capture(-1);
  if( capture.isOpened() ) {
    while( true ) {
      capture.read(frame);
#endif
      // mirror it
      cv::flip(frame, frame, 1);
      frame.copyTo(debugImage);

      // Apply the classifier to the frame
      if( !frame.empty() ) {
        detectAndDisplay( frame );
      }
      else {
        printf(" --(!) No captured frame -- Break!");
        break;
      }

      imshow(main_window_name,debugImage);

      int c = cv::waitKey(10);
      if( (char)c == 'c' ) { break; }
      if( (char)c == 'f' ) {
        imwrite("frame.png",frame);
      }

    }
  }

  releaseCornerKernels();

  return 0;
}

//void callbackButton(int state, void* userdata) {
//	//(state == 0) ? (state = 1) : (state = 0);
//	//(state == 0) ? (userdata = false) : (userdata = true);
//	//printf(&userdata);
//	printf("test");
//}

void findEyes(cv::Mat frame_gray, cv::Rect face) {
  cv::Mat faceROI = frame_gray(face);
  cv::Mat debugFace = faceROI;

  if (g_smoothFaceImage) {
	//Disallow zero as value
	if (g_smoothFaceFactorInt == 0) {
		g_smoothFaceFactorInt = 1;
	}
	float smoothFaceFactor = (float)g_smoothFaceFactorInt / 1000;
	//float smoothFaceFactor = g_smoothFaceFactorInt / 1000;
    double sigma = smoothFaceFactor * face.width;
    GaussianBlur( faceROI, faceROI, cv::Size( 0, 0 ), sigma);
  }
  //-- Find eye regions and draw them
  int eye_region_width = face.width * (g_eyePercentWidth/100.0);
  int eye_region_height = face.width * (g_eyePercentHeight/100.0);
  int eye_region_top = face.height * (g_eyePercentTop/100.0);
  cv::Rect leftEyeRegion(face.width*(g_eyePercentSide/100.0),
                         eye_region_top,eye_region_width,eye_region_height);
  cv::Rect rightEyeRegion(face.width - eye_region_width - face.width*(g_eyePercentSide/100.0),
                          eye_region_top,eye_region_width,eye_region_height);

  //-- Find Eye Centers
  cv::Point leftPupil = findEyeCenter(faceROI,leftEyeRegion,"Left Eye");
  cv::Point rightPupil = findEyeCenter(faceROI,rightEyeRegion,"Right Eye");
  // get corner regions
  cv::Rect leftRightCornerRegion(leftEyeRegion);
  leftRightCornerRegion.width -= leftPupil.x;
  leftRightCornerRegion.x += leftPupil.x;
  leftRightCornerRegion.height /= 2;
  leftRightCornerRegion.y += leftRightCornerRegion.height / 2;
  cv::Rect leftLeftCornerRegion(leftEyeRegion);
  leftLeftCornerRegion.width = leftPupil.x;
  leftLeftCornerRegion.height /= 2;
  leftLeftCornerRegion.y += leftLeftCornerRegion.height / 2;
  cv::Rect rightLeftCornerRegion(rightEyeRegion);
  rightLeftCornerRegion.width = rightPupil.x;
  rightLeftCornerRegion.height /= 2;
  rightLeftCornerRegion.y += rightLeftCornerRegion.height / 2;
  cv::Rect rightRightCornerRegion(rightEyeRegion);
  rightRightCornerRegion.width -= rightPupil.x;
  rightRightCornerRegion.x += rightPupil.x;
  rightRightCornerRegion.height /= 2;
  rightRightCornerRegion.y += rightRightCornerRegion.height / 2;
  rectangle(debugFace,leftRightCornerRegion,200);
  rectangle(debugFace,leftLeftCornerRegion,200);
  rectangle(debugFace,rightLeftCornerRegion,200);
  rectangle(debugFace,rightRightCornerRegion,200);
  // change eye centers to face coordinates
  rightPupil.x += rightEyeRegion.x;
  rightPupil.y += rightEyeRegion.y;
  leftPupil.x += leftEyeRegion.x;
  leftPupil.y += leftEyeRegion.y;
  // draw eye centers
  circle(debugFace, rightPupil, 3, 1234);
  circle(debugFace, leftPupil, 3, 1234);

  //-- Find Eye Corners
  if (g_enableEyeCorner) {
    cv::Point2f leftRightCorner = findEyeCorner(faceROI(leftRightCornerRegion), true, false);
    leftRightCorner.x += leftRightCornerRegion.x;
    leftRightCorner.y += leftRightCornerRegion.y;
    cv::Point2f leftLeftCorner = findEyeCorner(faceROI(leftLeftCornerRegion), true, true);
    leftLeftCorner.x += leftLeftCornerRegion.x;
    leftLeftCorner.y += leftLeftCornerRegion.y;
    cv::Point2f rightLeftCorner = findEyeCorner(faceROI(rightLeftCornerRegion), false, true);
    rightLeftCorner.x += rightLeftCornerRegion.x;
    rightLeftCorner.y += rightLeftCornerRegion.y;
    cv::Point2f rightRightCorner = findEyeCorner(faceROI(rightRightCornerRegion), false, false);
    rightRightCorner.x += rightRightCornerRegion.x;
    rightRightCorner.y += rightRightCornerRegion.y;
    circle(faceROI, leftRightCorner, 3, 200);
    circle(faceROI, leftLeftCorner, 3, 200);
    circle(faceROI, rightLeftCorner, 3, 200);
    circle(faceROI, rightRightCorner, 3, 200);
  }

  imshow(face_window_name, faceROI);
//  cv::Rect roi( cv::Point( 0, 0 ), faceROI.size());
//  cv::Mat destinationROI = debugImage( roi );
//  faceROI.copyTo( destinationROI );
}


cv::Mat findSkin (cv::Mat &frame) {
  cv::Mat input;
  cv::Mat output = cv::Mat(frame.rows,frame.cols, CV_8U);

  cvtColor(frame, input, CV_BGR2YCrCb);

  for (int y = 0; y < input.rows; ++y) {
    const cv::Vec3b *Mr = input.ptr<cv::Vec3b>(y);
//    uchar *Or = output.ptr<uchar>(y);
    cv::Vec3b *Or = frame.ptr<cv::Vec3b>(y);
    for (int x = 0; x < input.cols; ++x) {
      cv::Vec3b ycrcb = Mr[x];
//      Or[x] = (skinCrCbHist.at<uchar>(ycrcb[1], ycrcb[2]) > 0) ? 255 : 0;
      if(skinCrCbHist.at<uchar>(ycrcb[1], ycrcb[2]) == 0) {
        Or[x] = cv::Vec3b(0,0,0);
      }
    }
  }
  return output;
}

/**
 * @function detectAndDisplay
 */
void detectAndDisplay( cv::Mat frame ) {
  std::vector<cv::Rect> faces;
  //cv::Mat frame_gray;

  std::vector<cv::Mat> rgbChannels(3);
  cv::split(frame, rgbChannels);
  cv::Mat frame_gray = rgbChannels[2];

  //cvtColor( frame, frame_gray, CV_BGR2GRAY );
  //equalizeHist( frame_gray, frame_gray );
  //cv::pow(frame_gray, CV_64F, frame_gray);
  //-- Detect faces
  face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(150, 150) );
//  findSkin(debugImage);

  for( int i = 0; i < faces.size(); i++ )
  {
    rectangle(debugImage, faces[i], 1234);
  }
  //-- Show what you got
  if (faces.size() > 0) {
    findEyes(frame_gray, faces[0]);
  }
}
