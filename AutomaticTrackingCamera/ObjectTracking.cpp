#include "ObjectTracking.h"

const int channels[3] = {0, 1, 2};
const float hranges[2] = {0, 255};
const float* phranges[] = {hranges, hranges, hranges};
const int h_bins = 32, s_bins = 16, v_bins = 4;
const int histSize[] = {h_bins, s_bins, v_bins};

Object::Object()
{
    trackObject = 0;
    area = 0.0;
}

void Object::calcWeightedHistogram(Mat frame, const Rect roi, const Mat *mask)
{
    Mat image_roi = frame(roi);

    int img_width = image_roi.cols;
    int img_height = image_roi.rows;
    Point2f roi_center(0.5*img_width, 0.5*img_height);
    int hx = img_width*0.5 , hy = img_height*0.5;

    hist.create(3, histSize, CV_32F); //3維空間
    hist.flags = (hist.flags & ~CV_MAT_TYPE_MASK)|CV_32F;//直方圖矩陣的類型信息
    hist = Scalar(0); //清空資料

    Vec3b* pix; //pixel指標
    uchar *maskPtr; //mask指標

    double normal_c = 0.0; //統計加權總量以做Normalize

    if (mask != NULL){
        Mat mask_roi = (*mask)(roi);
        for (int y = 0; y < img_height; ++y) {
            pix  = image_roi.ptr<Vec3b>(y);
            maskPtr = mask_roi.ptr<uchar>(y);
            for (int x = 0; x < img_width; ++x) {
                if (maskPtr[x] > 0){
                    int h_i = cvFloor(pix[x][0]*histSize[0]/256.0),
                        s_i = cvFloor(pix[x][1]*histSize[1]/256.0),
                        v_i = cvFloor(pix[x][2]*histSize[2]/256.0);
                    float weighted_value = kernelFunc(roi_center, Point2f(x,y), hx, hy); //加權函數
                    hist.at<float>(h_i, s_i, v_i) += weighted_value;
                    normal_c += weighted_value;
                }
            }
        }
    }
    else{
        for (int y = 0; y < img_height; ++y) {
            pix  = image_roi.ptr<Vec3b>(y);
            for (int x = 0; x < img_width; ++x) {
                int h_i = cvFloor(pix[x][0]*histSize[0]/256.0),
                    s_i = cvFloor(pix[x][1]*histSize[1]/256.0),
                    v_i = cvFloor(pix[x][2]*histSize[2]/256.0);
                float weighted_value = kernelFunc(roi_center, Point2f(x,y), hx, hy); //加權函數
                hist.at<float>(h_i, s_i, v_i) += weighted_value;
                normal_c += weighted_value;
            }
        }
    }
    //cerr << normal_c << endl;
    if (normal_c != 0){
        for (int i = 0; i < h_bins; ++i)
            for (int j = 0; j < s_bins; ++j)
                for (int k = 0; k < v_bins; ++k){
                    hist.at<float>(i, j, k) /= normal_c;
                }
    }
}

float Object::kernelFunc(Point2f& roi_center, Point2f xy, int hx, int hy)
{
    float distx = (xy.x - roi_center.x)/hx;
    float disty = (xy.y - roi_center.y)/hy;
    float dist_square = distx*distx + disty*disty;

    if (dist_square >= 1)
        return 0;

    return (1-dist_square)/0.01;
}

void Object::hist0To255()
{
    float max_value = 0.0;
    for (int i = 0; i < h_bins; ++i)
        for (int j = 0; j < s_bins; ++j)
            for (int k = 0; k < v_bins; ++k){
                if (hist.at<float>(i, j, k) > max_value)
                    max_value = hist.at<float>(i, j, k);
            }
    //轉換至0-255
    max_value /= 256;
    for (int i = 0; i < h_bins; ++i)
        for (int j = 0; j < s_bins; ++j)
            for (int k = 0; k < v_bins; ++k)
                hist.at<float>(i, j, k) = (hist.at<float>(i, j, k)/max_value);
}

void Object::tracking(Mat hsv, const Rect selection, const Mat *mask)
{
    if(trackObject < 0)
    {
        calcWeightedHistogram(hsv, selection, NULL);
        hist0To255();
        trackWindow = selection;
        trackObject = 1;
    }
    calcBackProject(&hsv, 1, channels, hist, backproj, phranges);
    threshold(backproj, backproj, 64, 0, THRESH_TOZERO);

    trackBox = CamShift(backproj, trackWindow,
                TermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ));

    center = trackBox.center;
    area = trackBox.size.area();
}

void Object::stopTracking()
{
    trackObject = 0;
    area = 0.0;
}
