/*
 * ndw.h
 *
 *  Created on: 2015-11-16
 *      Author: cb
 */

#ifndef NDW_H
#define NDW_H

/*
Main function of NDW.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <netinet/ether.h>
#include <netpacket/packet.h>
#include <string.h>

//#include "ndnpoke.h"
#include "define.h"

extern BTNode *topo_head;
extern char *scope_name;

int g_count=0;
//uint16_t co2nodereq = 0;
int co2res = 0;
int armmark = 0;
int nodemark = 0;
int neimark = 0;
int pathmark = 0;
FILE *fp;

/**
 * Cal the CRC Byte
 */
uint16_t crcByte(uint16_t crc, uint8_t b) {
  crc = (uint8_t)(crc >> 8) | (crc << 8);
  crc ^= b;
  crc ^= (uint8_t)(crc & 0xff) >> 4;
  crc ^= crc << 12;
  crc ^= (crc & 0xff) << 5;
  return crc;
}

/**
 * Cal the CRC Val
 */
uint16_t crccal(unsigned char* buf, int len)
{
	uint16_t res = 0;
	int i;

	for ( i = 0; i < len; ++i)
	{
		res = crcByte(res, buf[i]);
	}

	return res;
}

/**
 * Print the MAC addr in HEX
 */
void printhex_macaddr(void *hex, int len, char *tag)
{
	int i;
	unsigned char *p = (unsigned char *)hex;

	if(len < 1)
		return;

	for(i = 0; i < len - 1; i++)
	{
		if(*p < 0x10)
			printf("0%x%s", *p++, tag);
		else
			printf("%2x%s", *p++, tag);
	}

	if(*p < 0x10)
		printf("0%x", *p++);
	else
		printf("%2x", *p++);
}

void topo_tree_traversal(BTNode **node, int *count, char *buf, char *temp)
{
	int i;
	sprintf(temp, "%d %d\n", (*node)->nodeID, (*node)->parent->nodeID);
	strcat(buf, temp);
	(*count)++;
	for(i=0; i<(*node)->child_num; i++)
	{
		topo_tree_traversal(&((*node)->ptr[i]), count, buf, temp);
	}
}

/**
 * modify by cb
 * write the interest into USB
 * this part code should use in Yulin gateway;
 */
void usb_write(interest_name *interest, uint16_t type)
{
	DEBUG printf("enter the function usb_write~~\n");
	int nread;

	tinyosndw_packet *packet = (tinyosndw_packet*)malloc(sizeof(tinyosndw_packet));
	memset((char*)packet, 0, sizeof(tinyosndw_packet));
	packet->head.F = 0x7e;
	packet->head.P = 0x44;
	packet->head.S = 0x26;
	packet->head.D = 0x00;

	packet->payload.dst_addr = 0x01;
	packet->payload.dst_addr = packet->payload.dst_addr<<8;
	packet->payload.src_addr = 0x00;
	packet->payload.src_addr = packet->payload.src_addr<<8 ;
	packet->payload.msg_len = 0x0E;
	packet->payload.groupID = 0x22;
	packet->payload.handlerID = 0x03;

	packet->payload.content.msgType = type;
	memcpy(&(packet->payload.content.msgName), interest, sizeof(interest_name));

	//packet->payload.content.data[0] = 0x01;
	//packet->payload.content.data[2] = 0xff;
	//packet->payload.content.data[3] = 0xff;

	packet->end.CRC = crccal((char*)packet+1, sizeof(tinyosndw_packet)-4);

	packet->end.F = 0x7e;
	printhex_macaddr((char*)packet, sizeof(tinyosndw_packet), " ");
	uint8_t loopflag=1;
	uint8_t ARQ=0;
	while(loopflag)
	{
		nread = write(fdusb, (char*)packet, sizeof(tinyosndw_packet));
		if(nread == -1)
		{
			if(ARQ<5)
			{
				printf("write failed!!\n");
				ARQ++;
			}
			else
			{
				printf("failed to write the interest into the USB port!\n");
				loopflag=0;
			}
		}
		else
		{
			loopflag=0;
			DEBUG printf("write usb successed!!-----NO:[%d]\n", g_count++);
		}
	}
	free(packet);
}

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
    fdusb = open(USB_PATH_PORT, O_CREAT | O_APPEND | O_RDWR );//打开串口
    if (fdusb == -1)
    {
        printf("open USB failed!!\n");
        exit(0);
        return 1;
    }
    nret = set_opt(fdusb,115200, 8, 'N', 1);//设置串口属性
    if (nret == -1){
    	 printf("open USB failed!!\n");
        exit(0);
        return 1;
    }
   return 0;
}

/**
 * entrence of ndw
 */


#endif
