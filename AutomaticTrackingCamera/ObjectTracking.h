#ifndef OBJECTTRACKING_H
#define OBJECTTRACKING_H

#include <opencv2/opencv.hpp>

using namespace cv;

class Object
{
    public:
        Object();
        int trackObject;
        double area;
        Point2f center;
        Mat backproj;
        void tracking(Mat frame, const Rect roi, const Mat *mask);
        void stopTracking();

    private:
        Rect trackWindow;
        RotatedRect trackBox;
        Mat hist;
        void calcWeightedHistogram(Mat frame, const Rect roi, const Mat *mask);
        float kernelFunc(Point2f& roi_center, Point2f xy, int hx, int hy);
        void hist0To255();
};

#endif // OBJECTTRACKING_H
