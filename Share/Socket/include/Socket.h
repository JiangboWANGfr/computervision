/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-01 10:03:32 +0800
 * @LastEditTime: 2019-11-16 16:28:36 +0800
 * @LastEditors: 
 * @Description: 
 */
#ifndef __SOCKET_H__
#define __SOCKET_H__
#include "header.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

typedef int socketfd;

class Socket
{
public:
    char data[1024];

private:
    //SOCK_STREAM是TCP模式，SOCK_DGRAM是UDP模式
    int mode;
    int port;
    char ip[100];
    //服务器句柄
    socketfd ser_skf;

private:
    void reUse(socketfd skf);
    int recvMSGFromClient(socketfd ser_skf);
    int sendMSG(socketfd, char *buff, size_t n_bytes, int flag);
    int receiveMSG(socketfd skf, char *buff, size_t max_bytes, int flag);
    void writeBack(socketfd skf, const void *buff, size_t n_bytes, int flag);

public:
    Socket(int sock_stream, int port, string ip_addr);
    ~Socket();

    void startServer();
    void recvDataFromClient();

    void getLocalIP(char *IP);

    int sendToServer(char *data);

    void createConnection(socketfd skf, struct sockaddr *serv_addr, size_t addr_len);

    socketfd createSocket(int type, int protocol);

    void initialzeSocketaddr(struct sockaddr_in *addr, char *ip_addr, int port);

    void bindSocketAddr(socketfd skf_socketfd, struct sockaddr *addr_sockaddr, int addrlen);

    void createListen(socketfd skf, int num);

    socketfd acceptConnection(socketfd sfk, struct sockaddr *addr, socklen_t len_addr);
};

#endif