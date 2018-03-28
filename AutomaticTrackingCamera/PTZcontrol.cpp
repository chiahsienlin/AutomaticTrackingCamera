#include "PTZcontrol.h"

PTZcontrol::PTZcontrol()
{
    centerX = 320; centerY = 240;
    panflag = 0; tiltflag = 0; zoomflag = 0; cur_spX = 0; cur_spY = 0;
    cur_pan = 0; cur_tilt = 0; cur_zoom = 0;
    pre_t = 0; selected_area = 0;
    memset(buffer, 0, sizeof(buffer)); first = 0; last = 0;
}

void PTZcontrol::exec()
{
    if(first != last && ((clock()-pre_t)*1000/CLOCKS_PER_SEC) >= 100)
    {
        pre_t = clock();
        //showqueue();
        int cmd = dequeue();

        switch(cmd/10)
        {
            case 1: ctrl.Pan(RIGHT, cmd%10); cur_pan = 1; break;
            case 2: ctrl.Pan(LEFT, cmd%10); cur_pan = 2; break;
            case 3: ctrl.Pan(P_STOP, cmd%10); cur_pan = 0; break;
            case 4: ctrl.Tilt(UP, cmd%10); cur_tilt = 1; break;
            case 5: ctrl.Tilt(DOWN, cmd%10); cur_tilt = 2; break;
            case 6: ctrl.Tilt(T_STOP, cmd%10); cur_tilt = 0; break;
            case 7: ctrl.Zoom(TELE); cur_zoom = 1; break;
            case 8: ctrl.Zoom(WIDE); cur_zoom = 2; break;
            case 9: ctrl.Zoom(Z_STOP); cur_zoom = 0; break;
            case 10: ctrl.PresetSet(SET, 0); break;
            case 11: ctrl.GotoHomePos(); break;
        }
    }
}

void PTZcontrol::process(int x, int y, double area)
{
    int speedX=speedJudge(x, centerX), speedY=speedJudge(y, centerY);
    if(selected_area == 0) selected_area = area;

    if(x-centerX > CENTER_RANGE && (panflag != 1 || cur_spX != speedX))  //pan right
        { enqueue(10+speedX); cur_spX = speedX; panflag = 1; }
    else if(x-centerX < -CENTER_RANGE && (panflag != 2 || cur_spX != speedX))  //pan left
        { enqueue(20+speedX); cur_spX=speedX; panflag = 2; }
    else if(x-centerX <= CENTER_RANGE && x-centerX >= -CENTER_RANGE && panflag != 0)  //pan stop
        { enqueue(30); panflag = 0; }

    if(y-centerY > CENTER_RANGE && (tiltflag != 1 || cur_spY != speedY))  //tilt up
        { enqueue(40+speedY); cur_spY=speedY; tiltflag = 1; }
    else if(y-centerY < -CENTER_RANGE && (tiltflag != 2 || cur_spY != speedY))  //tilt down
        { enqueue(50+speedY); cur_spY=speedY; tiltflag = 2; }
    else if(y-centerY <= CENTER_RANGE && y-centerY >= -CENTER_RANGE && tiltflag != 0)  //tilt stop
        { enqueue(60); tiltflag = 0; }

/*
    if(area < selected_area-ZOOM && zoomflag != 1)  //zoom in
        { enqueue(70); zoomflag = 1; }
    else if(area > selected_area+ZOOM && zoomflag != 2)  //zoom out
        { enqueue(80); zoomflag = 2; }
    else if(area >= selected_area-ZOOM && area <= selected_area+ZOOM && zoomflag != 0)  //zoom stop
        { enqueue(90); zoomflag = 0; }
*/
}

int PTZcontrol::speedJudge(int p, int center)
{
    return (std::abs(p-center)-CENTER_RANGE)*SPEED_LV/(center-CENTER_RANGE);
}

void PTZcontrol::stopControl()
{
    memset(buffer, 0, sizeof(buffer)); first = 0; last = 0;
    if(cur_pan != 0) enqueue(30);
    if(cur_tilt != 0) enqueue(60);
    if(cur_zoom != 0) enqueue(90);
    panflag = 0; tiltflag = 0; zoomflag = 0; cur_spX = 0; cur_spY = 0;
    cur_pan = 0; cur_tilt = 0; cur_zoom = 0;
    selected_area = 0;
}

void PTZcontrol::setCenter(int x, int y)
{
    centerX = x; centerY = y;
}

void PTZcontrol::manualControl(int cmd, int speed)
{
    if( cmd != 3 &&  cmd != 6 && cmd != 9 && cmd != 10 ) stopControl();
    enqueue(cmd*10+speed);
}

void PTZcontrol::enqueue(int data)
{
    int r = (last+1)%BUFSIZE;
    if(first == r) return;
    buffer[r] = data;
    last = r;
}

int PTZcontrol::dequeue()
{
    first = (first+1)%BUFSIZE;
    return buffer[first];
}

void PTZcontrol::showqueue()
{
    int c = first;
    while(c != last)
    {
        c = (c+1)%BUFSIZE;
        cout << str[buffer[c]/10-1] << "/" << buffer[c]%10;
        if(c != last) cout << " | ";
    }
    cout << endl;
}
