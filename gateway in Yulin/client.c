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
#define MAXLINE 200

    int sockfd;
    struct sockaddr_in servaddr;
    int fdusb;

void dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen){
    int n;
    char sendline[MAXLINE], recvline[MAXLINE + 1];

    while (fgets(sendline, MAXLINE, fp) != NULL) {
        sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);
        n = recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
        recvline[n] = 0;        /* null terminate */
        fputs(recvline, stdout);
    }
}

int build_connect(){

    int n=0;
    char  recvline[MAXLINE + 1];
  
    char sendline[]="Try to build connection...\n";
    
      while (n == 0){
        sendto(sockfd, sendline, strlen(sendline), 0,  (SA *) &servaddr, sizeof(servaddr));
        n = recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
//        recvline[n] = 0;        /* null terminate */
        if (n==0) printf("this time build connection failed! 10s try again...");
        sleep(10);//wait ten seconds then connect again
       }
        fputs(recvline, stdout);
        if(recvline!=NULL)  {
                 printf("recieve from server:%s\n",recvline);          
        	printf("build connection success!\n");
        	return 1;
        }
        else return 0;
}
int main(int argc, char **argv){

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_port        = htons(SERV_PORT);

    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(build_connect())
    {
        
    }

    //dg_cli(stdin, sockfd, (SA *) &servaddr, sizeof(servaddr));

    exit(0);
}
