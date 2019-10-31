/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-08-30 21:22:06 +0800
 * @LastEditTime: 2019-10-31 19:35:45 +0800
 * @LastEditors: 
 * @Description: 
 */
#ifndef __SOCKET_H__
#include "Socket.h"
#define BUFFER_SIZE 1024

/**
 * @Author: 王占坤
 * @Description: 创建socket对象
 * @Param: int mode SOCK_STREAM是TCP模式，SOCK_DGRAM是UDP模式
 * @Param: int port 端口号
 * @Param: char* ip_addr ip地址
 * @Return: 
 * @Throw: 
 */
Socket::Socket(int mode, int port, string ip_addr)
{
    this->mode = mode;
    this->port = port;
    strcpy(this->ip, ip_addr.c_str());
    memset(this->data, 0, sizeof(this->data));
}
Socket::~Socket()
{
}

/**
 * @Author: 王占坤
 * @Description: 这个是需要在服务端启用的接收信息的进程。
 * @Param: 无
 * @Return: socketfd 建立好的用于监听的句柄
 * @Throw: 
 */
void Socket::startServer()
{
    ser_skf = createSocket(this->mode, 0);
    if(ser_skf == -1)
        return;
    struct sockaddr_in ser_addr;
    initialzeSocketaddr(&ser_addr, ip, this->port);
    bindSocketAddr(ser_skf, (struct sockaddr *)&ser_addr, sizeof(ser_addr));
    struct sockaddr_in client;
    createListen(ser_skf, 200);
    return;
}

void Socket::recvDataFromClient()
{
    recvMSGFromClient(ser_skf);
}

/**
 * @Author: 王占坤
 * @Description: 创建socket进程
 * @Param: int type SOCK_STREAM是TCP模式，SOCK_DGRAM是UDP模式
 * @Param: int protocol 一般传入0即可
 * @Return: socketfd 返回创建的socket
 * @Throw: 
 */
socketfd Socket::createSocket(int type, int protocol)
{
    socketfd skf = socket(AF_INET, this->mode, 0);

    if (skf == -1)
    {
        perror("socket");
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        return -1;
    }
    else
    {
        reUse(skf);
        return skf;
    }
}

/**
 * @Author: 王占坤
 * @Description: 初始化地址
 * @Param: struct sockaddr_in* addr  待初始化的地址指针
 * @Param：in_addr_t ip_addr, ip地址,NULL代表自动获取ip
 * @Param: int port， 端口号
 * @Return: void
 * @Throw: 
 */
void Socket::initialzeSocketaddr(struct sockaddr_in *addr, char *ip_addr, int port)
{
    memset(addr, 0, sizeof(addr));
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    if (ip_addr == NULL)
        addr->sin_addr.s_addr = htonl(INADDR_ANY);
    else

        addr->sin_addr.s_addr = inet_addr(ip_addr);
}

/**
 * @Author: 王占坤
 * @Description: 将socket绑定至指定地址
 * @Param: socketfd* skf_socketfd 待被绑定地址的socket句柄
 * @Param：const struct sockaddr* addr_sockaddr被绑定的地址
 * @Param：int addrlen 地址长度
 * @Return: void
 * @Throw: 
 */
void Socket::bindSocketAddr(socketfd skf_socketfd, struct sockaddr *addr_sockaddr, int addrlen)
{
    int res = bind(skf_socketfd, addr_sockaddr, addrlen);
    if (res == -1) //失败
    {
        perror("bind");
    }
}

/**
 * @Author: 王占坤
 * @Description: 对需要进行监听的socket开启监听
 * @Param: socketfd& skf, 需要进行监听的socket
 * @Param：int num, 最大监听数量
 * @Return: void
 * @Throw: 
 */
void Socket::createListen(socketfd skf, int num)
{
    int res = listen(skf, num);
    if (res == -1) //执行失败
    {
        perror("listen");
    }
}

/**
 * @Author: 王占坤
 * @Description: 获取本机IP地址
 * @Param: char* IP 一个char指针，用于保存IP
 * @Return: 
 * @Throw: 
 */
void Socket::getLocalIP(char *IP)
{
    int inet_sock;
    struct ifreq ifr;
    inet_sock = socket(AF_INET, SOCK_DGRAM, 0);

    strcpy(ifr.ifr_name, "ens33");
    //SIOCGIFADDR标志代表获取接口地址
    if (ioctl(inet_sock, SIOCGIFADDR, &ifr) < 0)
        perror("ioctl");
    char *ip = inet_ntoa(((struct sockaddr_in *)&(ifr.ifr_addr))->sin_addr);
    printf("%s\n", inet_ntoa(((struct sockaddr_in *)&(ifr.ifr_addr))->sin_addr));
    int i = 0;
    while (*ip != 0)
    {
        IP[i] = *ip;
        ip++;
        printf("%c\n", IP[i]);
        i++;
    }
}

/**
 * @Author: 王占坤
 * @Description: 不允许外部调用.用于重用端口、地址
 * @Param: socketfd skf 传入需要进行服用的socketfd
 * @Return: NULL
 * @Throw: 
 */
