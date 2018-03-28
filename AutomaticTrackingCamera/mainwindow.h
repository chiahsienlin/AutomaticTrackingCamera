#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QTimer>
#include <QMessageBox>
#include <QMouseEvent>
#include <QKeyEvent>
#include <string.h>
#include "PTZcontrol.h"
#include "ObjectTracking.h"
#include "MotionDetection.h"
#define deviceID 0
#define DETECT_AREA 10

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private slots:
    void objTracking();
    void displayFrame();
    void on_pushButton_right_pressed();
    void on_pushButton_right_released();
    void on_pushButton_left_pressed();
    void on_pushButton_left_released();
    void on_pushButton_down_pressed();
    void on_pushButton_down_released();
    void on_pushButton_up_pressed();
    void on_pushButton_up_released();
    void on_pushButton_tele_pressed();
    void on_pushButton_tele_released();
    void on_pushButton_wide_pressed();
    void on_pushButton_wide_released();
    void on_pushButton_stoptrack_clicked();
    void on_pushButton_takephoto_clicked();
    void on_pushButton_startrecord_clicked();
    void on_pushButton_stoprecord_clicked();
    void on_pushButton_savepos_clicked();
    void on_pushButton_gotopos_clicked();

private:
    bool selectObject;
    bool recording;
    bool backprojMode;
    VideoCapture cap;
    VideoWriter writer;
    Mat frame;
    Point origin;
    Rect selection;
    PTZcontrol ptzCtrl;
    Object obj;
    MotionDetection motionDetection;
    QTimer *tracking_timer, *display_timer;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
