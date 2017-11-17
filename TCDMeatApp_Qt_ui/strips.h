#ifndef STRIPS_H
#define STRIPS_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv/cv.h>

#include <opencv2/tracking.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/imgcodecs.hpp>

#include <iostream>
#include <string>

using namespace cv;
using namespace std;

#include <vector>
#include <math.h>
#include <time.h>



//static int roiX = 0, roiY = 0, roiWidth = 0, roiHeight = 0;


Mat visuColorspace(Mat img, int Colorspace, int ThresholdMethod, int DEBUG);
void gradientProcess(Mat img, int Colorspace, int DEBUG);
Mat getBoundingBox(Mat img, int ThresholdMethod, int Colorspace, int DEBUG);
float avgHue(Mat img, int curveColorSpace);
float findMedian(Mat img, int curveColorSpace);
float computeOxygen(float estimated, float parameters[]);
float* getParameters(int colorspace, int method, int sensor);

Mat BgSubtraction(Mat img);
Mat GetBoundingBoxByBgSub(Mat img, int Colorspace, int DEBUG, Point2f & p1, Point2f & p2, Mat & roiMat);
string Formate(float O2);
Mat GetBoundingBoxByBgSub2(Mat img, int Colorspace, int DEBUG, Point2f & p1, Point2f & p2);

// static variables
//static float leftLine = 0.4;
//static float rightLine = 0.67;
// Yinghan's scalar => half size of roi


class Parameter{
public:

    Parameter(){
        leftLine = 0.8f;
        rightLine = 0.3f;
    }
    void setLeftLine(float num){
        leftLine = num;
    }
    void setRightLine(float num){
        rightLine = num;
    }
    float getLeftLine(){
        return leftLine;
    }
    float getRightLine(){
        return rightLine;
    }

    Mat GetBoundingBoxByBgSub2(Mat img, int Colorspace, int DEBUG, Point2f & p1, Point2f & p2);

//    Mat GetBoundingBoxByBgSub2(Mat img, int Colorspace, int DEBUG, Point2f & p1, Point2f & p2);
private:
    float leftLine;
    float rightLine;
};

static int scalar = 30;
static Parameter para2;



/*
class Strip
{

public:
    Mat getBoundingBox(Mat img);
    float avgHue(Mat img, int curveColorSpace);
    float findMedian(Mat img, int curveColorSpace);
    float computeOxygen(float estimated, float parameters[]);
    float* getParameters(int colorspace, int method);
    Mat getResult() {return resultMat;}
    Mat getContours() {return contoursMat;}
    float getEstimation() {return estimatedValue;}
    void setResult(int i) {initGL=i;}
    void setInitGL(int i) {initGL=i;}
    void setInitGL(int i) {initGL=i;}

private:
    Mat resultMat, contoursMat;
    float estimatedValue;

private slots:

};
*/

#endif // STRIPS_H
