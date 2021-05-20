#pragma once

#include<iostream>
#include<string>
#include<cstring>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
class tcpClient{
  private:
    std::string serv_ip;
    int serv_port;
    int sock;
  public:
    tcpClient(std::string _ip = "127.0.0.1",int _port = 8080,int _sock = -1)
      :serv_ip(_ip),serv_port(_port),sock(_sock)
    {}

    void initClient()
    {
      sock = socket(AF_INET,SOCK_STREAM,0);
      if(sock < 0){
        std::cerr<<"socket error"<<std::endl;
        exit(2);
      }

      struct sockaddr_in serv_point;
      serv_point.sin_family = AF_INET;
      serv_point.sin_port = htons(serv_port);
      serv_point.sin_addr.s_addr = inet_addr(serv_ip.c_str());

      //成功返回0，失败返回-1
      if(connect(sock,(struct sockaddr*)&serv_point,sizeof(serv_point)) < 0){
        std::cerr<<"connect error"<<std::endl;
      }
      
    }

    void start()
    {
      char msg[64];
      while(true){
        std::cout<<"Please Enter Massage# ";
        //这里有一个用户层的c/c++的缓冲区，所以需要强制刷新，这样就不会出现第一行不打印这句的问题
        fflush(stdin);
        //对于Client端首先应该发送数据，然后在考虑接收
        ssize_t s = read(0,msg,sizeof(msg)-1);//从标准输入中读取数据
        //因为cin不会会把回车键给过滤掉，但是read又会获得回车键
        if(s > 0){
          msg[s-1] = '\0';//这一步可以把读到的回车键给过滤掉，这样显示的时候，就不会出现空行
          send(sock,msg,strlen(msg),0);
          ssize_t ss = recv(sock,msg,sizeof(msg)-1,0);
          if(ss > 0){
            msg[ss] = '\0';
            std::cout<<"Server echo # " << msg<<std::endl;
          }
        }
      }
    }

    ~tcpClient()
    {
      close(sock);
    }

};


