#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include "opencvworker.h"
#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <qpixmap>
#include <QTimer>

using namespace cv;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;

    OpenCvWorker *worker;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void sendSetup(int device);
    void sendToggleStream();
    void sendLeftAreaValue(int num);
    void sendRightAreaValue(int num);

private slots:
    //Display video frame in player UI
    void receiveProcessedFrame(QImage img);

    // Display O2 value in UI
    void receiveO2Value(QString content);

    // Display prompt if oxygen is not calculated
    void receivePrompt(bool isOxygenCalculated);

    // If video is finished, reset play button
//    void receiveVideoFinished();

    //Slot for the load video push button.
    void on_pushButtonLoad_clicked();

    // Slot for the play push button.
    void on_pushButtonPlay_clicked();

    void on_horizontalSliderLeftArea_valueChanged(int num);

    void on_horizontalSliderRightArea_valueChanged(int num);

};

#endif // MAINWINDOW_H
