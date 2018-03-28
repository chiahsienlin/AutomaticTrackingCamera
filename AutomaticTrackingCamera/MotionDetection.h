#include <opencv2/opencv.hpp>

using namespace cv;

class MotionDetection {
    public:
        bool detecting;
        MotionDetection();
        void setFgMask(Mat frame);
        void detectObject();
        double area;
        Rect selection;

    private:
        BackgroundSubtractorMOG2 bgSubtractor;
        Mat fgMask;
        Moments oMoments;
};

