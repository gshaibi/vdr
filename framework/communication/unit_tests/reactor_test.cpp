#include "reactor.hpp"
#include "../include/unit_test.hpp"

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <sys/types.h>

#define TCP_PORT 2000
#define UDP_PORT 3000

using ilrd::Reactor;

struct ReadFd
{
  ReadFd(Reactor &r_) : m_r(r_) {}
  void operator()(int fd_)
  {
    std::memset(m_buffer, 0, sizeof(m_buffer));
    if (read(fd_, m_buffer, sizeof(m_buffer)) == 0)
    {
      std::cout << "Closing socket" << std::endl;
      close(fd_);
      m_r.RemFD(fd_, Reactor::READ);
      return;
    }
    if (std::strncmp(m_buffer, "stop", 4) == 0)
    {
      m_r.Stop();
    }
    std::cout << m_buffer << std::endl;
  }
  char m_buffer[100];
  Reactor &m_r;
};

class AcceptConnection
{
public:
  AcceptConnection(Reactor &r_) : m_r(r_) {}

  void operator()(int fd_)
  {
    struct sockaddr_in port;
    socklen_t socket_len = sizeof(port);

    int accepted_fd = accept(fd_, (sockaddr *)&port, &socket_len);

    if (-1 != accepted_fd)
    {
      m_r.AddFD(accepted_fd, Reactor::READ, ReadFd(m_r));
    }
  }

private:
  Reactor &m_r;
};

TestResult Overall()
{
  Reactor r;

  int listen_sock = socket(PF_INET, SOCK_STREAM, 0);
  int udp_sock = socket(PF_INET, SOCK_DGRAM, 0);

  REQUIRE(-1 != listen_sock);
  REQUIRE(-1 != udp_sock);

  sockaddr_in selfAddr;
  selfAddr.sin_family = AF_INET;
  selfAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  selfAddr.sin_port = htons(TCP_PORT);

  sockaddr_in udpIncome;
  udpIncome.sin_family = AF_INET;
  udpIncome.sin_addr.s_addr = htonl(INADDR_ANY);
  udpIncome.sin_port = htons(UDP_PORT);

  REQUIRE(-1 != bind(udp_sock, (sockaddr *)&udpIncome, sizeof(udpIncome)));

  REQUIRE(-1 != bind(listen_sock, (sockaddr *)&selfAddr, socklen_t(sizeof(selfAddr))));
  listen(listen_sock, 10);

  r.AddFD(STDIN_FILENO, Reactor::READ, ReadFd(r));
  r.AddFD(listen_sock, Reactor::READ, AcceptConnection(r));
  r.AddFD(udp_sock, Reactor::READ, ReadFd(r));

  std::cout << "type 'stop' to stop" << std::endl;
  r.Start();

  return SUCCESS;
}

int main()
{
  RUNTEST(Overall);
  return 0;
}
