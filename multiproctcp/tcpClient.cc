#include"tcpClient.hpp"
void Usage(std::string proc)
{
  std::cout<< "Usage: " << "\n";
  std::cout <<"\t"<<"serv_ip serv_port"<< std::endl;
}

int main(int argc,char *argv[])
{
  if(argc != 3){
    Usage(argv[0]);
    exit(1);
  }
  tcpClient *tc = new tcpClient(argv[1],atoi(argv[2]));
  tc->initClient();
  tc->start();

  delete tc;
  return 0;
}
