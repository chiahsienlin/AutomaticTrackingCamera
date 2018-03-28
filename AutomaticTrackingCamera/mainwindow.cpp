#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    selectObject = false;
    recording = false;
    backprojMode = false;

    //if not success, exit program
    if ( !cap.open(deviceID) ) QMessageBox::warning(NULL, "error", "could not open the camera");
    if( RS232_OpenComport(cport_nr, bdrate) ) QMessageBox::warning(NULL, "error", "could not open the comport");

    //create timer for display and tracking
    tracking_timer = new QTimer(this);
    display_timer = new QTimer(this);
    connect(tracking_timer, SIGNAL(timeout()), this, SLOT(objTracking()));
    connect(display_timer, SIGNAL(timeout()), this, SLOT(displayFrame()));
    display_timer->start(30);
    tracking_timer->start(10);
}

MainWindow::~MainWindow()
{
    obj.stopTracking();
    ptzCtrl.stopControl();
    RS232_CloseComport(cport_nr);
    display_timer->stop();
    tracking_timer->stop();
    delete ui;
}

/*timeEvent*/
void MainWindow::displayFrame()  //get the frame of camera and display
{
    Mat image, dest;

    if( !cap.isOpened() ) return;
    //read the next frame from the camera
    cap >> frame;
    flip(frame, frame, 0);  //vertical flipping of the image
    frame.copyTo(image);

    //record
    if( recording && writer.isOpened() ) writer << image;

    //if selectObject, highlight the selected region
    if( selectObject && selection.width > 0 && selection.height > 0 )
    {
        Mat roi(image, selection);
        bitwise_not(roi, roi);
    }
    //if backprojMode and tracking, display the back project image
    if( backprojMode && obj.trackObject ) cvtColor(obj.backproj, image, COLOR_GRAY2BGR);
    //ellipse( image, obj.trackBox, Scalar(0,0,255), 3, CV_AA );

    //show the frame on the label of Qt
    cvtColor(image, dest, CV_BGR2RGB);
    QImage qimage = QImage((uchar*)dest.data, dest.cols, dest.rows, dest.step, QImage::Format_RGB888);
    ui->frameLabel->setPixmap(QPixmap::fromImage(qimage));
    ui->frameLabel->show();
}

void MainWindow::objTracking()  //track the object and execute the queue
{
    Mat hsv;

    if( motionDetection.detecting )
    {
        motionDetection.setFgMask(frame);
        motionDetection.detectObject();
        if( motionDetection.selection.area() > CENTER_RANGE*CENTER_RANGE*4 )
        {
            selection = motionDetection.selection & Rect(0, 0, frame.cols, frame.rows);
            obj.trackObject = -1;
            motionDetection.detecting = false;
        }
    }

    if( obj.trackObject )
    {
        ui->pushButton_stoptrack->setEnabled(true);

        cvtColor(frame, hsv, COLOR_BGR2HSV);
        obj.tracking(hsv, selection, NULL);

        if( obj.area < DETECT_AREA )
        {
            obj.stopTracking();
            ptzCtrl.stopControl();
        }
        else ptzCtrl.process((int)obj.center.x, (int)obj.center.y, obj.area);
    }
    else ui->pushButton_stoptrack->setEnabled(false);

    ptzCtrl.exec();
}

