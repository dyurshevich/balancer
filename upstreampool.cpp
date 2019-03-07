#include "upstreampool.h"
#include "tcpsocket.h"
#include <algorithm>
#include <iostream>

using namespace std;
using namespace std::chrono;

static seconds REACTIVATE_TIME {100};

UpstreamPool::UpstreamPool()
{
}

void UpstreamPool::setUpstreams(const std::vector<std::string> &value, ushort port)
{
    m_port = port;
    m_upstreams.clear();
    for (const auto &upstream: value) {
        m_upstreams.emplace_back(upstream);
    }
    m_currentUpstream = m_upstreams.begin();
}

void UpstreamPool::deactivate(const std::string &host)
{
    auto it = std::find_if(m_upstreams.begin(), m_upstreams.end(),
                           [host](const Upstream& value) -> bool {
        return value.host == host;
    });
    if (it != m_upstreams.end()) {
        deactivate(it);
    }
}

void UpstreamPool::deactivate(vector<Upstream>::iterator it)
{
    it->isActive = false;
    it->inactiveSince = system_clock::now();
}

TcpSocket *UpstreamPool::getUpstream()
{
    if (m_upstreams.size() == 0) {
        return nullptr;
        //throw except
    }

    auto searchStart = m_currentUpstream;
    auto searchCurrent = searchStart;
    do {
        bool reactivate = REACTIVATE_TIME < (system_clock::now() - searchCurrent->inactiveSince);
        if (searchCurrent->isActive || reactivate) {
            cout << "Found good upstream " << searchCurrent->host << endl;
            TcpSocket *sock = new TcpSocket;
            if (sock->connect(searchCurrent->host, m_port)) {
                cout << "Connected to upstream " << searchCurrent->host << endl;
                searchCurrent->isActive = true;
                m_currentUpstream++;
                if (m_currentUpstream == m_upstreams.end()) {
                    m_currentUpstream = m_upstreams.begin();
                }
                return sock;
            }
            delete sock;
            cout << "Failed to connect to upstream " << searchCurrent->host << endl;
            deactivate(searchCurrent);
        }
        if (searchCurrent == m_upstreams.end()-1) {
            searchCurrent = m_upstreams.begin();
        } else {
            searchCurrent++;
        }
    } while(searchCurrent->host != searchStart->host);

    cout << "No upstreams found" << endl;

    return nullptr;
}
