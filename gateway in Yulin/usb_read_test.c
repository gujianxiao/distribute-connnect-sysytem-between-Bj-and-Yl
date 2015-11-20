/*
 * usb_read_test.c
 *
 *  Created on: 2015-11-20
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


int fdusb=0;

int thread_flag=1;
pthread_t pid_listen;
pthread_t pid_topo;

pthread_t pid_work;
Msg *recv_data;
int data_flag = 0;
char *scope_name=NULL;

sem_t sem_queue;
sem_t sem_interest;
BTNode *topo_head=NULL;
/*modified by zhy on 20141230*/
uint8_t topo_tree_node_check[256];
/**
 * Set the USB Port
 */

int main(int argc, char **argv){

    if(openusb()){
    	printf("failed to initialize the USB");
    }
    else
    	{
    	printf("initialize usb success!");
    	}
    DEBUG printf("create listening thread successed!!\n");
    int i,j;
    int nread;
    //unsigned char start_mark[] = {0x7e,0x45,0x00};
    char *start_p;
    char usbbuf[1024];
    unsigned char name_buf[64];
    unsigned char content_buf[10];
    uint16_t content;
    int total_read=0;
    int datagotflag=0;

    int move_pos;
    recv_data = (Msg *)malloc(sizeof(Msg));
    total_read=0;
    while(thread_flag)
    {
        //memset(usbbuf, 0, 1024);

        datagotflag=0;
        while(total_read<SHORTEST_DATA_FRAME_LEN)//每次读取至少DATA_FRAME_LEN字节的数据(数据包长DATA_FRAME_LEN)
        {
            nread = read(fdusb, usbbuf+total_read, 1024);//读USB串口
            total_read += nread;
        }
         DEBUG printhex_macaddr(usbbuf, total_read, " ");
         printf("\n");
        for(i=0; i<total_read-7; i++)//找到数据包标志
        {
            if(usbbuf[i] == 0x7e && usbbuf[i+1] == 0x45 && usbbuf[i+2] == 0x00 && usbbuf[i+3] == 0x00
                && usbbuf[i+4] == 0x00 && usbbuf[i+5] == 0x00 && usbbuf[i+6] == 0x01)
            {
                datagotflag=1;
                //DEBUG printf("total_read=%d, i=%d\n", total_read, i);
                while(total_read-i<SHORTEST_DATA_FRAME_LEN)//标志后的数据不够DATA_FRAME_LEN字节，继续读数据
                {
                    nread = read(fdusb, usbbuf+total_read, 1024);
                    total_read += nread;
                    //DEBUG printf("total_read=%d, i=%d\n", total_read, i);
                }
                start_p = usbbuf + i;//start_p point to the begining of the frame
                break;
            }
        }
        if(!datagotflag)//没找到标志位，不予处理
        {
                printf("not found the start flag!\n");

                for(move_pos=total_read-1; move_pos>=total_read-7; move_pos--)
                {
                    if(usbbuf[move_pos] == 0x7e)
                        break;
                }
                if(move_pos<total_read-7)
                {
                    total_read = 0;
                    continue;
                }
                for(j=move_pos,i=0; j<total_read; j++, i++)
                {
                        usbbuf[i] = usbbuf[j];
                }
                total_read -= move_pos;
                continue;
        }
        //DEBUG printf("got the packet and nread=%d\n", total_read);

       if (*(start_p+10) == DATA)//内容包
        {
                    while(total_read<28+start_p-usbbuf)//每次读取至少28字节的数据
                    {
                        nread = read(fdusb, usbbuf+total_read, 1024);//读USB串口
                        total_read += nread;
                    }
                    //DEBUG printf("Got a data message!\n");
                    DEBUG printhex_macaddr(start_p, 28, " ");
                    DEBUG printf("\n");

                    memcpy(recv_data, start_p + 10, sizeof(Msg));
                    if(recv_data->msgType == DATA)
                    {
                        DEBUG printf("Got Sensor data!\n");

                        memset(name_buf, 0, sizeof(name_buf));
                        memset(content_buf, 0, sizeof(content_buf));
                        content = recv_data->data;
                        sprintf(name_buf, "ndn:/%s/ints/%hd,%hd/%hd,%hd/", NAME_PREFIX, recv_data->msgName.ability.leftUp.x, recv_data->msgName.ability.leftUp.y,
                                                                                                recv_data->msgName.ability.rightDown.x, recv_data->msgName.ability.rightDown.y);
                        if(recv_data->msgName.dataType == Light)
                            strcat(name_buf, "light");
                        if(recv_data->msgName.dataType == Temp)
                            strcat(name_buf, "temp");
                        if(recv_data->msgName.dataType == Humidity)
                            strcat(name_buf, "humidity");
                        printf("interest name = %s\n", name_buf);
                        sprintf(content_buf, "%hd\n", content);
                        printf("content data = %s", content_buf);
//                        pack_data_content(name_buf, content_buf);

                        data_flag = 1;
                    }
        }
        else if (*(start_p+10) == TOPOLOGY)//topology packet
        {
                while(total_read<37+start_p-usbbuf)//每次读取至少38字节的数据
                {
                        nread = read(fdusb, usbbuf+total_read, 1024);//读USB串口
                        total_read += nread;
                }
                DEBUG printf("Got a topology message!\n");
                DEBUG printhex_macaddr(start_p, 37, " ");
                DEBUG printf("\n");
                topo_msg *recv_topo = (topo_msg*) malloc(sizeof(topo_msg));
                memcpy(recv_topo, start_p + 12, sizeof(topo_msg));
                printf("num=%d\n", recv_topo->num);

                if(top!=(bottom-1+PTR_MAX)%PTR_MAX)
                {
                      queue[top++] = recv_topo;
                      if(top>=PTR_MAX)
                                   top = top%PTR_MAX;
//                      sem_post(&sem_queue);
                }
        }
        else if(*(start_p+10) == MAPPING)
        {
                while(total_read<25+start_p-usbbuf)//每次读取至少26字节的数据
                {
                        nread = read(fdusb, usbbuf+total_read, 1024);//读USB串口
                        total_read += nread;
                }
                DEBUG printf("Got a mapping message!\n");
                DEBUG printhex_macaddr(start_p, 25, " ");
                DEBUG printf("\n");

                node_info *node = (node_info*) malloc(sizeof(node_info));
                memcpy(node, start_p + 12, sizeof(node_info));
                nodeID_mapping_table[node->nodeID].coordinate=node->coordinate.leftUp;
                nodeID_mapping_table[node->nodeID].expire = 5;
                printf("nodeID=%d->(%d,%d)\n", node->nodeID, nodeID_mapping_table[node->nodeID].coordinate.x, nodeID_mapping_table[node->nodeID].coordinate.y);
                free(node);
        }
        for(move_pos=total_read-1; move_pos>=total_read-7; move_pos--)
        {
            if(usbbuf[move_pos] == 0x7e)
                break;
        }
        if(move_pos<total_read-7)
        {
            total_read = 0;
            continue;
        }
        for(j=move_pos,i=0; j<total_read; j++, i++)
        {
                usbbuf[i] = usbbuf[j];
        }
        total_read -= move_pos;
    }
    exit(0);
}








