#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <string>
#include <vector>
#include <exception>

class SocketException: public std::exception
{
    SocketException(std::string error):
        m_error(error)
    {
    }
    const char *what() const throw() override
    {
        return m_error.c_str();
    }
private:
    std::string m_error;
};

class TcpSocket
{
public:
    TcpSocket();
    TcpSocket(int fd);
    TcpSocket(const TcpSocket &other) = delete;
    TcpSocket(TcpSocket &&other);
    TcpSocket &operator =(const TcpSocket &other) = delete;
    TcpSocket &operator =(TcpSocket &&other);
    ~TcpSocket();
    TcpSocket *accept();
    std::vector<char> read();
    bool listen(ushort port);
    bool connect(std::string host, ushort port);
    int write(const std::vector<char> &data);
    void close();
    int fd() const {return m_desc;}
private:
    int m_desc;
};

#endif // TCPSOCKET_H
