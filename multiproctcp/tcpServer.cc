#include"tcpServer.hpp"

void Usage(std::string proc)
{
  std::cout<<"Usage: " << proc<< "port"<<std::endl;
}

int main(int argc,char *argv[])
{
  if(argc != 2)
  {
    Usage(argv[0]);
    exit(1);
  }

  tcpServer *ts = new tcpServer(atoi(argv[1]));
  ts->initServer();
  ts->start();

  delete ts;
}
