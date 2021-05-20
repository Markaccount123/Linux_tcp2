#pragma once 

#include<iostream>
#include<cstdlib>
#include<cstring>
#include<unistd.h>
#include<string>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<signal.h>

#define BACKLOG 5 //一般这个的值都设置的比较小，表示的意思是此时底层链接队列中等待链接的长度
class tcpServer{
  private:
    int port;
    int lsock;//表示监听的套接字
  public:

    tcpServer(int _port = 8080,int _lsock = -1)
      :port(_port),lsock(_lsock)
    {}

    void initServer()
    {
      signal(SIGCHLD,SIG_IGN); //采用忽略的方式
      lsock = socket(AF_INET,SOCK_STREAM,0);
      if(lsock < 0){
        std::cerr<<"socket error"<<std::endl;
        exit(2);
      }
      struct sockaddr_in local;
      local.sin_family = AF_INET;
      local.sin_port = htons(port);
      local.sin_addr.s_addr = htonl(INADDR_ANY);

      if(bind(lsock,(struct sockaddr*)&local,sizeof(local)) < 0){
        std::cerr<<"bind error!"<<std::endl;
        exit(3);
      }

      //走到这里说明bind成功了，但是tcp是面向链接的，所以还需要一个接口
      if(listen(lsock,BACKLOG) < 0){
        std::cout<<"listen error!"<<std::endl;
        exit(4);
      } 
      
      //tcp必须要将套接字设置为监听状态  这里可以想夜间你去吃饭，什么时候去都可以吃到，是为什么？是因为店里面一直有人在等待着你
      //任意时间有客户端来连接我
      //成功返回0，失败返回-1
    }

    //echo服务器
    void service(int sock)
    {
      while(true){
        //打开一个文件，也叫打开一个流，所以这里其实使用read和write也是可以的，但是这里是网络，最好使用tcp的recv和send
        char buf[64];
        ssize_t s = recv(sock,buf,sizeof(buf)-1,0);
        if(s > 0){
          buf[s] = '\0';
          std::cout<<"Client# "<< buf <<std::endl;
          send(sock,buf,strlen(buf),0);//此时的网络就是文件，你往文件中发送东西的格式，可不是以\0作为结束，加入\0会出现乱码的情况
        }
        else if(s == 0){
          std::cout<<"Client quit ..."<<std::endl;
          break;
        }else{
          std::cerr<<"recv error"<<std::endl;
          break;
        }
      }
      close(sock);
    }

    void start()
    {
      struct sockaddr_in endpoint;
      while(true){
        //第一步应该获取链接
        //这里可以理解为 饭店拉客
        //lsock 是负责拉客的   accept的返回值是主要负责服务客户的
        socklen_t len = sizeof(endpoint);
        int sock = accept(lsock,(struct sockaddr*)&endpoint,&len);
        if(sock < 0){
          std::cerr<<"accept error!"<<std::endl;
          continue; //相当于拉客失败了，我需要继续拉客
        }
        //打印获取到的连接的ip和端口号
        std::string cli_info = inet_ntoa(endpoint.sin_addr);
        cli_info += ":";
        cli_info += std::to_string(endpoint.sin_port); //这里获取的端口号是一个整数，需要把该整数转换为字符串,当然这里也可以使用stringstream 比如定义一个对象 ， 此时有一个int变量，一个string 对象 你把int输入stringstream 然后再把stringstream在输入string的对象中，就进行了转换
        //stringstream ss;
        //int a = 100;
        //string b ;
        //ss << a;
        //ss >> b;  这样就进行了整形转字符串的功能
        std::cout<<"get a new link "<< cli_info<<" sock: "<<sock<<std::endl;

        pid_t id = fork();
        if(id == 0){
          //说明这是子进程
          //此时我让子进程来进行对应的处理服务
          //对于子进程来说始会继承父进程的一整套东西的，那么相对应的也会继承他的lsock和sock，那么对于子进程更关心sock，所以还需要把lsock进行关闭
          close(lsock);
          service(sock);
          exit(0);//子进程运行完毕（执行完相应的任务）以后有可能会继续运行下去，所以让他退出
        }

        //但是问题是，我还需要父进程在这里等待
        //waitpid(id,NULL,0);父进程在这里阻塞等待，当时父进程可以等待吗？不行呀，因为父进程需要去接受新来的连接。那么就不等待子进程的消息了吗？那不行，还有一种方式就是
        //对于子进程退出，其实是会给父进程发送一个SIG_CHILD的信号的，那么当父进程将收到的SIG_CHILD进行忽略的话，子进程退出的时候，系统会自动回收子进程
        //此时就是对我拉上来的客人进行服务
        
        //父进程不关心sock
        close(sock);//不要想不通，因为他们使用的是不同的文件描述符表
      }
    }

    ~tcpServer()
    {}
};
