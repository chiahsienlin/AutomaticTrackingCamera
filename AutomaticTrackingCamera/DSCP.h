#ifndef DSCP_H
#define DSCP_H

/*
_____________DynaColor Surveillance Control Protocol(DSCP)_____________

*UART Protocol:
    Baud rate:  9600    Start bit:  1   Data bit:   8
    Parity:     None    Stop bit:   1

*Command Packet:
    Every command is composed of six bytes;
    the packet format is described as below.
    The time delay between each byte must be smaller than 2 ms.
    Device response time must be less than 100ms,
    and Host commands' gap time must be more than 100 ms.

        Byte1         Byte2       Byte3    Byte4    Byte5    Byte6
    Receiver ID  Transmitter ID  OP code   Data0    Data1  Check Sum

    The first byte is the Receiver or 'Destination' device ID.
    The second byte is the Transmitter or 'Source' device ID.
    The third byte is the OP code or 'command' byte.
    The fourth and fifth bytes are the data byte.
    The sixth byte is the check sum of this command packet.
    Checksum = Byte1 XOR Byte2 XOR Byte3 XOR Byte4 XOR Byte5
*/

/* This file uses RS232 comport to send packet. (with rs232.h) */
/* Date: Nov 12, 2014 */

#include "rs232.h"
#define R_ID 0x01  //Receiver ID
#define T_ID 0x00  //Transmitter ID

const int cport_nr=0, bdrate=9600;
enum PAN  {RIGHT=0x00, LEFT=0x01, P_STOP};
enum TILT {UP=0x02, DOWN=0x03, T_STOP};
enum ZOOM {WIDE=0x00, TELE=0x01, Z_STOP=0x04};
enum PRESET {SET=0x00, CLEAR=0x01};

class DSCP
{
    public:
        DSCP(unsigned char r=R_ID, unsigned char t=T_ID) { packet[0]=r; packet[1]=t; }
        int Pan(PAN action, unsigned char speed);  //action = {RIGHT, LEFT, P_STOP}; speed = 0x00~0x07
        int Tilt(TILT action, unsigned char speed);  //action = {UP, DOWN, T_STOP}; speed = 0x00~0x07
        int Zoom(ZOOM action);  //action = {WIDE, TELE, Z_STOP}
        int ZoomSpeed(unsigned char speed);  //speed = 0x00~0x03
        int SetPanSpeed(unsigned char speed);  //speed = 0x00~0x07
        int SetTiltSpeed(unsigned char speed);  //speed = 0x00~0x07
        int PresetSet(PRESET action, unsigned char N);
        int SetPresetToHome(unsigned char N);
        int GotoHomePos();
        int GotoPanPos(int degree);  //degree = 0~3600 step ( 1step = 0.1degree )
        int GotoTiltPos(int degree);  //degree = 0~900 step ( 1step = 0.1degree )
        //after used Request function, it has to call "RS232_PollComport" function to get the Response
        int ReqPanPos();
        int ReqTiltPos();

    private:
        unsigned char packet[6];
};

#endif // DSCP_H