/*mouseEvent (select object)*/
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if( ui->frameLabel->underMouse() )
    {
        int x=ui->frameLabel->mapFromParent(event->pos()).x();
        int y=ui->frameLabel->mapFromParent(event->pos()).y();

        origin = Point(x, y);
        selection = Rect(x, y, 0, 0);
        selectObject = true;
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if( ui->frameLabel->underMouse() )
    {
        selectObject = false;
        if( selection.width > 0 && selection.height > 0 ) obj.trackObject = -1;
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if( selectObject && ui->frameLabel->underMouse() )  //if selectObject, caculate the selected region
    {
        int x=ui->frameLabel->mapFromParent(event->pos()).x();
        int y=ui->frameLabel->mapFromParent(event->pos()).y();

        selection.x = MIN(x, origin.x);
        selection.y = MIN(y, origin.y);
        selection.width = std::abs(x - origin.x);
        selection.height = std::abs(y - origin.y);

        selection &= Rect(0, 0, frame.cols, frame.rows);
    }
}

/*pushButton*/
//manual control
void MainWindow::on_pushButton_right_pressed()  { motionDetection.detecting = false; obj.stopTracking(); ptzCtrl.manualControl(1, ui->speedSlider->value()); }
void MainWindow::on_pushButton_right_released() { motionDetection.detecting = false; obj.stopTracking(); ptzCtrl.manualControl(3, ui->speedSlider->value()); }
void MainWindow::on_pushButton_left_pressed()   { motionDetection.detecting = false; motionDetection.detecting = false; obj.stopTracking(); ptzCtrl.manualControl(2, ui->speedSlider->value()); }
void MainWindow::on_pushButton_left_released()  { motionDetection.detecting = false; obj.stopTracking(); ptzCtrl.manualControl(3, ui->speedSlider->value()); }
void MainWindow::on_pushButton_down_pressed()   { motionDetection.detecting = false; obj.stopTracking(); ptzCtrl.manualControl(4, ui->speedSlider->value()); }
void MainWindow::on_pushButton_down_released()  { motionDetection.detecting = false; obj.stopTracking(); ptzCtrl.manualControl(6, ui->speedSlider->value()); }
void MainWindow::on_pushButton_up_pressed()     { motionDetection.detecting = false; obj.stopTracking(); ptzCtrl.manualControl(5, ui->speedSlider->value()); }
void MainWindow::on_pushButton_up_released()    { motionDetection.detecting = false; obj.stopTracking(); ptzCtrl.manualControl(6, ui->speedSlider->value()); }
void MainWindow::on_pushButton_tele_pressed()   { motionDetection.detecting = false; obj.stopTracking(); ptzCtrl.manualControl(7, ui->speedSlider->value()); }
void MainWindow::on_pushButton_tele_released()  { motionDetection.detecting = false; obj.stopTracking(); ptzCtrl.manualControl(9, ui->speedSlider->value()); }
void MainWindow::on_pushButton_wide_pressed()   { motionDetection.detecting = false; obj.stopTracking(); ptzCtrl.manualControl(8, ui->speedSlider->value()); }
void MainWindow::on_pushButton_wide_released()  { motionDetection.detecting = false; obj.stopTracking(); ptzCtrl.manualControl(9, ui->speedSlider->value()); }

//save and goto position
void MainWindow::on_pushButton_savepos_clicked() { ptzCtrl.manualControl(10, 0); }
void MainWindow::on_pushButton_gotopos_clicked() { motionDetection.detecting = false; obj.stopTracking(); ptzCtrl.manualControl(11, 0); }

//stop tracking
void MainWindow::on_pushButton_stoptrack_clicked() { motionDetection.detecting = false; obj.stopTracking(); ptzCtrl.stopControl(); }

//take photos
void MainWindow::on_pushButton_takephoto_clicked()
{
    if(!cap.isOpened()) return;

    Mat dest;
    cvtColor(frame, dest, CV_BGR2RGB);
    QImage qimage = QImage((uchar*)dest.data, dest.cols, dest.rows, dest.step, QImage::Format_RGB888);

    time_t rawtime = time(NULL);
    struct tm * timeinfo = localtime(&rawtime);
    char curtime[30];
    strftime(curtime, 30, "photo_%b%d-%H%M%S.jpg", timeinfo);
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Photo"), tr(curtime),
                                                    tr("Images (*.jpg *.png *.xpm)"));

    if( !filename.isEmpty() ) qimage.save(filename);
}

//record
void MainWindow::on_pushButton_startrecord_clicked()
{
    if( !recording )
    {
        time_t rawtime = time(NULL);
        struct tm * timeinfo = localtime(&rawtime);
        char curtime[30] = {0};
        strftime(curtime, 30, "video_%b%d-%H%M%S.avi", timeinfo);

        writer.open(curtime, CV_FOURCC('D','I','V','X'), 30, frame.size());

        recording = true;
        ui->pushButton_startrecord->setEnabled(false);
    }
}

void MainWindow::on_pushButton_stoprecord_clicked()
{
    if( recording )
    {
        writer.release();

        recording = false;
        ui->pushButton_startrecord->setEnabled(true);
    }
}

/*keyEvent*/
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch( event->key() )
    {
        case Qt::Key_D: on_pushButton_right_pressed(); break;
        case Qt::Key_A: on_pushButton_left_pressed(); break;
        case Qt::Key_S: on_pushButton_down_pressed(); break;
        case Qt::Key_W: on_pushButton_up_pressed(); break;
        case Qt::Key_Z: on_pushButton_tele_pressed(); break;
        case Qt::Key_X: on_pushButton_wide_pressed(); break;
        case Qt::Key_T: on_pushButton_savepos_clicked(); break;
        case Qt::Key_Y: on_pushButton_gotopos_clicked(); break;
        case Qt::Key_G: on_pushButton_startrecord_clicked(); break;
        case Qt::Key_H: on_pushButton_stoprecord_clicked(); break;
        case Qt::Key_J: on_pushButton_takephoto_clicked(); break;
        case Qt::Key_P: on_pushButton_stoptrack_clicked(); break;
        case Qt::Key_B: backprojMode = !backprojMode; break;
        case Qt::Key_M: obj.stopTracking(); ptzCtrl.stopControl();
                        motionDetection.detecting = true; break;
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    switch( event->key() )
    {
        case Qt::Key_D: on_pushButton_right_released(); break;
        case Qt::Key_A: on_pushButton_left_released(); break;
        case Qt::Key_S: on_pushButton_down_released(); break;
        case Qt::Key_W: on_pushButton_up_released(); break;
        case Qt::Key_Z: on_pushButton_tele_released(); break;
        case Qt::Key_X: on_pushButton_wide_released(); break;
    }
}
