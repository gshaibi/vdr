#include "tcp_reader.hpp"
#include "unit_test.hpp"
#include <algorithm>
#include <iostream>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <sys/socket.h>
#include <sys/types.h>

#define TCP_PORT 2000

using namespace std;
using namespace ilrd;

void PrintChar(char c) { cout << c; }

size_t OnHeader(boost::shared_ptr<std::vector<char>> header_)
{
    cout << ("Header recieved!") << endl;

    std::cout << "Header:" << std::endl;
    for_each(header_->begin(), header_->end(), PrintChar);

    return 50;
}

void OnPacket(boost::shared_ptr<std::vector<char>> packet_)
{
    cout << ("Whole packet recieved!") << endl << endl;

    std::cout << "Header + packet:" << std::endl;

    for_each(packet_->begin(), packet_->end(), PrintChar);

    std::cout << std::endl;

    std::cout << "-------------------------------------------------------------"
              << std::endl;
}

TestResult Overall()
{
    Reactor react;

    int listen_sock = socket(PF_INET, SOCK_STREAM, 0);

    REQUIRE(-1 != listen_sock);

    sockaddr_in selfAddr;
    selfAddr.sin_family = AF_INET;
    selfAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    selfAddr.sin_port = htons(TCP_PORT);

    int reuse_enable = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &reuse_enable,
               sizeof(reuse_enable));

    REQUIRE(-1 != bind(listen_sock, (sockaddr *)&selfAddr,
                       socklen_t(sizeof(selfAddr))));
    listen(listen_sock, 10);

    std::cout << "Please connect through TCP port 2000 and start sending"
              << std::endl;
    int data_sock = accept(listen_sock, NULL, NULL);

    REQUIRE(data_sock != -1);

    TcpReader r(data_sock, 10, react, OnHeader, OnPacket);

    react.Start();

    return SUCCESS;
}

int main()
{
    RUNTEST(Overall);
    return 0;
}