void Socket::reUse(socketfd skf)
{
    int optval = 1;
    int res = setsockopt(skf, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
    if (res == -1)
    {
        perror("setsockopt");
    }
}

/**
 * @Author: 王占坤
 * @Description: 不允许外部调用。用于阻塞等待连接请求。
 * @Param: socketfd ser_skf 用于等待连接请求的socketfd句柄
 * @Return: 
 * @Throw: 
 */
int Socket::recvMSGFromClient(socketfd ser_skf)
{
    socketfd sock = acceptConnection(ser_skf, NULL, NULL);
    receiveMSG(sock, data, sizeof(data), 0);
    close(sock);
}

/**
 * @Author: 王占坤
 * @Description: 用于客户端发送消息到接收方的接口
 * @Param: 
 * @Return: 
 * @Throw: 
 */
int Socket::sendToServer(char *data)
{
    char buf[BUFFER_SIZE];
    strcpy(buf, data);
    socketfd skf = createSocket(this->mode, 0);
    struct sockaddr_in ser_addr;
    initialzeSocketaddr(&ser_addr, this->ip, this->port);
    createConnection(skf, (struct sockaddr *)&ser_addr, sizeof(ser_addr));
    sendMSG(skf, data, BUFFER_SIZE, 0);
    close(skf);
}

/**
 * @Author: 王占坤
 * @Description: 在skf上创建一个到serv_addr的连接
 * @Param: socketfd skf, 需要创建连接的socket句柄
 * @Param: struct sockaddr* serv_addr 连接的目标地址
 * @Param: int addr_len 目标地址大小
 * @Return: 
 * @Throw: 
 */
void Socket::createConnection(socketfd skf, struct sockaddr *serv_addr, size_t addr_len)
{
    if (serv_addr == NULL)
    {
        printf("地址不存在！\n");
    }
    int res = connect(skf, serv_addr, addr_len);
    if (res == -1)
    {
        perror("connect");
        printf("create connection error: %s(errno: %d)\n", strerror(errno), errno);
    }
}

/**
 * @Author: 王占坤
 * @Description: 监听到有连接时进行连接
 * @Param: 
 * @Return: 
 * @Throw: 
 */

socketfd Socket::acceptConnection(socketfd sfk, struct sockaddr *addr, socklen_t len_addr)
{
    //判断是否连接如果可以连接返回新建的
    socketfd cli_skf = accept(sfk, addr, &len_addr);
    if (cli_skf == -1)
    {
        printf("accept socket error: %s(errno: %d)", strerror(errno), errno);
    }
    return cli_skf;
}

/**
 * @Author: 王占坤
 * @Description: 从buff向skf发送n_bytes个字节的数据
 * @Param: socketfd skf  目标socket
 * @Param: char* buff  缓冲区
 * @Param: size_t n_bytes 要发送的字节数目
 * @Param: flag 一般默认为0即可
 * @Return: int 0:成功
 * @Throw: 
 */
int Socket::sendMSG(socketfd skf, char *buff, size_t n_bytes, int flag)
{
    if (n_bytes >= BUFFER_SIZE)
        n_bytes = BUFFER_SIZE - 1;
    if (n_bytes < 0)
        n_bytes = 0;

    int num_of_sending_words = send(skf, buff, n_bytes, flag);
    if (num_of_sending_words == -1) //失败
    {
        printf("receive message error: %s(errno: %d)\n", strerror(errno), errno);
    }
    return 0;
}

/**
 * @Author: 王占坤
 * @Description: 从socket接受信息放到buff区，最大字节数为max_size
 * @Param: socketfd skf  信息源socket
 * @Param: char *buff  用于存储的缓冲
 * @Param: size_t n_bytes 最大接受多少字节的信息
 * @Param: int flag 一般传入为0即可
 * @Return: int 0:成功
 * @Throw: 
 */

int Socket::receiveMSG(socketfd skf, char *buff, size_t n_bytes, int flag)
{
    memset(buff, 0, BUFFER_SIZE);
    if (n_bytes > BUFFER_SIZE)
        n_bytes = BUFFER_SIZE - 1;
    int num_of_reading_words = recv(skf, buff, n_bytes, flag);
    // printf("num_of_reading_words: %d\n", num_of_reading_words);
    // printf("%s\n", buff);
    if (num_of_reading_words == -1) //失败
    {
        printf("receive message error: %s(errno: %d)\n", strerror(errno), errno);
    }

    if (num_of_reading_words >= BUFFER_SIZE)
        num_of_reading_words = BUFFER_SIZE - 1;

    buff[num_of_reading_words] = 0;
    return 0;
}

/**
  * @Author: 王占坤
  * @Description: 向socket回传数据
  * @Param: socketfd skf 目标socket
  * @Param: const void* buff  需要发送的数据指针
  * @Param: size_t n_bytes  需要发送的字节数
  * @Param: int flag  一般传入0即可
  * @Return: void
  * @Throw: 
  */
void Socket::writeBack(socketfd skf, const void *buff, size_t n_bytes, int flag)
{
    if (!fork())
    {
        int res = send(skf, buff, n_bytes, flag);
        if (res == -1)
        {
            printf("write back error: %s(errno: %d)", strerror(errno), errno);
        }
    }
}

#endif