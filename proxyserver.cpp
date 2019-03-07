#include <iostream>
#include <thread>
#include <memory>
#include "upstreampool.h"
#include "proxyserver.h"

using namespace std;
using namespace std::chrono;

static UpstreamPool upstreams;
static TcpSocket socket;

void ProxyServer::loop()
{
    for (;;) {
        unique_ptr<TcpSocket> sock {socket.accept()};
        unique_ptr<TcpSocket> upstream {upstreams.getUpstream()};
        if ( !(sock.get()) ) {
            break;
        }
        if ( !(upstream.get())) {
            continue;
        }

        vector<char> msg = sock->read();
        upstream->write(msg);
        vector<char> answer = upstream->read();
        sock->write(answer);
    }
}

ProxyServer::ProxyServer()
{
}

void ProxyServer::setUpstreams(const std::vector<string> &value)
{
    upstreams.setUpstreams(value, m_port);
}

void ProxyServer::start()
{
    if (socket.listen(m_port)) {
        thread t(loop);
        t.join();
    } else {
        cout << "Unable to listen on specified port" << endl;
    }
}

void ProxyServer::stop()
{
    socket.close(); //causes the server loop to exit
}

