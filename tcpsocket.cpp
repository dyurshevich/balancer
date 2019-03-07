#include "tcpsocket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include <cstring>

static const int MAXPACKLENGTH = 16384;

using namespace std;

TcpSocket::TcpSocket()
{
    m_desc = socket(AF_INET, SOCK_STREAM, 0);
}

TcpSocket::TcpSocket(int fd):
    m_desc(fd)
{
}

TcpSocket::TcpSocket(TcpSocket &&other):
    m_desc(other.m_desc)
{
    m_desc = -1;
}

TcpSocket &TcpSocket::operator =(TcpSocket &&other)
{
    m_desc = other.m_desc;
    other.m_desc = -1;
}

TcpSocket::~TcpSocket()
{
    close();
}

bool TcpSocket::listen(ushort port)
{
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr=htonl(INADDR_ANY);

    if (bind(m_desc, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
        cout << "Bind Error " << errno << endl;
        return false;
    }

    if (::listen(m_desc, 5) == -1) {
        cout << "Socket listen error " << errno << endl;
        return false;
    }

    cout << "Socket is listening on port " << port << endl;
    return true;
}

bool TcpSocket::connect(string host, ushort port)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_INET;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;

    struct addrinfo *res;
    int s = getaddrinfo(host.data(), to_string(port).data(), &hints, &res);
    for (struct addrinfo *rp = res; rp != nullptr; rp = rp->ai_next) {
        if (::connect(m_desc, rp->ai_addr, rp->ai_addrlen) != -1) {
            return true;
        }
    }
    return false;
}

int TcpSocket::write(const vector<char> &data)
{
    return ::write(m_desc, data.data(), data.size());
}

vector<char> TcpSocket::read()
{
    vector<char> msg(0);
    char package[MAXPACKLENGTH];

    int n = 0;
    while (n = ::read(m_desc, package, MAXPACKLENGTH)) {
        msg.resize(msg.size() + n, 0);
        copy(package, package+n, msg.end() - n);
        if (n < MAXPACKLENGTH) {
            break;
        }
    }

    return msg;
}

void TcpSocket::close()
{
    shutdown(m_desc, SHUT_RD);
    ::close(m_desc);
}

TcpSocket *TcpSocket::accept()
{
    int acceptedFd = ::accept(m_desc, 0, 0);
    if (acceptedFd != -1) {
        return new TcpSocket(acceptedFd);
    } else {
        return nullptr;
    }
}
