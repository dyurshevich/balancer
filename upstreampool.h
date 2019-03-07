#ifndef UPSTREAMPOOL_H
#define UPSTREAMPOOL_H

#include <string>
#include <vector>
#include <chrono>

class TcpSocket;

class UpstreamPool {
public:
    UpstreamPool();
    void setUpstreams(const std::vector<std::string> &value, ushort port);
    void deactivate(const std::string &host);
    TcpSocket *getUpstream();
    //TODO create method bool upstreamAvailable();
private:
    struct Upstream {
        Upstream(std::string host):
            host(host),
            isActive(true),
            inactiveSince(std::chrono::system_clock::duration::min())
        {
        }
        std::string host;
        bool isActive;
        std::chrono::time_point<std::chrono::system_clock> inactiveSince;
    };
    void deactivate(std::vector<Upstream>::iterator it);
    std::vector<Upstream> m_upstreams;
    ushort m_port;
    std::vector<Upstream>::iterator m_currentUpstream;
};
#endif // UPSTREAMPOOL_H
