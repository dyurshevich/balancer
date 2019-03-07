#ifndef PROXYSERVER_H
#define PROXYSERVER_H

#include "tcpsocket.h"

class ProxyServer
{
public:
    ProxyServer();
    void setUpstreams(const std::vector<std::string> &value);
    void setPort(ushort value) {m_port = value;}
    void start();
    void stop();
private:
    ushort m_port;
    static void loop();
};

#endif // PROXYSERVER_H
