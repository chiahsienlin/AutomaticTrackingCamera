#include "DSCP.h"

int DSCP::Pan(PAN action, unsigned char speed)  //action = {RIGHT, LEFT, P_STOP}; speed = 0x00~0x07
{
    switch(action)
    {
        case P_STOP:
            packet[2]=0x13; packet[3]=0x00; packet[4]=0x00;
            break;
        case RIGHT:
        case LEFT:
            packet[2]=0x18; packet[3]=action; packet[4]=speed;
            break;
    }
    packet[5]=packet[0]^packet[1]^packet[2]^packet[3]^packet[4];

    return RS232_SendBuf(cport_nr, packet, sizeof(packet));
}

int DSCP::Tilt(TILT action, unsigned char speed)  //action = {UP, DOWN, T_STOP}; speed = 0x00~0x07
{
    switch(action)
    {
        case T_STOP:
            packet[2]=0x14; packet[3]=0x00; packet[4]=0x00;
            break;
        case UP:
        case DOWN:
            packet[2]=0x18; packet[3]=action; packet[4]=speed;
            break;
    }
    packet[5]=packet[0]^packet[1]^packet[2]^packet[3]^packet[4];

    return RS232_SendBuf(cport_nr, packet, sizeof(packet));
}

int DSCP::Zoom(ZOOM action)  //action = {WIDE, TELE, Z_STOP}
{
    packet[2]=0x24; packet[3]=action; packet[4]=0x00;
    packet[5]=packet[0]^packet[1]^packet[2]^packet[3]^packet[4];

    return RS232_SendBuf(cport_nr, packet, sizeof(packet));
}

int DSCP::ZoomSpeed(unsigned char speed)  //speed = 0x00~0x03
{
    packet[2]=0x24; packet[3]=0x03; packet[4]=speed;
    packet[5]=packet[0]^packet[1]^packet[2]^packet[3]^packet[4];

    return RS232_SendBuf(cport_nr, packet, sizeof(packet));
}

int DSCP::SetPanSpeed(unsigned char speed)  //speed = 0x00~0x07
{
    packet[2]=0x0B; packet[3]=speed; packet[4]=0x00;
    packet[5]=packet[0]^packet[1]^packet[2]^packet[3]^packet[4];

    return RS232_SendBuf(cport_nr, packet, sizeof(packet));
}

int DSCP::SetTiltSpeed(unsigned char speed)  //speed = 0x00~0x07
{
    packet[2]=0x0C; packet[3]=speed; packet[4]=0x00;
    packet[5]=packet[0]^packet[1]^packet[2]^packet[3]^packet[4];

    return RS232_SendBuf(cport_nr, packet, sizeof(packet));
}

int DSCP::PresetSet(PRESET action, unsigned char N)
{
    packet[2]=0x1D; packet[3]=action; packet[4]=N;
    packet[5]=packet[0]^packet[1]^packet[2]^packet[3]^packet[4];

    return RS232_SendBuf(cport_nr, packet, sizeof(packet));
}

int DSCP::SetPresetToHome(unsigned char N)
{
    packet[2]=0x49; packet[3]=0x03; packet[4]=N;
    packet[5]=packet[0]^packet[1]^packet[2]^packet[3]^packet[4];

    return RS232_SendBuf(cport_nr, packet, sizeof(packet));
}

int DSCP::GotoHomePos()
{
    packet[2]=0x49; packet[3]=0x02; packet[4]=0x00;
    packet[5]=packet[0]^packet[1]^packet[2]^packet[3]^packet[4];

    return RS232_SendBuf(cport_nr, packet, sizeof(packet));
}

int DSCP::GotoPanPos(int degree)  //degree = 0~3600 step ( 1step = 0.1degree )
{
    packet[2]=0x29; packet[3]=degree/256; packet[4]=degree%256;
    packet[5]=packet[0]^packet[1]^packet[2]^packet[3]^packet[4];

    return RS232_SendBuf(cport_nr, packet, sizeof(packet));
}

int DSCP::GotoTiltPos(int degree)  //degree = 0~900 step ( 1step = 0.1degree )
{
    packet[2]=0x2A; packet[3]=degree/256; packet[4]=degree%256;
    packet[5]=packet[0]^packet[1]^packet[2]^packet[3]^packet[4];

    return RS232_SendBuf(cport_nr, packet, sizeof(packet));
}

//after used Request function, it has to use "RS232_PollComport" function to get the Response
int DSCP::ReqPanPos()
{
    packet[2]=0x09; packet[3]=0x01; packet[4]=0x00;
    packet[5]=packet[0]^packet[1]^packet[2]^packet[3]^packet[4];

    return RS232_SendBuf(cport_nr, packet, sizeof(packet));
}

int DSCP::ReqTiltPos()
{
    packet[2]=0x0A; packet[3]=0x01; packet[4]=0x00;
    packet[5]=packet[0]^packet[1]^packet[2]^packet[3]^packet[4];

    return RS232_SendBuf(cport_nr, packet, sizeof(packet));
}
