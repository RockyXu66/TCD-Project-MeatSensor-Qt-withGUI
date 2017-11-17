#ifndef OPENCVWORKER_H
#define OPENCVWORKER_H

#include <QMutex>
#include <QThread>
#include <QImage>
#include <QWaitCondition>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

#include "strips.h"

using namespace std;

using namespace cv;

class OpenCvWorker : public QThread
{
    Q_OBJECT

private:
    bool stop;
    QMutex mutex;
    QWaitCondition condition;
    Mat processedFrame;
    Mat frame;
    int frameRate;
    VideoCapture cap;
    Mat RGBframe;
    QImage img;
    string filename;


    Parameter para;

    // ------------------------------meat sensor parameters start-------------------------------------
    int colorspace = 0; // 0 for HSV --- 1 for L*a*b
    int method = 0; // 0 for Average --- 1 for Median
    int sensor = 0; // 0 for MT2_L-3 sensor --- 1 for old sensor

    float O2, estimated;
    string O2_str;
    // 1st thresholding method simply adds threshold; 2nd thresholding method adds specific value of boundary and also adds dilate function
    int threshMethod = 1;
    int DEBUG = 0;  // 1 = verbose mode
    float* p;       // parameters variable

    Mat roiImg;         // Region of interest mat
    Mat foregroundImg;

    Point2f p1;     // left top point for roi in original frame
    Point2f p2;     // right bottom point for roi in original frame

    Mat panelMat;    // Create panel for text
    // ------------------------------meat sensor parameters end-------------------------------------

public:
    explicit OpenCvWorker(QObject *parent = nullptr);
    ~OpenCvWorker();

    // Initial image when opening the app
    QImage initialImg;

    //Load a video from memory
    bool loadVideo(String filename);

    //Play the video
    void Play();

    //Stop the video
    void Stop();

    //check if the player has been stopped
    bool isStopped() const;

protected:

    // Run the video
    void run();

    void msleep(int ms);

    // Process each frame
    void processFrame();

signals:
    //Signal to output frame to be displayed
    void sendFrame(const QImage &frameProcessed);

    // If video is finished, restart the video
    void sendVideoFinished();

    // Send O2 value to ui to be display
    void sendO2Value(QString content);

    // Send isOxygenCalculated to ui to be display
    void sendPrompt(bool isOxygenCalculated);

private slots:
    void receiveLeftArea(int num);
    void receiveRightArea(int num);

};

#endif // OPENCVWORKER_H
