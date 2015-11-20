#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <stddef.h>
#include <stdarg.h>

#include <stddef.h>
#include <stdarg.h>
#include <fcntl.h>

#include <termios.h>
#include <netinet/ether.h>
#include <netpacket/packet.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>

int fdusb = 0;
char usbbuf[1024*4];
int nread = 0;

int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop)
{
    struct termios newtio,oldtio;
    if  ( tcgetattr( fd,&oldtio)  !=  0) {
        perror("SetupSerial 1");
        return -1;
    }
    bzero( &newtio, sizeof( newtio ) );
    newtio.c_cflag  |=  CLOCAL | CREAD;
    newtio.c_cflag &= ~CSIZE;

    switch( nBits )
    {
    case 7:
        newtio.c_cflag |= CS7;
        break;
    case 8:
        newtio.c_cflag |= CS8;
        break;
    }

    switch( nEvent )
    {
    case 'O':
        newtio.c_cflag |= PARENB;
        newtio.c_cflag |= PARODD;
        newtio.c_iflag |= (INPCK | ISTRIP);
        break;
    case 'E':
        newtio.c_iflag |= (INPCK | ISTRIP);
        newtio.c_cflag |= PARENB;
        newtio.c_cflag &= ~PARODD;
        break;
    case 'N':
        newtio.c_cflag &= ~PARENB;
        break;
    }

    switch( nSpeed )
    {
    case 2400:
        cfsetispeed(&newtio, B2400);
        cfsetospeed(&newtio, B2400);
        break;
    case 4800:
        cfsetispeed(&newtio, B4800);
        cfsetospeed(&newtio, B4800);
        break;
    case 9600:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    case 115200:
        cfsetispeed(&newtio, B115200);
        cfsetospeed(&newtio, B115200);
        break;
    case 460800:
        cfsetispeed(&newtio, B460800);
        cfsetospeed(&newtio, B460800);
        break;
    default:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    }
    if( nStop == 1 )
        newtio.c_cflag &=  ~CSTOPB;
    else if ( nStop == 2 )
    newtio.c_cflag |=  CSTOPB;
    newtio.c_cc[VTIME]  = 0;//重要
    newtio.c_cc[VMIN] = 1;//返回的最小值  重要
    tcflush(fd,TCIFLUSH);
    if((tcsetattr(fd,TCSANOW,&newtio))!=0)
    {
        perror("com set error");
        return -1;
    }
//  //printf("set done!\n\r");
    return 0;
}

int openusb(){
	int nret;
    fdusb = open("/dev/ttyUSB0", O_RDWR);//打开串口
    if (fdusb == -1)
    {
        printf("open USB failed!!\n");
        return 1;
    }
    nret = set_opt(fdusb,115200, 8, 'N', 1);//设置串口属性
    if (nret == -1)
    	 printf("open USB failed!!\n");
        return 1;
}



int main(void)
{
	if(openusb())
	{
    		printf("failed to initialize the USB");
	}
	while(1){
    	 nread=read(fdusb, usbbuf, 1024*4);
		if(nread == -1)
		{
			perror("error for usb reading");
			exit(0);
		}
         
    	 printf("This Read function read  %d byte usb data\n",nread);
    	 printf("The usb data is :%s\n",usbbuf);
    }
}
