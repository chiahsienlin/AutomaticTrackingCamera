#include "MotionDetection.h"

MotionDetection::MotionDetection()
{
    detecting = false;
    bgSubtractor.set("nmixtures", 3);
}

void MotionDetection::setFgMask(Mat frame)
{
    bgSubtractor.operator()(frame, fgMask);
    erode(fgMask, fgMask, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
    dilate(fgMask, fgMask, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
    oMoments = moments(fgMask);
    area = oMoments.m00;
}

void MotionDetection::detectObject()  //find the biggest one
{
    vector< vector<Point> > contours;
    findContours(fgMask, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

    vector< vector<Point> > contours_poly(contours.size());
    vector<Rect> boundRect(contours.size());

    for(int i = 0, largest_area = 0; i < contours.size(); ++i)
    {
        boundRect[i] = boundingRect(Mat(contours[i]));
        if(largest_area < boundRect[i].area())
        {
            largest_area = boundRect[i].area();
            selection = boundRect[i];
        }
    }
}
