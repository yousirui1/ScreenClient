#include "recvthread.h"
#include <errno.h>
#include <unistd.h>
#include <time.h>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#endif

#include "queue.h"
#include <QDebug>

#define DATA_SIZE 1452
#define MAX_VIDSBUFSIZE 1024 * 1024


unsigned char ucVidsBuf[MAX_VIDSBUFSIZE];
QUEUE stVidsQueue;


RecvThread::RecvThread(char *ip, int port)
{
   Init_Queue(&stVidsQueue,ucVidsBuf,MAX_VIDSBUFSIZE);
   qWarning("ip %s, port %d", ip, port);
   this->ip = ip;
   this->port = port;
}

void RecvThread::run()
{
#ifdef _WIN32
    WSADATA wsData = {0};
    if(0 != WSAStartup(0x202, &wsData))
    {
        WSACleanup();
        return;
    }
    int socklen;
#else
    socklen_t socklen;
#endif

    struct sockaddr_in send_addr,recv_addr;
    int sockfd;
    struct ip_mreq mreq;

    fd_set fds;
    struct timeval tv;
    struct sockaddr_in fd_addr;
    int ret = -1;

    tv.tv_sec = 1;

    time_t current_time;
    time_t last_time;

    socklen = sizeof (struct sockaddr_in);
    int opt = 0;

    /* 创建 socket 用于UDP通讯 */
    sockfd = socket (AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        printf ("socket creating err in udptalk\n");
        exit (1);
    }
    /* 设置要加入组播的地址 */
    memset(&mreq, 0, sizeof (struct ip_mreq));
    mreq.imr_multiaddr.s_addr = inet_addr(this->ip);
    /* 设置发送组播消息的源主机的地址信息 */
    mreq.imr_interface.s_addr = htonl (INADDR_ANY);

    /* 把本机加入组播地址，即本机网卡作为组播成员，只有加入组才能收到组播消息 */
     if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP , (char *)&mreq,sizeof (struct ip_mreq)) == -1)
     {
         perror ("setsockopt");
         exit (-1);
     }

    memset (&send_addr, 0, socklen);
    send_addr.sin_family = AF_INET;
    send_addr.sin_port = htons (this->port);
    send_addr.sin_addr.s_addr = inet_addr(this->ip);

	opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR , (char *)&opt, sizeof(opt)) < 0)
    {

        perror("setsockopt");
        exit (-1);
    }

    opt = 1;
    if (setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&opt,sizeof (opt)) == -1)
    {
        printf("IP_MULTICAST_LOOP set fail!\n");
    }

    opt = 32*1024;//设置为32K
    if (setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (char *)&opt, sizeof (opt)) == -1)
    {
        printf("IP_MULTICAST_LOOP set fail!\n");
    }

    opt = 32*1024;//设置为32K
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (char *)&opt, sizeof (opt)) == -1)
    {
        printf("IP_MULTICAST_LOOP set fail!\n");
    }

    recv_addr.sin_family = AF_INET;
    recv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
    recv_addr.sin_port = htons(this->port);


    /* 绑定自己的端口和IP信息到socket上 */
    if (bind(sockfd, (struct sockaddr *) &recv_addr,sizeof (struct sockaddr_in)) == -1)
    {
        perror("Bind error");
        exit (0);
    }

    unsigned int total_size = 0;
    unsigned char buf[MAX_VIDSBUFSIZE] = {0};
    unsigned int offset = 0;
    unsigned char *tmp;
    unsigned short count = 0;
    unsigned short current_count = 0;
    time(&current_time);
    last_time = current_time;
    while(1)
    {
        FD_ZERO(&fds);
        FD_SET(sockfd, &fds);

        ret = select(sockfd + 1, &fds, NULL, NULL, &tv);
        if(ret < 0)
            continue;
        if(FD_ISSET(sockfd, &fds))
        {
            time(&last_time);
            ret = recvfrom(sockfd, (char *)buf + offset, MAX_VIDSBUFSIZE - offset, 0, (struct sockaddr*) &recv_addr, &socklen);
            tmp = &buf[offset];
            offset += ret;
            if(errno == EINTR || errno == EAGAIN)
            {
                continue;
            }
            else
            {
                //emit sigNoRecv();  //发送信号
                //break;
            }
            qWarning("ret %d",ret);
            if(tmp[0] == 0xff && tmp[1] == 0xff)
            {
                count = *((unsigned short *)&tmp[2]);
                if(count != current_count)
                {
                    if(current_count == 0)
                    {
                        current_count = count;
                        continue;
                    }
                    total_size = *((unsigned int *)&tmp[4]);
                    En_Queue(&stVidsQueue, buf + 8, offset - ret - 8, 0x0);
                    memcpy(buf, tmp, ret);
                    offset = ret;
                    current_count = count   ;
                }
                else
                {
                    if(offset == total_size + 8)
                    {
                        En_Queue(&stVidsQueue, buf + 8, offset - 8, 0x0);
                        offset = 0;
                    }
                }
            }
        }
        time(&current_time);

        if(current_time - last_time > 3)
        {
            //emit sigNoRecv();  //发送信号
            //break;
        }
    }
#ifdef _WIN32
    closesocket(sockfd);
#else
   ::close(sockfd);
#endif
    quit();
}


#if 0
        if(FD_ISSET(sockfd, &fds))
        {
            ret = recvfrom(sockfd, (char *)buf + offset, MAX_VIDSBUFSIZE - offset, 0, (struct sockaddr*) &recv_addr, &socklen);
            tmp = &buf[offset];
            offset += ret;
            if(errno == EINTR || errno == EAGAIN)
            {
                continue;
            }
            else
            {
                //break;
            }

            if(tmp[0] == 0xff && tmp[1] == 0xff && tmp[2] == 0xff && tmp[3] == 0xff)
            {
                if(totalSize == 0)
                {
                    totalSize = *((unsigned int *)&tmp[4]);
                    continue;
                }
                if(offset - ret - 8 > 0)
                {
                    totalSize = *((unsigned int *)&tmp[4]);
                    En_Queue(&stVidsQueue, buf + 8, offset - ret - 8, 0x0);
                    memcpy(buf, tmp, ret);
                    offset = ret;
                }
            }
            if(offset == totalSize + 8)
            {
                En_Queue(&stVidsQueue, buf + 8, offset - 8, 0x0);
                offset = 0;
            }
        }

#endif
