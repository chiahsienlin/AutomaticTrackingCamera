#ifndef PTZCONTROL_H
#define PTZCONTROL_H

#include "DSCP.h"
#include <time.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
#define CENTER_RANGE 100
#define ZOOM 5000
#define BUFSIZE 30
#define SPEED_LV 4

using namespace std;

const char str[][15]={ "Pan Right", "Pan Left", "Pan Stop", "Tilt Down", "Tilt Up", "Tilt Stop" };

class PTZcontrol
{
    public:
        PTZcontrol();
        void exec();
        void process(int x, int y, double area);
        void stopControl();
        void setCenter(int x, int y);
        void manualControl(int cmd, int speed);

    private:
        int centerX, centerY;
        int panflag, tiltflag, zoomflag, cur_spX, cur_spY;
        int cur_pan, cur_tilt, cur_zoom;
        DSCP ctrl;
        clock_t pre_t;
        double selected_area;
        int buffer[BUFSIZE], first, last;
        int speedJudge(int p, int center);
        void enqueue(int data);
        int dequeue();
        void showqueue();
};

#endif // PTZCONTROL_H
