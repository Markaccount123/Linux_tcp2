FLAG=-std=c++11

.PHONY:all
all:tcpClient tcpServer
tcpClient:tcpClient.cc
	g++ -o $@ $^ $(FLAG)
tcpServer:tcpServer.cc
	g++ -o $@ $^ $(FLAG)
.PHONY:clean
clean:
	rm -f tcpClient tcpServer 
