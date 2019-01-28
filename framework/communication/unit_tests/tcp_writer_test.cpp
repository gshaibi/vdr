#include "tcp_writer.hpp"
#include "unit_test.hpp"
#include <iostream>
#include <cstdio>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <algorithm>

#define TCP_OUT 2000

using namespace ilrd;

int EstablishConnection()
{
  int data_socket = socket(AF_INET, SOCK_STREAM, 0);

  sockaddr_in server;
	server.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &server.sin_addr.s_addr);
	server.sin_port = htons(TCP_OUT);

	connect(data_socket, (struct sockaddr *)&server, sizeof(server));

  return data_socket;
}

TestResult Overall()
{
  boost::shared_ptr<std::vector<char> > packet1(new std::vector<char>(1000000));
  boost::shared_ptr<std::vector<char> > packet2(new std::vector<char>(1000000));
  
  fill(packet1->begin(), packet1->end(), 'A');
  fill(packet2->begin(), packet2->end(), 'Z');

  int fd = EstablishConnection();

  Reactor r;
  TcpWriter writer(fd, r);

  writer.Write(packet1);
  writer.Write(packet2);

  r.Start();

  return SUCCESS;
}

int main()
{
  RUNTEST(Overall);
  return 0;
}
