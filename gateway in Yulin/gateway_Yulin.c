/*
 * gateway_Yulin.c
 *
 *  Created on: 2015-11-17
 *      Author: cb
 */

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

#include"ndw.h"
#include"define.h"

#define LISTENQ         1024    /* 2nd argument to listen() */
#define SA      struct sockaddr
#define SERV_PORT 11111
#define MAXLINE 1024*4

pthread_t pid_listen;
pthread_t pid_getting;
int thread_flag=1;
int fdusb;
    int sockfd;
    struct sockaddr_in servaddr;


//


//The thread use to read data from usb then send it to UDPpackage
void usbportListen(void *arg)
{
    perror("get into usbListen:");
    if(openusb()){
    	printf("failed to initialize the USB\n");
    }
    else
    	{
    	printf("initialize usb success!\n");
    	}
    char usbbuf[MAXLINE];
    int nread;
    while( thread_flag){
    	int total_read=0;
    	while(total_read<SHORTEST_DATA_FRAME_LEN){
    	 nread=read(fdusb, usbbuf+total_read, 1024);
    	 total_read += nread;
    	}
    	 //send to Beijing
    	if(nread!=-1) sendto(sockfd, usbbuf, total_read, 0,  (SA *) &servaddr, sizeof(servaddr));
        printhex_macaddr(usbbuf, total_read, " ");
        printf("\n");
//    	else printf("read data error!");
         if(nread==-1) perror("error for usb reading:");
    	 printf("This Read function read  %d byte usb data\n",nread);
    	 printf("The usb data is :%s\n",usbbuf);
    }
    return ;

}


//this function use to read data from Beijing then send it to usb
void udpportListen(void *arg){
    char udpbuf[MAXLINE];
    int total_read=0;
    int n=0;
    while(thread_flag){
    	 //get from  Beijing
    	 n= recvfrom(sockfd, udpbuf, MAXLINE, 0, NULL, NULL);
         udpbuf[n]=0;//end the udpbuf for print;
//    	 usb_write(udpbuf, IN);
    	 printf("This Read function read %d byte UDP data\n",sizeof(udpbuf));
    	 printf("The UDP data is :%s\n",udpbuf);
    }
   return ;
}

//this function use to build connectin with Beijing
int build_connect(){

    int n=0;
    char  recvline[MAXLINE + 1];
  
    char sendline[]="Try to build connection...\n";
    
      while (n == 0){
        sendto(sockfd, sendline, strlen(sendline), 0,  (SA *) &servaddr, sizeof(servaddr));
        n = recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
        printf("pass the recv function...");
//        recvline[n] = 0;        /* null terminate */
        if (n==0) printf("this time build connection failed! 10s try again...");

       }
        if(n!=0){
                 printf("recieve from server:%s\n",recvline);          
        	 printf("build connection success!\n");
        	 return 1;
        }
        else {
              return 0;
        }
}



//this function use to create dealing_wsn_data thread and dealing_udp_data thread
void connecting(){
	  int res1=0;
	  int res2=0;
                printf("getting start to create thread...");
	        res1 = pthread_create(&pid_listen, NULL, usbportListen, NULL);//开启接收usb数据线程
	        if(res1!=0)
	        {
	            printf("create listening usb thread error!!\n");
	        }
	        res2= pthread_create(&pid_getting, NULL, udpportListen, NULL);//开启接收udp数据线程
	        if(res2!=0)
	        {
	            printf("create listening udp thread error!!\n");
	        }

}


int main(int argc, char **argv){
	//general socket setting

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_port        = htons(SERV_PORT);

    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(build_connect()){
    printf("begin to communication...");
    connecting();
    }
    else printf("build connection failed!");

    //
//    pthread_detach(pid_listen);
//    pthread_detach(pid_getting);
//    close(fdusb);
//   dealing with the relationship between father_thread and son_thread; 
    pthread_exit(NULL);
    exit(0);
}


